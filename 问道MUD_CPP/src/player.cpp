#include "player.h"
#include <algorithm>
#include <sstream>

Player::Player(const std::string& n, GameData* d) : name(n), data(d) {
    // 初始属性来自 levels.txt 的 [base]
    max_hp = data->base_stats.hp;
    hp = max_hp;
    max_mp = data->base_stats.mp;
    mp = max_mp;
    attack = data->base_stats.attack;
    defense = data->base_stats.defense;
    money = data->base_stats.money;

    level = 1;
    exp = 0;
    equipped_weapon_id = "";
    equipped_armor_id = "";
    current_room = data->start_room;

    LevelData* lv = data->get_level(1);
    if (lv) level_name = lv->name;

    init_skills();
}

void Player::init_skills() {
    for (int lv = 1; lv <= level; lv++) {
        LevelData* cfg = data->get_level(lv);
        if (cfg) {
            for (auto& sid : cfg->unlock_skills) {
                learn_skill(sid);
            }
        }
    }
}

void Player::learn_skill(const std::string& skill_id) {
    for (auto& s : skills) {
        if (s.id == skill_id) return;
    }
    SkillData* sd = data->get_skill(skill_id);
    if (sd) {
        skills.push_back(Skill(*sd));
    }
}

// ============================================================
// 经验与升级
// ============================================================
bool Player::gain_exp(int amount) {
    exp += amount;
    bool leveled = false;
    while (true) {
        LevelData* next = data->get_level(level + 1);
        if (!next) break;
        if (exp >= next->exp_required) {
            level_up(*next);
            leveled = true;
        } else {
            break;
        }
    }
    return leveled;
}

void Player::level_up(const LevelData& cfg) {
    level++;
    level_name = cfg.name;
    max_hp += cfg.hp_grow;
    max_mp += cfg.mp_grow;
    attack += cfg.atk_grow;
    defense += cfg.def_grow;
    hp = max_hp;
    mp = max_mp;
    for (auto& sid : cfg.unlock_skills) {
        learn_skill(sid);
    }
}

int Player::exp_to_next() const {
    LevelData* next = data->get_level(level + 1);
    if (!next) return 0;
    return next->exp_required - exp;
}

// ============================================================
// 背包操作
// ============================================================
bool Player::add_item(const std::string& item_id) {
    ItemData* id = data->get_item(item_id);
    if (!id) return false;
    inventory.push_back(Item(*id));
    return true;
}

bool Player::remove_item(const std::string& item_id) {
    for (auto it = inventory.begin(); it != inventory.end(); ++it) {
        if (it->id == item_id) {
            // 如果装备着，先卸下
            if (equipped_weapon_id == item_id) equipped_weapon_id = "";
            if (equipped_armor_id == item_id) equipped_armor_id = "";
            inventory.erase(it);
            return true;
        }
    }
    return false;
}

bool Player::has_item(const std::string& item_id) const {
    for (auto& item : inventory) {
        if (item.id == item_id) return true;
    }
    return false;
}

std::string Player::use_item(const std::string& item_id) {
    for (auto it = inventory.begin(); it != inventory.end(); ++it) {
        if (it->id == item_id && it->is_consumable()) {
            std::string msg;
            if (it->effect_type == "heal") {
                int healed = std::min(it->effect_value, max_hp - hp);
                hp += healed;
                msg = "你使用了" + it->name + "，恢复了" + std::to_string(healed) + "点气血。";
            } else if (it->effect_type == "buff") {
                buffs.push_back({it->effect_stat, it->effect_value, it->effect_duration});
                msg = "你使用了" + it->name + "，" + it->effect_stat + "提升了" + std::to_string(it->effect_value) + "点。";
            } else if (it->effect_type == "exp") {
                gain_exp(it->effect_value);
                msg = "你研读了" + it->name + "，获得了" + std::to_string(it->effect_value) + "点经验。";
            }
            inventory.erase(it);
            return msg;
        }
    }
    return "";
}

std::string Player::equip_item(const std::string& item_id) {
    // 装备的物品保留在背包中，用ID标记已装备
    for (auto& item : inventory) {
        if (item.id == item_id && item.is_equipment()) {
            if (item.slot == "weapon") {
                equipped_weapon_id = item.id;
            } else if (item.slot == "armor") {
                equipped_armor_id = item.id;
            }
            return "你装备了" + item.name + "。";
        }
    }
    return "";
}

// ============================================================
// 属性计算
// ============================================================
int Player::total_attack() const {
    int atk = attack;
    if (!equipped_weapon_id.empty()) {
        for (auto& item : inventory) {
            if (item.id == equipped_weapon_id) {
                atk += item.effect_value;
                break;
            }
        }
    }
    for (auto& b : buffs) {
        if (b.stat == "attack") atk += b.value;
    }
    return atk;
}

int Player::total_defense() const {
    int dfs = defense;
    if (!equipped_armor_id.empty()) {
        for (auto& item : inventory) {
            if (item.id == equipped_armor_id) {
                dfs += item.effect_value;
                break;
            }
        }
    }
    for (auto& b : buffs) {
        if (b.stat == "defense") dfs += b.value;
    }
    return dfs;
}

int Player::take_damage(int damage) {
    int actual = std::max(1, damage - total_defense() / 2);
    hp = std::max(0, hp - actual);
    return actual;
}

bool Player::is_alive() const {
    return hp > 0;
}

void Player::rest() {
    hp = std::min(max_hp, hp + max_hp / 4);
    mp = std::min(max_mp, mp + max_mp / 3);
    buffs.clear();
}

void Player::tick_buffs() {
    for (auto& b : buffs) b.remaining--;
    buffs.erase(std::remove_if(buffs.begin(), buffs.end(),
        [](const Buff& b) { return b.remaining <= 0; }), buffs.end());
}

Item* Player::find_item(const std::string& item_id) {
    for (auto& item : inventory) {
        if (item.id == item_id) return &item;
    }
    return nullptr;
}

std::string Player::to_string() const {
    std::stringstream ss;
    ss << "【" << name << "】 境界：" << level_name << "(Lv." << level << ")  "
       << "气血：" << hp << "/" << max_hp << "  心神：" << mp << "/" << max_mp << "  "
       << "攻击：" << total_attack() << "  防御：" << total_defense() << "  "
       << "银两：" << money;
    return ss.str();
}
