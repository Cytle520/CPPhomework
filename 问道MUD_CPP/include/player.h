#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include "game_data.h"
#include "item.h"
#include "skill.h"

// 增益效果
struct Buff {
    std::string stat;   // attack / defense
    int value;
    int remaining;      // 剩余回合
};

// 玩家类：管理玩家属性、背包、装备、技能、升级
class Player {
public:
    std::string name;
    GameData* data;

    // 核心属性
    int max_hp, hp;
    int max_mp, mp;
    int attack, defense;
    int money;

    // 等级与经验
    int level;
    std::string level_name;
    int exp;

    // 背包与装备（用ID存储，避免vector扩容导致指针失效）
    std::vector<Item> inventory;
    std::string equipped_weapon_id;  // 空字符串表示未装备
    std::string equipped_armor_id;

    // 已学技能
    std::vector<Skill> skills;

    // 位置
    std::string current_room;

    // 战斗增益
    std::vector<Buff> buffs;

    Player(const std::string& name, GameData* data);

    // 经验与升级
    bool gain_exp(int amount);   // 返回是否升级
    int exp_to_next() const;

    // 背包操作
    bool add_item(const std::string& item_id);
    bool remove_item(const std::string& item_id);
    bool has_item(const std::string& item_id) const;
    std::string use_item(const std::string& item_id);  // 返回使用结果描述
    std::string equip_item(const std::string& item_id);

    // 属性计算（含装备和增益）
    int total_attack() const;
    int total_defense() const;

    // 战斗相关
    int take_damage(int damage);
    bool is_alive() const;
    void rest();
    void tick_buffs();

    // 学习技能
    void learn_skill(const std::string& skill_id);

    // 通过ID查找背包中的物品（用于装备查询）
    Item* find_item(const std::string& item_id);

    // 显示
    std::string to_string() const;

private:
    void init_skills();
    void level_up(const LevelData& cfg);
};

#endif // PLAYER_H
