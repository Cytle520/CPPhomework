#include "npc.h"
#include <cstdlib>

NPC::NPC(const NPCData& data, GameData* gdata) {
    id = data.id;
    name = data.name;
    title = data.title;
    desc = data.desc;
    type = data.type;
    level = data.level;
    max_hp = data.hp;
    hp = max_hp;
    attack = data.attack;
    defense = data.defense;
    exp_reward = data.exp;
    dialogues = data.dialogues;
    defeat_dialogue = data.defeat_dialogue;
    drops = data.drops;
    shop_items = data.shop_items;
    defeated = false;
    dialogue_index = 0;

    for (auto& sid : data.skills) {
        SkillData* sd = gdata->get_skill(sid);
        if (sd) skills.push_back(Skill(*sd));
    }
}

std::string NPC::next_dialogue() {
    if (dialogues.empty()) return name + "沉默不语。";
    std::string text = dialogues[dialogue_index % dialogues.size()];
    dialogue_index++;
    return text;
}

Skill* NPC::choose_skill() {
    if (skills.empty()) return nullptr;
    int idx = rand() % skills.size();
    return &skills[idx];
}

int NPC::take_damage(int damage) {
    int actual = std::max(1, damage - defense / 2);
    hp = std::max(0, hp - actual);
    return actual;
}

void NPC::reset() {
    hp = max_hp;
}
