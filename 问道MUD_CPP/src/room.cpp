#include "room.h"
#include <sstream>

Room::Room(const RoomData& data, GameData* gdata) {
    id = data.id;
    name = data.name;
    desc = data.desc;
    exits = data.exits;

    for (auto& nid : data.npcs) {
        NPCData* nd = gdata->get_npc(nid);
        if (nd) npcs.push_back(NPC(*nd, gdata));
    }
    for (auto& iid : data.items) {
        ItemData* id = gdata->get_item(iid);
        if (id) items.push_back(Item(*id));
    }
}

std::string Room::get_exit(const std::string& direction) const {
    for (auto& [dir, target] : exits) {
        if (dir == direction) return target;
    }
    return "";
}

NPC* Room::find_npc(const std::string& name) {
    for (auto& npc : npcs) {
        if (npc.name.find(name) != std::string::npos || name.find(npc.name) != std::string::npos) {
            return &npc;
        }
    }
    return nullptr;
}

bool Room::pick_up_item(const std::string& name, Item& out) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->name.find(name) != std::string::npos || name.find(it->name) != std::string::npos) {
            out = *it;
            items.erase(it);
            return true;
        }
    }
    return false;
}

void Room::reset_npcs() {
    for (auto& npc : npcs) npc.reset();
}

std::string Room::to_string() const {
    std::stringstream ss;
    ss << "\n====================================================\n";
    ss << "  " << name << "\n";
    ss << "====================================================\n";
    ss << "  " << desc << "\n\n";
    if (!exits.empty()) {
        ss << "  【出口】";
        for (size_t i = 0; i < exits.size(); i++) {
            if (i > 0) ss << "、";
            ss << exits[i].first << "(" << exits[i].second << ")";
        }
        ss << "\n";
    }
    if (!npcs.empty()) {
        ss << "  【人物】";
        for (size_t i = 0; i < npcs.size(); i++) {
            if (i > 0) ss << "、";
            ss << npcs[i].name;
            if (!npcs[i].title.empty()) ss << "（" << npcs[i].title << "）";
        }
        ss << "\n";
    }
    if (!items.empty()) {
        ss << "  【物品】";
        for (size_t i = 0; i < items.size(); i++) {
            if (i > 0) ss << "、";
            ss << items[i].name;
        }
        ss << "\n";
    }
    ss << "====================================================\n";
    return ss.str();
}
