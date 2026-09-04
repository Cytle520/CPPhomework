#include "ui.h"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

UI::UI() : width(52) {
#ifdef _WIN32
    // 启用Windows终端ANSI颜色支持
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

void UI::clear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void UI::line(char c) {
    std::cout << Color::CYAN << std::string(width, c) << Color::RESET << std::endl;
}

void UI::msg(const std::string& text) {
    std::cout << text << std::endl;
}

void UI::title(const std::string& text) {
    std::cout << Color::BOLD << Color::YELLOW << text << Color::RESET << std::endl;
}

std::string UI::prompt(const std::string& text) {
    std::cout << Color::GREEN << text << Color::RESET;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

// 数字选择菜单：输入min~max之间的数字，自动校验
int UI::prompt_int(const std::string& text, int min, int max) {
    while (true) {
        std::string input = prompt(text);
        if (input.empty()) return -1;  // 空输入表示取消
        try {
            int val = std::stoi(input);
            if (val >= min && val <= max) return val;
            msg("输入超出范围，请重新选择。");
        } catch (...) {
            msg("请输入数字。");
        }
    }
}

void UI::show_title() {
    clear();
    std::cout << Color::BOLD << Color::YELLOW << R"(
    ╔══════════════════════════════════════════╗
    ║                                          ║
    ║            问      道                    ║
    ║                                          ║
    ║       —— 诸子百家 · 论道修行 ——         ║
    ║                                          ║
    ╚══════════════════════════════════════════╝
)" << Color::RESET << std::endl;
    std::cout << Color::CYAN << "    一款以中国古典哲学为内核的 MUD 文字游戏" << Color::RESET << std::endl;
    line();
    msg();
}

void UI::show_help() {
    msg();
    title("【操作说明】");
    msg("  游戏全程使用数字菜单操作，无需记忆任何指令。");
    msg("  主菜单输入对应数字即可执行操作。");
    msg("  任何菜单直接按回车可返回上一级。");
    msg();
}

void UI::show_room(const Room& room) {
    std::cout << room.to_string();
}

void UI::show_status(const Player& player) {
    msg();
    line('-');
    std::cout << Color::BOLD << "  " << player.to_string() << Color::RESET << std::endl;
    int next_exp = player.exp_to_next();
    if (next_exp > 0) {
        msg("  经验：" + std::to_string(player.exp) + "  距下一境界还需：" + std::to_string(next_exp));
    } else {
        msg("  经验：" + std::to_string(player.exp) + "（已达最高境界）");
    }
    if (!player.equipped_weapon_id.empty()) {
        for (auto& item : player.inventory) {
            if (item.id == player.equipped_weapon_id) {
                msg("  武器：" + item.name);
                break;
            }
        }
    }
    if (!player.equipped_armor_id.empty()) {
        for (auto& item : player.inventory) {
            if (item.id == player.equipped_armor_id) {
                msg("  护甲：" + item.name);
                break;
            }
        }
    }
    line('-');
    msg();
}

void UI::show_inventory(const Player& player) {
    title("\n【背包】");
    if (player.inventory.empty()) {
        msg("  空空如也。");
    } else {
        for (size_t i = 0; i < player.inventory.size(); i++) {
            auto& item = player.inventory[i];
            std::string tag;
            if (item.is_equipment()) tag = Color::YELLOW + " [装备]" + Color::RESET;
            else if (item.is_consumable()) tag = Color::GREEN + " [消耗]" + Color::RESET;
            else if (item.is_quest()) tag = Color::MAGENTA + " [任务]" + Color::RESET;
            msg("  " + std::to_string(i + 1) + ". " + item.name + tag);
            msg("     " + item.desc);
        }
    }
    msg();
}

void UI::show_skills(const std::vector<Skill>& skills, int current_mp) {
    title("\n【可用招式】");
    for (size_t i = 0; i < skills.size(); i++) {
        auto& s = skills[i];
        std::string mp_tag;
        if (current_mp >= 0) {
            std::string color = (s.cost <= current_mp) ? Color::GREEN : Color::RED;
            mp_tag = " " + color + "（心神" + std::to_string(s.cost) + "）" + Color::RESET;
        }
        msg("  " + std::to_string(i + 1) + ". " + s.name + mp_tag);
        msg("     " + s.desc);
    }
    msg();
}

void UI::show_combat_status(const Player& player, const NPC& npc, int player_mp) {
    msg();
    line('-');
    msg("  " + Color::GREEN + "你" + Color::RESET + "  气血[" + bar(player.hp, player.max_hp) + "] "
        + std::to_string(player.hp) + "/" + std::to_string(player.max_hp)
        + "  心神:" + std::to_string(player_mp) + "/" + std::to_string(player.max_mp));
    msg("  " + Color::RED + npc.name + Color::RESET + "  气血[" + bar(npc.hp, npc.max_hp) + "] "
        + std::to_string(npc.hp) + "/" + std::to_string(npc.max_hp));
    line('-');
    msg();
}

std::string UI::bar(int current, int maximum, int length) {
    if (maximum <= 0) return std::string(length, '-');
    int filled = std::max(0, std::min(length, (int)(current * 1.0 / maximum * length)));
    std::string color;
    double pct = current * 1.0 / maximum;
    if (pct > 0.5) color = Color::GREEN;
    else if (pct > 0.25) color = Color::YELLOW;
    else color = Color::RED;
    return color + std::string(filled, '#') + Color::RESET + std::string(length - filled, '-');
}

void UI::show_shop(const NPC& merchant, GameData* data) {
    title("\n【" + merchant.name + "的店铺】");
    for (size_t i = 0; i < merchant.shop_items.size(); i++) {
        ItemData* id = data->get_item(merchant.shop_items[i]);
        if (id) {
            msg("  " + std::to_string(i + 1) + ". " + id->name + "  价格："
                + Color::YELLOW + std::to_string(id->price) + "文" + Color::RESET);
            msg("     " + id->desc);
        }
    }
    msg();
}

void UI::game_over(bool win) {
    msg();
    line();
    if (win) {
        msg(Color::GREEN + "  你已得道，天下闻名。" + Color::RESET);
    } else {
        msg(Color::RED + "  你心力交瘁，倒在了论道台上……" + Color::RESET);
        msg("  但问道之路，从未终结。");
    }
    line();
    msg();
}

// ============================================================
// 数字菜单
// ============================================================
int UI::main_menu() {
    msg();
    line('-');
    msg("  【主菜单】");
    msg("  1. 移动");
    msg("  2. 对话");
    msg("  3. 论道（战斗）");
    msg("  4. 拾取物品");
    msg("  5. 背包");
    msg("  6. 状态");
    msg("  7. 技能");
    msg("  8. 任务");
    msg("  9. 商店");
    msg("  0. 休息");
    msg("  q. 退出游戏");
    line('-');
    std::string input = prompt("请选择：");
    if (input == "q" || input == "Q") return -1;
    try {
        return std::stoi(input);
    } catch (...) {
        return -2;
    }
}

int UI::move_menu(const Room& room) {
    msg();
    msg("  【选择方向】");
    for (size_t i = 0; i < room.exits.size(); i++) {
        msg("  " + std::to_string(i + 1) + ". " + room.exits[i].first);
    }
    msg("  0. 返回");
    return prompt_int("请选择方向：", 0, (int)room.exits.size());
}

int UI::npc_menu(const Room& room, const std::string& action) {
    if (room.npcs.empty()) {
        msg("  这里没有人。");
        return -1;
    }
    msg();
    msg("  【选择对象 - " + action + "】");
    for (size_t i = 0; i < room.npcs.size(); i++) {
        std::string tag;
        if (room.npcs[i].is_enemy()) tag = Color::RED + " [可论道]" + Color::RESET;
        else if (room.npcs[i].is_merchant()) tag = Color::YELLOW + " [商人]" + Color::RESET;
        else if (room.npcs[i].is_mentor()) tag = Color::BLUE + " [导师]" + Color::RESET;
        msg("  " + std::to_string(i + 1) + ". " + room.npcs[i].name + tag);
    }
    msg("  0. 返回");
    return prompt_int("请选择：", 0, (int)room.npcs.size());
}

int UI::item_menu(const std::vector<Item>& items, const std::string& title_text) {
    if (items.empty()) {
        msg("  没有可用物品。");
        return -1;
    }
    msg();
    msg("  【" + title_text + "】");
    for (size_t i = 0; i < items.size(); i++) {
        msg("  " + std::to_string(i + 1) + ". " + items[i].name);
    }
    msg("  0. 返回");
    return prompt_int("请选择：", 0, (int)items.size());
}
