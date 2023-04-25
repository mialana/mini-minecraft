#ifndef INVENTORYWINDOW_H
#define INVENTORYWINDOW_H

#include <QWidget>
#include <unordered_map>
#include <map>
#include <QString>
#include <ui_mainwindow.h>
#include "scene/chunk.h"
#include "scene/InventoryManager.h"
#include <QListWidgetItem>
#include <vector>

namespace Ui {
class InventoryWindow;
}

struct TypePairHash
{
    std::size_t operator()(const std::pair<BlockType, BlockType> &pair) const
    {
        return std::hash<std::size_t>()(pair.first) ^ std::hash<std::size_t>()(pair.second);
    }
};

static std::unordered_map<BlockType, QString> typeStringPairs {
    {GRASS, "Grass"},
    {DIRT, "Dirt"},
    {STONE, "Stone"},
    {COBBLESTONE, "Cobblestone"},
    {MOSS_STONE, "Moss Stone"},
    {SAND, "Sand"},
    {TALL_GRASS, "Tall Grass"},
    {WATER, "Water"},
    {LAVA, "Lava"},
    {BEDROCK, "Bedrock"},
    {CEDAR_PLANKS, "Cedar Planks"},
    {TEAK_PLANKS, "Teak Planks"},
    {CHERRY_PLANKS, "Cherry Planks"},
    {MAPLE_PLANKS, "Maple Planks"},
    {PINE_PLANKS, "Pine Planks"},
    {WISTERIA_PLANKS, "Wisteria Planks"},
    {CEDAR_WINDOW_X, "Cedar Window"},
    {TEAK_WINDOW_X, "Teak Window"},
    {CHERRY_WINDOW_X, "Cherry Window"},
    {MAPLE_WINDOW_X, "Maple Window"},
    {PINE_WINDOW_X, "Pine Window"},
    {WISTERIA_WINDOW_X, "Wisteria Window"},
    {RED_PAINTED_WOOD, "Red Painted Wood"},
    {BLACK_PAINTED_WOOD, "Black Painted Wood"},
    {ROOF_TILES_1, "Roof Tiles 1"},
    {ROOF_TILES_2, "Roof Tiles 2"},
    {ROOF_TILES, "Roof Tiles"},
    {TILLED_DIRT, "Tilled Dirt"},
    {PATH, "Path"},
    {CEDAR_WOOD_Y, "Cedar Wood"},
    {TEAK_WOOD_Y, "Teak Wood"},
    {CHERRY_WOOD_Y, "Cherry Wood"},
    {MAPLE_WOOD_Y, "Maple Wood"},
    {PINE_WOOD_Y, "Pine Wood"},
    {WISTERIA_WOOD_Y, "Wisteria Wood"},
    {CLOTH_1, "Recipe Book"},
    {SNOW_1, "Snow 1"},
    {SNOW_2, "Snow 2"},
    {SNOW_3, "Snow 3"},
    {SNOW_4, "Snow 4"},
    {SNOW_5, "Snow 5"},
    {SNOW_6, "Snow 6"},
    {SNOW_7, "Snow 7"},
    {SNOW_8, "Snow 8"},
    {ICE, "Ice"},
    {CEDAR_LEAVES, "Cedar Leaves"},
    {TEAK_LEAVES, "Teak Leaves"},
    {CHERRY_BLOSSOMS_1, "Cherry Blossoms 1"},
    {CHERRY_BLOSSOMS_2, "Cherry Blossoms 2"},
    {CHERRY_BLOSSOMS_3, "Cherry Blossoms 3"},
    {CHERRY_BLOSSOMS_4, "Cherry Blossoms 4"},
    {MAPLE_LEAVES_1, "Maple Leaves 1"},
    {MAPLE_LEAVES_2, "Maple Leaves 2"},
    {MAPLE_LEAVES_3, "Maple Leaves 3"},
    {PINE_LEAVES, "Pine Leaves"},
    {WISTERIA_BLOSSOMS_1, "Wisteria Blossoms 1"},
    {WISTERIA_BLOSSOMS_2, "Wisteria Blossoms 2"},
    {WISTERIA_BLOSSOMS_3, "Wisteria Blossoms 3"},
    {PLASTER, "Plaster"},
    {STRAW_1, "Straw 1"},
    {STRAW_2, "Straw 2"},
    {STRAW, "Straw"},
    {LILY_PAD, "Lily Pad"},
    {LOTUS_1, "Lotus 1"},
    {LOTUS_2, "Lotus 2"},
    {WHEAT_1, "Wheat 1"},
    {WHEAT_2, "Wheat 2"},
    {WHEAT_3, "Wheat 3"},
    {WHEAT_4, "Wheat 4"},
    {WHEAT_5, "Wheat 5"},
    {WHEAT_6, "Wheat 6"},
    {WHEAT_7, "Wheat 7"},
    {WHEAT_8, "Wheat 8"},
    {RICE_1, "Rice 1"},
    {RICE_2, "Rice 2"},
    {RICE_3, "Rice 3"},
    {RICE_4, "Rice 4"},
    {RICE_5, "Rice 5"},
    {RICE_6, "Rice 6"},
    {BAMBOO_1, "Bamboo 1"},
    {BAMBOO_2, "Bamboo 2"},
    {BAMBOO_3, "Bamboo 3"},
    {PAPER_LANTERN, "Paper Lantern"},
    {WOOD_LANTERN, "Wood Lantern"},
    {BONSAI_TREE, "Bonsai Tree"},
    {MAGNOLIA_IKEBANA, "Magnolia Ikebana"},
    {LOTUS_IKEBANA, "Lotus Ikebana"},
    {GREEN_HYDRANGEA_IKEBANA, "Green Hydrangea Ikebana"},
    {CHRYSANTHEMUM_IKEBANA, "Chrysanthemum Ikebana"},
    {CHERRY_BLOSSOM_IKEBANA, "Cherry Blossom Ikebana"},
    {BLUE_HYDRANGEA_IKEBANA, "Blue Hydrangea Ikebana"},
    {TULIP_IKEBANA, "Tulip Ikebana"},
    {DAFFODIL_IKEBANA, "Daffodil Ikebana"},
    {PLUM_BLOSSOM_IKEBANA, "Plum Blossom Ikebana"},
    {MAGNOLIA_BUD_IKEBANA, "Magnolia Bud Ikebana"},
    {POPPY_IKEBANA, "Poppy Ikebana"},
    {MAPLE_IKEBANA, "Maple Ikebana"},
    {ONCIDIUM_IKEBANA, "Oncidium Ikebana"},
    {GHOST_LILY, "Ghost Lily"},
    {GHOST_WEED, "Ghost Weed"},
    {CORAL_1, "Coral 1"},
    {CORAL_2, "Coral 2"},
    {CORAL_3, "Coral 3"},
    {CORAL_4, "Coral 4"},
    {KELP_1, "Kelp 1"},
    {KELP_2, "Kelp 2"},
    {SEA_GRASS, "Sea Grass"}
};

