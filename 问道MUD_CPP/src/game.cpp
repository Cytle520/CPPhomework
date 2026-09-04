#include "game.h"
#include <iostream>
#include <algorithm>

Game::Game() : player(nullptr), story(nullptr), running(false) {}

void Game::start() {
    ui.show_title();

    ui.msg("正在加载游戏数据...\n");
    std::string data_dir = "data";
    if (!data.load_all(data_dir)) {
        ui.msg("数据加载失败！请确认 data 目录存在。");
        return;
    }
    ui.msg("");

    // 创建房间
    for (auto& [id, rdata] : data.rooms) {
        rooms[id] = Room(rdata, &data);
    }

    // 创建玩家
    std::string name = ui.prompt("请输入你的名字：");
    if (name.empty()) name = "无名学子";
    player = new Player(name, &data);

    // 初始化剧情
    story = new StoryManager(&data, &ui);

    ui.msg("\n欢迎你，" + name + "。你的问道之旅，从此刻开始……\n");
    ui.prompt("按回车继续...");

    running = true;
    show_current_room();
    game_loop();

    delete player;
    delete story;
}

void Game::game_loop() {
    while (running) {
        check_story_rewards();

        int choice = ui.main_menu();
        if (choice == -1) {  // q 退出
            ui.msg("\n山高水长，后会有期。\n");
            running = false;
            break;
        }
        if (choice == -2) {
            ui.msg("无效输入，请输入数字。");
            continue;
        }

        switch (choice) {
            case 1: do_move(); break;
            case 2: do_talk(); break;
            case 3: do_debate(); break;
            case 4: do_pickup(); break;
            case 5: do_inventory(); break;
            case 6: do_status(); break;
            case 7: do_skills(); break;
            case 8: do_quest(); break;
            case 9: do_shop(); break;
            case 0: do_rest(); break;
            default: ui.msg("无效选择。"); break;
        }

        if (!player->is_alive()) {
            ui.game_over(false);
            ui.prompt("按回车退出...");
            running = false;
        }
    }
}

void Game::show_current_room() {
    auto it = rooms.find(player->current_room);
    if (it != rooms.end()) {
        ui.show_room(it->second);
    }
}

void Game::check_story_rewards() {
    if (story->has_pending_reward()) {
        int exp = story->get_pending_exp();
        std::string item = story->get_pending_item();
        story->clear_pending_reward();

        if (exp > 0) {
            bool leveled = player->gain_exp(exp);
            ui.msg("\n你获得了 " + std::to_string(exp) + " 点经验！");
            if (leveled) {
                ui.msg("\n★ 境界突破！你已达到「" + player->level_name + "」！");
                LevelData* lv = data.get_level(player->level);
                if (lv) ui.msg(lv->desc);
            }
        }
        if (!item.empty()) {
            if (player->add_item(item)) {
                ItemData* id = data.get_item(item);
                ui.msg("你获得了：" + (id ? id->name : item));
            }
        }
    }
}

// ============================================================
// 菜单动作实现
// ============================================================

void Game::do_move() {
    auto it = rooms.find(player->current_room);
    if (it == rooms.end()) return;
    Room& room = it->second;

    int idx = ui.move_menu(room);
    if (idx <= 0) return;

    std::string target = room.exits[idx - 1].second;
    if (target.empty()) {
        ui.msg("这个方向没有路。");
        return;
    }

    room.reset_npcs();
    player->current_room = target;
    show_current_room();
    story->check_event("enter_room", target);
}

void Game::do_talk() {
    auto it = rooms.find(player->current_room);
    if (it == rooms.end()) return;
    Room& room = it->second;

    int idx = ui.npc_menu(room, "对话");
    if (idx <= 0) return;

    NPC& npc = room.npcs[idx - 1];
    ui.msg("\n你走向" + npc.name + "。");
    ui.msg(npc.desc + "\n");

    // 检查是否是剧情要求的对话
    StoryStep* step = story->current_step();
    if (step && step->type == "talk" && step->target == npc.id && !step->dialogue.empty()) {
        ui.msg(step->dialogue);
    } else {
        ui.msg(npc.next_dialogue());
    }

    if (npc.is_merchant()) {
        ui.msg("\n（在主菜单选择「商店」可与" + npc.name + "交易）");
    }

    story->check_event("talk", npc.id);
}

