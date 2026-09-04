#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>
#include "game_data.h"
#include "player.h"
#include "room.h"
#include "combat.h"
#include "story.h"
#include "ui.h"

// 游戏主控制器：处理数字菜单、游戏循环、各子系统调度
class Game {
public:
    Game();
    void start();

private:
    UI ui;
    GameData data;
    Player* player;
    std::map<std::string, Room> rooms;
    StoryManager* story;
    bool running;

    void game_loop();
    void show_current_room();
    void check_story_rewards();

    // 菜单动作
    void do_move();
    void do_talk();
    void do_debate();
    void do_pickup();
    void do_inventory();
    void do_use_item();
    void do_equip_item();
    void do_status();
    void do_skills();
    void do_quest();
    void do_shop();
    void do_rest();
};

#endif // GAME_H
