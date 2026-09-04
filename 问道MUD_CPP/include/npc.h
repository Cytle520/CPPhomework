#ifndef NPC_H
#define NPC_H

#include <string>
#include <vector>
#include "game_data.h"
#include "skill.h"

// NPC类：非玩家角色
class NPC {
public:
    std::string id;
    std::string name;
    std::string title;
    std::string desc;
    std::string type;    // mentor / enemy / merchant / npc
    int level;
    int max_hp, hp;
    int attack, defense;
    int exp_reward;
    std::vector<std::string> dialogues;
    std::string defeat_dialogue;
    std::vector<Skill> skills;
    std::vector<std::string> drops;
    std::vector<std::string> shop_items;
    bool defeated;

    NPC() : level(1), max_hp(50), hp(50), attack(5), defense(3), exp_reward(0), defeated(false) {}
    NPC(const NPCData& data, GameData* gdata);

    std::string next_dialogue();
    bool is_enemy() const { return type == "enemy"; }
    bool is_merchant() const { return type == "merchant"; }
    bool is_mentor() const { return type == "mentor"; }
    Skill* choose_skill();  // 战斗AI：随机选择技能
    int take_damage(int damage);
    void reset();

private:
    int dialogue_index;
};

#endif // NPC_H
