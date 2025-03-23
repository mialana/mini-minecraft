#ifndef INVENTORYMANAGER_H
#define INVENTORYMANAGER_H

#include <map>
#include "chunk.h"
#include <QListWidgetItem>
#include <QTableWidgetItem>

class InventoryItem : public QListWidgetItem
{
public:
    InventoryItem()
    {
        this->setText("0");
    }

    ~InventoryItem() {}

    int count = 0;

    BlockType type;
};

class InventoryManager
{
public:
    InventoryManager();
    ~InventoryManager();

    std::map<BlockType, InventoryItem> items;

    bool removeItem(BlockType);
    bool addItem(BlockType);
};

#endif  // INVENTORYMANAGER_H
