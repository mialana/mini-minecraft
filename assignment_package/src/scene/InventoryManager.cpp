#include "InventoryManager.h"

InventoryManager::InventoryManager() : items() {}

InventoryManager::~InventoryManager() {}

bool InventoryManager::addItem(BlockType type) {

    /*if (items[type]) {
        items[type].count++;
    }
    else {
        items[type] = new InventoryItem();

    }*/


    items[type].count++;
    items[type].type = type;


    return true;
}

bool InventoryManager::removeItem(BlockType type) {
    //if (!items[type]) { return false; }

    if (items[type].count - 1 < 0) {
        return false;
    }
    items[type].count--;
    items[type].type = type;



    return true;
}
