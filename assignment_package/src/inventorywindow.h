#ifndef INVENTORYWINDOW_H
#define INVENTORYWINDOW_H

#include <QWidget>
#include <ui_mainwindow.h>

namespace Ui {
class InventoryWindow;
}

class InventoryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit InventoryWindow(QWidget *parent = nullptr);
    ~InventoryWindow();

    void keyPressEvent(QKeyEvent *e);
    Ui::MainWindow *ui_main;

private:
    Ui::InventoryWindow *ui;
};

#endif // INVENTORYWINDOW_H
