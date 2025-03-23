#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cameracontrolshelp.h"
#include "playerinfo.h"
#include "inventorywindow.h"
#include "recipewindow.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

    void slot_actionToggleInventory(bool);

    void slot_showRecipeWindow();

    /**
     * @brief slot_changeMobPathsText
     * @param newState: true if all mob paths are now showing, false otherwise.
     */
    void slot_changeMobPathsText(bool newState);

private:
    Ui::MainWindow* ui;
    CameraControlsHelp cHelp;
    PlayerInfo playerInfoWindow;
    InventoryWindow inventoryWindow;
    RecipeWindow recipeWindow;

    virtual void closeEvent(QCloseEvent* e);
};

#endif  // MAINWINDOW_H
