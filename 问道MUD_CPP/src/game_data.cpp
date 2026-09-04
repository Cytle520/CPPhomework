#include "game_data.h"
#include <fstream>
#include <sstream>
#include <algorithm>

// ============================================================
// 字符串工具
// ============================================================
std::string GameData::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<std::string> GameData::split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(trim(item));
    }
    return result;
}

// ============================================================
// INI 解析器
// 格式：[section] 开头，key=value，#开头为注释
// 同一个key多次出现时，所有值按顺序保存
// ============================================================
std::map<std::string, std::map<std::string, std::vector<std::string>>>
GameData::parse_ini(const std::string& filepath) {
    std::map<std::string, std::map<std::string, std::vector<std::string>>> result;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[警告] 无法打开文件: " << filepath << std::endl;
        return result;
    }
    std::string line;
    std::string current_section;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        // section
        if (line[0] == '[' && line.back() == ']') {
            current_section = line.substr(1, line.size() - 2);
            continue;
        }
        // key=value
        size_t eq = line.find('=');
        if (eq != std::string::npos && !current_section.empty()) {
            std::string key = trim(line.substr(0, eq));
            std::string value = trim(line.substr(eq + 1));
            result[current_section][key].push_back(value);
        }
    }
    return result;
}

// ============================================================
// 加载全部数据
// ============================================================
bool GameData::load_all(const std::string& data_dir) {
    load_maps(data_dir);
    load_npcs(data_dir);
    load_items(data_dir);
    load_skills(data_dir);
    load_levels(data_dir);
    load_story(data_dir);

    std::cout << "[加载] 地图房间: " << rooms.size() << " 个" << std::endl;
    std::cout << "[加载] NPC: " << npcs.size() << " 个" << std::endl;
    std::cout << "[加载] 物品: " << items.size() << " 个" << std::endl;
    std::cout << "[加载] 技能: " << skills.size() << " 个" << std::endl;
    std::cout << "[加载] 境界: " << levels.size() << " 个" << std::endl;
    std::cout << "[加载] 剧情章节: " << chapters.size() << " 个" << std::endl;
    return !rooms.empty();
}

// ============================================================
// 加载地图
// ============================================================
void GameData::load_maps(const std::string& dir) {
    auto data = parse_ini(dir + "/maps.txt");
    for (auto& [id, kv] : data) {
        RoomData room;
        room.id = id;
        if (kv.count("name")) room.name = kv["name"][0];
        if (kv.count("desc")) room.desc = kv["desc"][0];
        // exits: key以exit开头
        for (auto& [key, vals] : kv) {
            if (key.substr(0, 4) == "exit") {
                std::string direction = key.substr(4);
                for (auto& v : vals) {
                    room.exits.push_back({direction, v});
                }
            }
        }
        if (kv.count("npc")) room.npcs = kv["npc"];
        if (kv.count("item")) room.items = kv["item"];
        if (kv.count("start") && kv["start"][0] == "1") {
            start_room = id;
        }
        rooms[id] = room;
    }
}

// ============================================================
// 加载NPC
// ============================================================
void GameData::load_npcs(const std::string& dir) {
    auto data = parse_ini(dir + "/npcs.txt");
    for (auto& [id, kv] : data) {
        NPCData npc;
        npc.id = id;
        if (kv.count("name")) npc.name = kv["name"][0];
        if (kv.count("title")) npc.title = kv["title"][0];
        if (kv.count("desc")) npc.desc = kv["desc"][0];
        if (kv.count("type")) npc.type = kv["type"][0];
        if (kv.count("level")) npc.level = std::stoi(kv["level"][0]);
        if (kv.count("hp")) npc.hp = std::stoi(kv["hp"][0]);
        if (kv.count("attack")) npc.attack = std::stoi(kv["attack"][0]);
        if (kv.count("defense")) npc.defense = std::stoi(kv["defense"][0]);
        if (kv.count("exp")) npc.exp = std::stoi(kv["exp"][0]);
        if (kv.count("dialogue")) npc.dialogues = kv["dialogue"];
        if (kv.count("defeat")) npc.defeat_dialogue = kv["defeat"][0];
        if (kv.count("skill")) npc.skills = kv["skill"];
        if (kv.count("drop")) npc.drops = kv["drop"];
        if (kv.count("shop")) npc.shop_items = kv["shop"];
        npcs[id] = npc;
    }
}

// ============================================================
// 加载物品
// ============================================================
void GameData::load_items(const std::string& dir) {
    auto data = parse_ini(dir + "/items.txt");
    for (auto& [id, kv] : data) {
        ItemData item;
        item.id = id;
        if (kv.count("name")) item.name = kv["name"][0];
        if (kv.count("type")) item.type = kv["type"][0];
        if (kv.count("desc")) item.desc = kv["desc"][0];
        if (kv.count("slot")) item.slot = kv["slot"][0];
        if (kv.count("usable")) item.usable = (kv["usable"][0] == "1");
        if (kv.count("price")) item.price = std::stoi(kv["price"][0]);
        // effect 格式: type:value 或 type:stat:value:duration
        if (kv.count("effect")) {
            auto parts = split(kv["effect"][0], ':');
            if (!parts.empty()) item.effect_type = parts[0];
            if (parts.size() >= 2) {
                if (item.effect_type == "buff") {
                    item.effect_stat = parts[1];
                    if (parts.size() >= 3) item.effect_value = std::stoi(parts[2]);
                    if (parts.size() >= 4) item.effect_duration = std::stoi(parts[3]);
                } else {
                    item.effect_value = std::stoi(parts[1]);
                }
            }
        }
        items[id] = item;
    }
}

