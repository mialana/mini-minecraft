#include <QKeyEvent>
#include "inventorywindow.h"
#include "ui_inventorywindow.h"
#include <QListWidget>
#include <QMessageBox>
#include <iostream>

InventoryWindow::InventoryWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InventoryWindow)
{
    ui->setupUi(this);

    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_blockSelected(QListWidgetItem*)));

    connect(ui->crafting_item1, SIGNAL(clicked()), this, SLOT(slot_crafting_item1()));
    connect(ui->crafting_item2, SIGNAL(clicked()), this, SLOT(slot_crafting_item2()));
    connect(ui->crafting_craft, SIGNAL(clicked()), this, SLOT(slot_crafting_craft()));
}

InventoryWindow::~InventoryWindow()
{
    delete ui;
}

void InventoryWindow::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_I || e->key() == Qt::Key_T) {
        ui_main->mygl->isInventoryOpen = false;
        currItem = nullptr;
        this->close();
    }
}

void InventoryWindow::populateTable(std::map<BlockType, InventoryItem> list) {

    int cnt = 0;
    ui->listWidget->clear();
    ui->crafting_item1->setText("Click to add item 1");
    ui->crafting_item2->setText("Click to add item 2");
    crafting_item1 = EMPTY;
    crafting_item2 = EMPTY;
    currItem = nullptr;
    for (std::map<BlockType, InventoryItem>::iterator itr = list.begin(); itr != list.end(); itr++) {
        InventoryItem it = itr->second;
        if (it.count > 0) {
            InventoryItem* i = new InventoryItem();
            i->setText(typeStringPairs[it.type] + ": " + QString::number(it.count));
            i->count = it.count;
            i->type = it.type;
            ui->listWidget->addItem(i);
        }
    }
    ui->listWidget->update();
}

void InventoryWindow::slot_blockSelected(QListWidgetItem* item) {
    currItem = static_cast<InventoryItem*>(item);
    if (currItem) {
        ui_main->mygl->currBlock = currItem->type;
    }
}

void InventoryWindow::slot_crafting_item1() {
    if (currItem) {
        crafting_item1 = currItem->type;
        ui->crafting_item1->setText(typeStringPairs[crafting_item1]);
    }
    else {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","no item selected\nPlease select something from inventory");
        messageBox.setFixedSize(500,200);
    }
}
void InventoryWindow::slot_crafting_item2() {
    if (currItem) {
        crafting_item2 = currItem->type;
        ui->crafting_item2->setText(typeStringPairs[crafting_item2]);
    }
    else {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","no item selected\nPlease select something from inventory");
        messageBox.setFixedSize(500,200);
    }
}
void InventoryWindow::slot_crafting_craft() {
    if (crafting_item1 != EMPTY && crafting_item2 != EMPTY) {
        if(canCraft(crafting_item1, crafting_item2)
                && recipeExists(crafting_item1, crafting_item2)) {
            QMessageBox messageBox;
            messageBox.information(0,"Success",
            typeStringPairs[craftingRecipes[std::make_pair(crafting_item1, crafting_item2)]]);
            messageBox.setFixedSize(500,200);
            ui_main->mygl->m_player.inventory.removeItem(crafting_item1);
            ui_main->mygl->m_player.inventory.removeItem(crafting_item2);
            ui_main->mygl->m_player.inventory.addItem(craftingRecipes[std::make_pair(crafting_item1, crafting_item2)]);
            populateTable(ui_main->mygl->m_player.inventory.items);
        }
    }
}

bool InventoryWindow::canCraft(BlockType i1, BlockType i2) {
    InventoryManager m = ui_main->mygl->m_player.inventory;
    if (i1 == i2) {
        return m.items[i1].count >= 2;
    }

    return ((m.items[i1].count >= 1) && (m.items[i2].count >= 1));
}

bool InventoryWindow::recipeExists(BlockType i1, BlockType i2) {
    return craftingRecipes.count(std::make_pair(i1, i2)) == 0 ? false : true;
}