static std::unordered_map<std::pair<BlockType, BlockType>, BlockType, TypePairHash> craftingRecipes {
    {std::make_pair(GRASS, DIRT), SAND},
    {std::make_pair(ICE, WATER), SNOW_1},
    {std::make_pair(SNOW_1, WATER), ICE},
    {std::make_pair(STONE, COBBLESTONE), MOSS_STONE},
    {std::make_pair(DIRT, WATER), TILLED_DIRT},
    {std::make_pair(TILLED_DIRT, STRAW), WHEAT_1},
    {std::make_pair(WHEAT_8, WATER), RICE_1},
    {std::make_pair(RICE_6, WATER), RICE_6},
    {std::make_pair(TEAK_PLANKS, TEAK_PLANKS), CEDAR_PLANKS},
    {std::make_pair(CEDAR_PLANKS, CEDAR_PLANKS), TEAK_PLANKS},
    {std::make_pair(CHERRY_PLANKS, CHERRY_PLANKS), MAPLE_PLANKS},
    {std::make_pair(MAPLE_PLANKS, MAPLE_PLANKS), CHERRY_PLANKS},
    {std::make_pair(PINE_PLANKS, PINE_PLANKS), WISTERIA_PLANKS},
    {std::make_pair(WISTERIA_PLANKS, WISTERIA_PLANKS), PINE_PLANKS},
    {std::make_pair(WISTERIA_BLOSSOMS_1, WISTERIA_BLOSSOMS_2), WISTERIA_BLOSSOMS_3},
    {std::make_pair(WISTERIA_BLOSSOMS_3, WISTERIA_BLOSSOMS_2), WISTERIA_BLOSSOMS_1},
    {std::make_pair(BAMBOO_1, BAMBOO_1), BAMBOO_2},
    {std::make_pair(BAMBOO_2, BAMBOO_1), BAMBOO_3},
    {std::make_pair(BAMBOO_3, BAMBOO_2), BAMBOO_1},
    {std::make_pair(LILY_PAD, WATER), LOTUS_1},
    {std::make_pair(LOTUS_1, WATER), LOTUS_2},
    {std::make_pair(LOTUS_2, WATER), LILY_PAD}
};

class InventoryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit InventoryWindow(QWidget *parent = nullptr);
    ~InventoryWindow();

    void keyPressEvent(QKeyEvent *e);
    Ui::MainWindow *ui_main;

    void populateTable(std::map<BlockType, InventoryItem>);

    BlockType crafting_item1;
    BlockType crafting_item2;

    InventoryItem* currItem;

    bool canCraft(BlockType, BlockType);

    bool recipeExists(BlockType, BlockType);

public slots:

    void slot_blockSelected(QListWidgetItem*);

    void slot_crafting_item1();
    void slot_crafting_item2();
    void slot_crafting_craft();

private:
    Ui::InventoryWindow *ui;
};

#endif // INVENTORYWINDOW_H
