#ifndef COMBAT_H
#define COMBAT_H

#include "player.h"
#include "npc.h"
#include "ui.h"

// 论道战斗系统
class Combat {
public:
    Combat(Player* p, NPC* n, UI* u);

    // 开始战斗，返回结果："win" / "lose" / "flee"
    std::string start();

private:
    Player* player;
    NPC* npc;
    UI* ui;
    int turn;
    int player_mp;  // 战斗中独立管理MP

    int calc_damage(int atk, const Skill& skill, int def);
    int player_turn();   // 返回：0继续 1逃跑 2物品
    void npc_turn();
};

#endif // COMBAT_H
