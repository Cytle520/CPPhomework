#include "combat.h"
#include <cstdlib>
#include <algorithm>

Combat::Combat(Player* p, NPC* n, UI* u)
    : player(p), npc(n), ui(u), turn(0), player_mp(p->mp) {}

// 伤害公式：攻击力 * 技能倍率 - 防御力/2，再加 ±20% 随机浮动
int Combat::calc_damage(int atk, const Skill& skill, int def) {
    double base = atk * skill.damage;
    int reduction = def / 2;
    int damage = std::max(1, (int)(base - reduction));
    double variance = 0.8 + (rand() % 41) / 100.0;  // 0.8 ~ 1.2
    return std::max(1, (int)(damage * variance));
}

int Combat::player_turn() {
    turn++;
    ui->show_combat_status(*player, *npc, player_mp);

    ui->msg("\n  【你的回合】");
    ui->msg("  1. 直言（普通攻击）");
    ui->msg("  2. 使用招式");
    ui->msg("  3. 使用物品");
    ui->msg("  4. 逃跑");
    int choice = ui->prompt_int("请选择：", 1, 4);

    if (choice == 4) return 1;  // 逃跑

    if (choice == 3) {
        // 使用物品
        std::vector<Item> consumables;
        for (auto& item : player->inventory) {
            if (item.is_consumable()) consumables.push_back(item);
        }
        int idx = ui->item_menu(consumables, "选择物品");
        if (idx > 0) {
            std::string result = player->use_item(consumables[idx - 1].id);
            if (!result.empty()) ui->msg("  " + result);
        }
        return 2;  // 使用物品
    }

    if (choice == 2) {
        // 使用技能
        ui->show_skills(player->skills, player_mp);
        int idx = ui->prompt_int("选择招式（0返回）：", 0, (int)player->skills.size());
        if (idx <= 0) return player_turn();  // 返回重选
        Skill& skill = player->skills[idx - 1];
        if (skill.cost > player_mp) {
            ui->msg("  心神不足，无法使用此招式！");
            return player_turn();
        }
        player_mp -= skill.cost;

        if (skill.is_attack()) {
            int dmg = calc_damage(player->total_attack(), skill, npc->defense);
            npc->hp = std::max(0, npc->hp - dmg);
            ui->msg("  你使出【" + skill.name + "】，" + npc->name + "受到了" + std::to_string(dmg) + "点伤害！");
            if (skill.bonus_def > 0) {
                player->buffs.push_back({"defense", skill.bonus_def, 2});
            }
        } else if (skill.is_heal()) {
            int healed = std::min(skill.heal, player->max_hp - player->hp);
            player->hp += healed;
            ui->msg("  你使用【" + skill.name + "】，恢复了" + std::to_string(healed) + "点气血。");
        } else if (skill.is_buff()) {
            player->buffs.push_back({skill.buff_stat, skill.buff_value, skill.duration});
            ui->msg("  你使用【" + skill.name + "】，" + skill.buff_stat + "提升了" + std::to_string(skill.buff_value) + "点！");
        }
        return 0;
    }

    // choice == 1: 普通攻击（直言）
    int dmg = std::max(1, player->total_attack() - npc->defense / 2);
    npc->hp = std::max(0, npc->hp - dmg);
    ui->msg("  你直言驳斥，" + npc->name + "受到了" + std::to_string(dmg) + "点伤害。");
    return 0;
}

void Combat::npc_turn() {
    if (!npc->is_alive()) return;

    Skill* skill = npc->choose_skill();
    if (!skill) {
        int dmg = std::max(1, npc->attack - player->total_defense() / 2);
        player->hp = std::max(0, player->hp - dmg);
        ui->msg("  " + npc->name + "直言驳斥，你受到了" + std::to_string(dmg) + "点伤害。");
        return;
    }

    if (skill->is_attack()) {
        int dmg = calc_damage(npc->attack, *skill, player->total_defense());
        player->hp = std::max(0, player->hp - dmg);
        ui->msg("  " + npc->name + "使出【" + skill->name + "】，你受到了" + std::to_string(dmg) + "点伤害！");
    } else if (skill->is_heal()) {
        int healed = std::min(skill->heal, npc->max_hp - npc->hp);
        npc->hp += healed;
        ui->msg("  " + npc->name + "使用【" + skill->name + "】，恢复了" + std::to_string(healed) + "点气血。");
    } else if (skill->is_buff()) {
        npc->attack += skill->buff_value / 2;
        ui->msg("  " + npc->name + "使用【" + skill->name + "】，气势大振！");
    }
}

std::string Combat::start() {
    ui->msg("\n" + std::string(40, '='));
    ui->msg("  论道开始！你 vs " + npc->name);
    ui->msg(std::string(40, '=') + "\n");

    while (player->is_alive() && npc->is_alive()) {
        int action = player_turn();

        if (action == 1) {
            ui->msg("  你选择了回避，这场论道暂告一段落。");
            return "flee";
        }

        if (!npc->is_alive()) break;

        npc_turn();
        player->tick_buffs();
        player_mp = std::min(player->max_mp, player_mp + 2);  // 每回合恢复2点MP
    }

    player->mp = player_mp;

    if (!player->is_alive()) {
        ui->msg("\n  你败下阵来……" + npc->name + "的言辞让你哑口无言。");
        return "lose";
    }

    if (!npc->is_alive()) {
        ui->msg("\n  你赢得了这场论道！" + npc->name + "理屈词穷。");
        if (!npc->defeat_dialogue.empty()) {
            ui->msg("\n  " + npc->defeat_dialogue);
        }
        npc->defeated = true;
        return "win";
    }

    return "flee";
}
