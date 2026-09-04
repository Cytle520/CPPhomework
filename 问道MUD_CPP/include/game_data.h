#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

// ============================================================
// 数据结构定义
// 所有游戏数据都从 data/ 目录的文本文件加载
// 修改游戏内容只需编辑 data/ 下的文件，无需改动代码
// ============================================================

// 物品数据
struct ItemData {
    std::string id;
    std::string name;
    std::string type;        // consumable / equipment / quest
    std::string desc;
    std::string slot;        // weapon / armor（装备用）
    bool usable = false;
    int price = 0;
    // effect 解析后存储
    std::string effect_type; // heal / buff / exp
    std::string effect_stat; // attack / defense
    int effect_value = 0;
    int effect_duration = 0;
};

// 技能数据
struct SkillData {
    std::string id;
    std::string name;
    std::string desc;
    std::string type;        // attack / heal / buff
    double damage = 1.0;
    int cost = 0;
    int heal = 0;
    std::string buff_stat;
    int buff_value = 0;
    int duration = 0;
    int bonus_def = 0;
    int unlock_level = 1;
};

// NPC数据
struct NPCData {
    std::string id;
    std::string name;
    std::string title;
    std::string desc;
    std::string type;        // mentor / enemy / merchant / npc
    int level = 1;
    int hp = 50;
    int attack = 5;
    int defense = 3;
    int exp = 0;
    std::vector<std::string> dialogues;
    std::string defeat_dialogue;
    std::vector<std::string> skills;
    std::vector<std::string> drops;
    std::vector<std::string> shop_items;
};

// 房间数据
struct RoomData {
    std::string id;
    std::string name;
    std::string desc;
    std::vector<std::pair<std::string, std::string>> exits; // 方向 -> 房间id
    std::vector<std::string> npcs;
    std::vector<std::string> items;
};

// 等级数据
struct LevelData {
    int level = 1;
    std::string name;
    int exp_required = 0;
    int hp_grow = 10;
    int mp_grow = 5;
    int atk_grow = 2;
    int def_grow = 1;
    std::string desc;
    std::vector<std::string> unlock_skills;
};

// 剧情步骤
struct StoryStep {
    std::string text;
    std::string objective;
    std::string type;        // talk / defeat / get_item / enter_room / use_item_on
    std::string target;      // 目标NPC/物品/房间ID
    std::string target_npc;  // use_item_on 时的目标NPC
    std::string dialogue;
    int reward_exp = 0;
    std::string reward_item;
};

// 剧情章节
struct StoryChapter {
    std::string id;
    std::string title;
    std::vector<StoryStep> steps;
};

// 玩家初始属性
struct BaseStats {
    int hp = 80;
    int mp = 20;
    int attack = 8;
    int defense = 5;
    int money = 50;
};

// ============================================================
// GameData: 游戏数据容器
// 负责加载所有数据文件并提供查询接口
// ============================================================
class GameData {
public:
    std::map<std::string, RoomData> rooms;
    std::map<std::string, NPCData> npcs;
    std::map<std::string, ItemData> items;
    std::map<std::string, SkillData> skills;
    std::vector<LevelData> levels;
    std::vector<StoryChapter> chapters;
    BaseStats base_stats;
    std::string start_room;

    // 加载全部数据
    bool load_all(const std::string& data_dir);

    // 查询接口
    RoomData* get_room(const std::string& id);
    NPCData* get_npc(const std::string& id);
    ItemData* get_item(const std::string& id);
    SkillData* get_skill(const std::string& id);
    LevelData* get_level(int level);

private:
    // INI解析：返回 {section -> {key -> [values]}}
    // 同一个key出现多次时，values按顺序保存
    std::map<std::string, std::map<std::string, std::vector<std::string>>>
    parse_ini(const std::string& filepath);

    void load_maps(const std::string& dir);
    void load_npcs(const std::string& dir);
    void load_items(const std::string& dir);
    void load_skills(const std::string& dir);
    void load_levels(const std::string& dir);
    void load_story(const std::string& dir);

    // 字符串工具
    std::string trim(const std::string& s);
    std::vector<std::string> split(const std::string& s, char delim);
};

#endif // GAME_DATA_H
