#ifndef SKILL_H
#define SKILL_H

#include <string>
#include "game_data.h"

// 技能类：论道战斗中的招式
class Skill {
public:
    std::string id;
    std::string name;
    std::string desc;
    std::string type;    // attack / heal / buff
    double damage;
    int cost;
    int heal;
    std::string buff_stat;
    int buff_value;
    int duration;
    int bonus_def;
    int unlock_level;

    Skill() : damage(1.0), cost(0), heal(0), buff_value(0), duration(0), bonus_def(0), unlock_level(1) {}
    Skill(const SkillData& data) {
        id = data.id;
        name = data.name;
        desc = data.desc;
        type = data.type;
        damage = data.damage;
        cost = data.cost;
        heal = data.heal;
        buff_stat = data.buff_stat;
        buff_value = data.buff_value;
        duration = data.duration;
        bonus_def = data.bonus_def;
        unlock_level = data.unlock_level;
    }

    bool is_attack() const { return type == "attack"; }
    bool is_heal() const { return type == "heal"; }
    bool is_buff() const { return type == "buff"; }
};

#endif // SKILL_H
