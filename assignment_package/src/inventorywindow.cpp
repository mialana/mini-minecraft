#include <QKeyEvent>
#include "inventorywindow.h"
#include "ui_inventorywindow.h"

InventoryWindow::InventoryWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InventoryWindow)
{
    ui->setupUi(this);
}

InventoryWindow::~InventoryWindow()
{
    delete ui;
}

void InventoryWindow::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_I) {
        ui_main->mygl->isInventoryOpen = false;
        this->close();
    }
}