// ============================================================
// 加载技能
// ============================================================
void GameData::load_skills(const std::string& dir) {
    auto data = parse_ini(dir + "/skills.txt");
    for (auto& [id, kv] : data) {
        SkillData skill;
        skill.id = id;
        if (kv.count("name")) skill.name = kv["name"][0];
        if (kv.count("desc")) skill.desc = kv["desc"][0];
        if (kv.count("type")) skill.type = kv["type"][0];
        if (kv.count("damage")) skill.damage = std::stod(kv["damage"][0]);
        if (kv.count("cost")) skill.cost = std::stoi(kv["cost"][0]);
        if (kv.count("heal")) skill.heal = std::stoi(kv["heal"][0]);
        if (kv.count("buff_stat")) skill.buff_stat = kv["buff_stat"][0];
        if (kv.count("buff_value")) skill.buff_value = std::stoi(kv["buff_value"][0]);
        if (kv.count("duration")) skill.duration = std::stoi(kv["duration"][0]);
        if (kv.count("bonus_def")) skill.bonus_def = std::stoi(kv["bonus_def"][0]);
        if (kv.count("unlock")) skill.unlock_level = std::stoi(kv["unlock"][0]);
        skills[id] = skill;
    }
}

// ============================================================
// 加载等级
// ============================================================
void GameData::load_levels(const std::string& dir) {
    auto data = parse_ini(dir + "/levels.txt");
    // base stats
    if (data.count("base")) {
        auto& b = data["base"];
        if (b.count("hp")) base_stats.hp = std::stoi(b["hp"][0]);
        if (b.count("mp")) base_stats.mp = std::stoi(b["mp"][0]);
        if (b.count("attack")) base_stats.attack = std::stoi(b["attack"][0]);
        if (b.count("defense")) base_stats.defense = std::stoi(b["defense"][0]);
        if (b.count("money")) base_stats.money = std::stoi(b["money"][0]);
    }
    // levels
    for (int i = 1; i <= 20; i++) {
        std::string key = "level" + std::to_string(i);
        if (!data.count(key)) break;
        auto& kv = data[key];
        LevelData lv;
        lv.level = i;
        if (kv.count("name")) lv.name = kv["name"][0];
        if (kv.count("exp")) lv.exp_required = std::stoi(kv["exp"][0]);
        if (kv.count("hp_grow")) lv.hp_grow = std::stoi(kv["hp_grow"][0]);
        if (kv.count("mp_grow")) lv.mp_grow = std::stoi(kv["mp_grow"][0]);
        if (kv.count("atk_grow")) lv.atk_grow = std::stoi(kv["atk_grow"][0]);
        if (kv.count("def_grow")) lv.def_grow = std::stoi(kv["def_grow"][0]);
        if (kv.count("desc")) lv.desc = kv["desc"][0];
        if (kv.count("unlock")) lv.unlock_skills = kv["unlock"];
        levels.push_back(lv);
    }
}

// ============================================================
// 加载剧情
// ============================================================
void GameData::load_story(const std::string& dir) {
    auto data = parse_ini(dir + "/story.txt");
    for (auto& [id, kv] : data) {
        StoryChapter chapter;
        chapter.id = id;
        if (kv.count("title")) chapter.title = kv["title"][0];
        // 步骤：stepN_xxx
        // 先找出最大的步骤号
        int max_step = 0;
        for (auto& [key, vals] : kv) {
            if (key.substr(0, 4) == "step") {
                size_t us = key.find('_');
                if (us != std::string::npos) {
                    int n = std::stoi(key.substr(4, us - 4));
                    max_step = std::max(max_step, n);
                }
            }
        }
        for (int n = 1; n <= max_step; n++) {
            StoryStep step;
            std::string prefix = "step" + std::to_string(n) + "_";
            for (auto& [key, vals] : kv) {
                if (key.substr(0, prefix.size()) == prefix) {
                    std::string field = key.substr(prefix.size());
                    if (field == "text") step.text = vals[0];
                    else if (field == "objective") step.objective = vals[0];
                    else if (field == "type") step.type = vals[0];
                    else if (field == "target") step.target = vals[0];
                    else if (field == "target_npc") step.target_npc = vals[0];
                    else if (field == "dialogue") step.dialogue = vals[0];
                    else if (field == "reward_exp") step.reward_exp = std::stoi(vals[0]);
                    else if (field == "reward_item") step.reward_item = vals[0];
                }
            }
            if (!step.type.empty()) {
                chapter.steps.push_back(step);
            }
        }
        chapters.push_back(chapter);
    }
}

// ============================================================
// 查询接口
// ============================================================
RoomData* GameData::get_room(const std::string& id) {
    auto it = rooms.find(id);
    return it != rooms.end() ? &it->second : nullptr;
}

NPCData* GameData::get_npc(const std::string& id) {
    auto it = npcs.find(id);
    return it != npcs.end() ? &it->second : nullptr;
}

ItemData* GameData::get_item(const std::string& id) {
    auto it = items.find(id);
    return it != items.end() ? &it->second : nullptr;
}

SkillData* GameData::get_skill(const std::string& id) {
    auto it = skills.find(id);
    return it != skills.end() ? &it->second : nullptr;
}

LevelData* GameData::get_level(int level) {
    if (level >= 1 && level <= (int)levels.size()) {
        return &levels[level - 1];
    }
    return nullptr;
}
