#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <vector>
#include <utility>
#include "game_data.h"
#include "npc.h"
#include "item.h"

// 房间类：游戏中的场景
class Room {
public:
    std::string id;
    std::string name;
    std::string desc;
    std::vector<std::pair<std::string, std::string>> exits; // 方向 -> 房间id
    std::vector<NPC> npcs;
    std::vector<Item> items;

    Room() {}
    Room(const RoomData& data, GameData* gdata);

    std::string get_exit(const std::string& direction) const;
    NPC* find_npc(const std::string& name);
    bool pick_up_item(const std::string& name, Item& out);  // 成功返回true，物品存入out
    void reset_npcs();
    std::string to_string() const;
};

#endif // ROOM_H
