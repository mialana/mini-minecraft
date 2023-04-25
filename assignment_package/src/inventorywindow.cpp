#include <QKeyEvent>
#include "inventorywindow.h"
#include "ui_inventorywindow.h"
#include <QListWidget>
#include <iostream>

InventoryWindow::InventoryWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InventoryWindow)
{
    ui->setupUi(this);

    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_blockSelected(QListWidgetItem*)));
}

InventoryWindow::~InventoryWindow()
{
    delete ui;
}

void InventoryWindow::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_I || e->key() == Qt::Key_T) {
        ui_main->mygl->isInventoryOpen = false;
        this->close();
    }
}

void InventoryWindow::populateTable(std::map<BlockType, InventoryItem> list) {

    int cnt = 0;
    ui->listWidget->clear();
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
    InventoryItem* i = static_cast<InventoryItem*>(item);
    if (i) {
        ui_main->mygl->currBlock = i->type;

        if (i->type == WHEAT_1) {
            for (auto& mob : ui_main->mygl->m_mobs) {
                mob->m_inputs.isHoldingWheat = true;
            }
        } else {
            for (auto& mob : ui_main->mygl->m_mobs) {
                mob->m_inputs.isHoldingWheat = false;
            }
        }

    }
}
