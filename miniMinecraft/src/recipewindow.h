#ifndef RECIPEWINDOW_H
#define RECIPEWINDOW_H

#include <QWidget>
#include <QString>

namespace Ui
{
class RecipeWindow;
}

static std::vector<QString>
    recipeNames{"Grass + Dirt = Sand",
                "Ice + Snow = Water",
                "Snow + Ice = Water",
                "Stone + Cobblestone = Moss Stone",
                "Dirt + Straw = Tilled Dirt",
                "Tilled Dirt + Wheat = Straw",
                "Water + Rice = Rice Paddy",
                "Rice Paddy + Rice Paddy = Rice Field",
                "Teak Planks + Cedar Planks = Cherry Planks",
                "Cherry Planks + Maple Planks = Pine Planks",
                "Cherry Planks + Wisteria Planks = Maple Planks",
                "Wisteria Blossoms 1 + Wisteria Blossoms 2 = Wisteria Blossoms 3",
                "Bamboo 1 + Bamboo 2 = Bamboo 3",
                "Bamboo 2 + Bamboo 3 = Bamboo 1",
                "Bamboo 3 + Bamboo 1 = Bamboo 2",
                "Lily Pad + Lotus 1 = Water",
                "Lotus 1 + Lotus 2 = Water",
                "Lotus 2 + Lily Pad = Water"};

class RecipeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RecipeWindow(QWidget* parent = nullptr);
    ~RecipeWindow();

    void populateTable();

private:
    Ui::RecipeWindow* ui;
};

#endif  // RECIPEWINDOW_H
