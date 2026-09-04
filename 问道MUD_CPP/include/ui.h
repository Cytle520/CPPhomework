#ifndef UI_H
#define UI_H

#include <string>
#include <vector>
#include "player.h"
#include "npc.h"
#include "room.h"
#include "item.h"
#include "skill.h"

// 终端颜色
namespace Color {
    const std::string RESET  = "\033[0m";
    const std::string BOLD   = "\033[1m";
    const std::string RED    = "\033[31m";
    const std::string GREEN  = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE   = "\033[34m";
    const std::string MAGENTA= "\033[35m";
    const std::string CYAN   = "\033[36m";
    const std::string WHITE  = "\033[37m";
}

class UI {
public:
    int width;

    UI();

    void clear();
    void line(char c = '=');
    void msg(const std::string& text = "");
    void title(const std::string& text);
    std::string prompt(const std::string& text = "> ");
    int prompt_int(const std::string& text, int min, int max);  // 数字选择菜单

    void show_title();
    void show_help();
    void show_room(const Room& room);
    void show_status(const Player& player);
    void show_inventory(const Player& player);
    void show_skills(const std::vector<Skill>& skills, int current_mp = -1);
    void show_combat_status(const Player& player, const NPC& npc, int player_mp);
    void show_shop(const NPC& merchant, GameData* data);
    void game_over(bool win = false);

    // 主菜单：返回选择的数字
    int main_menu();
    // 移动菜单：显示出口，返回选择的方向索引（-1取消）
    int move_menu(const Room& room);
    // NPC菜单：显示房间内NPC，返回索引（-1取消）
    int npc_menu(const Room& room, const std::string& action);
    // 物品菜单：显示背包物品，返回索引（-1取消）
    int item_menu(const std::vector<Item>& items, const std::string& title);

private:
    std::string bar(int current, int maximum, int length = 20);
};

#endif // UI_H
