#ifndef ITEM_H
#define ITEM_H

#include <string>
#include "game_data.h"

// 物品类：运行时的物品实例
class Item {
public:
    std::string id;
    std::string name;
    std::string type;    // consumable / equipment / quest
    std::string desc;
    std::string slot;    // weapon / armor
    bool usable;
    int price;
    std::string effect_type;
    std::string effect_stat;
    int effect_value;
    int effect_duration;

    Item() : usable(false), price(0), effect_value(0), effect_duration(0) {}
    Item(const ItemData& data) {
        id = data.id;
        name = data.name;
        type = data.type;
        desc = data.desc;
        slot = data.slot;
        usable = data.usable;
        price = data.price;
        effect_type = data.effect_type;
        effect_stat = data.effect_stat;
        effect_value = data.effect_value;
        effect_duration = data.effect_duration;
    }

    bool is_equipment() const { return type == "equipment"; }
    bool is_consumable() const { return type == "consumable"; }
    bool is_quest() const { return type == "quest"; }
};

#endif // ITEM_H