void Game::do_debate() {
    auto it = rooms.find(player->current_room);
    if (it == rooms.end()) return;
    Room& room = it->second;

    // 只显示可战斗的NPC
    std::vector<NPC*> enemies;
    for (auto& npc : room.npcs) {
        if (npc.is_enemy() && !npc.defeated) enemies.push_back(&npc);
    }
    if (enemies.empty()) {
        ui.msg("这里没有可以论道的人。");
        return;
    }

    ui.msg("\n  【选择论道对象】");
    for (size_t i = 0; i < enemies.size(); i++) {
        ui.msg("  " + std::to_string(i + 1) + ". " + enemies[i]->name
               + "（Lv." + std::to_string(enemies[i]->level) + "）");
    }
    ui.msg("  0. 返回");
    int idx = ui.prompt_int("请选择：", 0, (int)enemies.size());
    if (idx <= 0) return;

    NPC* npc = enemies[idx - 1];
    Combat combat(player, npc, &ui);
    std::string result = combat.start();

    if (result == "win") {
        int exp = npc->exp_reward;
        ui.msg("\n你获得了 " + std::to_string(exp) + " 点经验！");
        bool leveled = player->gain_exp(exp);
        if (leveled) {
            ui.msg("\n★ 境界突破！你已达到「" + player->level_name + "」！");
            LevelData* lv = data.get_level(player->level);
            if (lv) ui.msg(lv->desc);
        }
        // 掉落
        for (auto& drop : npc->drops) {
            if (player->add_item(drop)) {
                ItemData* id = data.get_item(drop);
                ui.msg("你获得了：" + (id ? id->name : drop));
            }
        }
        story->check_event("defeat", npc->id);
    } else if (result == "lose") {
        player->hp = player->max_hp / 2;
        player->mp = player->max_mp / 2;
        player->current_room = data.start_room;
        ui.msg("\n你被人送回了书院，休养了几日才恢复过来。");
        show_current_room();
    }
}

void Game::do_pickup() {
    auto it = rooms.find(player->current_room);
    if (it == rooms.end()) return;
    Room& room = it->second;

    if (room.items.empty()) {
        ui.msg("这里没有可以拾取的物品。");
        return;
    }

    ui.msg("\n  【选择拾取物品】");
    for (size_t i = 0; i < room.items.size(); i++) {
        ui.msg("  " + std::to_string(i + 1) + ". " + room.items[i].name);
    }
    ui.msg("  0. 返回");
    int idx = ui.prompt_int("请选择：", 0, (int)room.items.size());
    if (idx <= 0) return;

    Item item = room.items[idx - 1];
    room.items.erase(room.items.begin() + idx - 1);
    player->inventory.push_back(item);
    ui.msg("你拾取了：" + item.name);
    story->check_event("get_item", item.id);
}

void Game::do_inventory() {
    ui.show_inventory(*player);
    ui.msg("  1. 使用物品");
    ui.msg("  2. 装备物品");
    ui.msg("  0. 返回");
    int choice = ui.prompt_int("请选择：", 0, 2);
    if (choice == 1) do_use_item();
    else if (choice == 2) do_equip_item();
}

void Game::do_use_item() {
    std::vector<Item> consumables;
    for (auto& item : player->inventory) {
        if (item.is_consumable()) consumables.push_back(item);
    }
    int idx = ui.item_menu(consumables, "使用物品");
    if (idx <= 0) return;
    std::string result = player->use_item(consumables[idx - 1].id);
    if (!result.empty()) ui.msg(result);
    else ui.msg("无法使用该物品。");
}

void Game::do_equip_item() {
    std::vector<Item> equipments;
    for (auto& item : player->inventory) {
        if (item.is_equipment()) equipments.push_back(item);
    }
    int idx = ui.item_menu(equipments, "装备物品");
    if (idx <= 0) return;
    std::string result = player->equip_item(equipments[idx - 1].id);
    if (!result.empty()) ui.msg(result);
    else ui.msg("无法装备该物品。");
}

void Game::do_status() {
    ui.show_status(*player);
}

void Game::do_skills() {
    ui.show_skills(player->skills);
}

void Game::do_quest() {
    ui.msg(story->quest_log());
}

void Game::do_shop() {
    auto it = rooms.find(player->current_room);
    if (it == rooms.end()) return;
    Room& room = it->second;

    NPC* merchant = nullptr;
    for (auto& npc : room.npcs) {
        if (npc.is_merchant()) { merchant = &npc; break; }
    }
    if (!merchant) {
        ui.msg("这里没有商人。");
        return;
    }

    ui.show_shop(*merchant, &data);
    ui.msg("  你的银两：" + std::to_string(player->money) + "文");
    int idx = ui.prompt_int("输入序号购买（0离开）：", 0, (int)merchant->shop_items.size());
    if (idx <= 0) return;

    std::string item_id = merchant->shop_items[idx - 1];
    ItemData* idata = data.get_item(item_id);
    if (!idata) return;

    if (player->money >= idata->price) {
        player->money -= idata->price;
        player->add_item(item_id);
        ui.msg("你花了" + std::to_string(idata->price) + "文，买下了" + idata->name + "。");
    } else {
        ui.msg("银两不足！");
    }
}

void Game::do_rest() {
    player->rest();
    ui.msg("你找了个安静的地方打坐休息，气血和心神都有所恢复。");
}
