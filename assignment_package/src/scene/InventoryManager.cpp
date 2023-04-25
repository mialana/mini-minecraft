#include "InventoryManager.h"
#include <iostream>

InventoryManager::InventoryManager() : items() {
    items[CLOTH_1].count = 2;
    items[CLOTH_1].type = CLOTH_1;
}

InventoryManager::~InventoryManager() {}

bool InventoryManager::addItem(BlockType type) {
    items[type].count++;
    items[type].type = type;
    return true;
}

bool InventoryManager::removeItem(BlockType type) {

    if (items[type].count - 1 < 0) {
        return false;
    }
    items[type].count--;
    items[type].type = type;

    return true;
}
