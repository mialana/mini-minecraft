#ifndef RECIPEWINDOW_H
#define RECIPEWINDOW_H

#include <QWidget>
#include <QString>

namespace Ui {
class RecipeWindow;
}

static std::vector<QString> recipeNames {
    "Grass + Dirt = Sand",
    "Ice + Water = Snow 1",
    "Snow 1 + Water = Ice",
    "Stone + Cobblestone = Moss Stone",
    "Dirt + Water = Tilled Dirt",
    "Tilled Dirt + Straw = Wheat 1",
    "Wheat 8 + Water = Rice 1",
    "Rice 6 + Water = Rice 6",
    "Teak Planks + Teak Planks = Cedar Planks",
    "Cedar Planks + Cedar Planks = Teak Planks",
    "Cherry Planks + Cherry Planks = Maple Planks",
    "Maple Planks + Maple Planks = Cherry Planks",
    "Pine Planks + Pine Planks = Wisteria Planks",
    "Wisteria Planks + Wisteria Planks = Pine Planks",
    "Wisteria Blossoms 1 + Wisteria Blossoms 2 = Wisteria Blossoms 3",
    "Wisteria Blossoms 3 + Wisteria Blossoms 2 = Wisteria Blossoms 1",
    "Bamboo 1 + Bamboo 1 = Bamboo 2",
    "Bamboo 2 + Bamboo 1 = Bamboo 3",
    "Bamboo 3 + Bamboo 2 = Bamboo 1",
    "Lily Pad + Water = Lotus 1",
    "Lotus 1 + Water = Lotus 2",
    "Lotus 2 + Water = Lily Pad"
};

class RecipeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RecipeWindow(QWidget *parent = nullptr);
    ~RecipeWindow();

    void populateTable();

private:
    Ui::RecipeWindow *ui;
};

#endif // RECIPEWINDOW_H
