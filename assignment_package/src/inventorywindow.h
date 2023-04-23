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

    void populateTable(std::map<BlockType, InventoryItem>);

    std::unordered_map<BlockType, QString> typeStringPairs {
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


//        {std::make_pair(SNOW_1, XPOS),},


//        {std::make_pair(SNOW_2, XPOS),},


//        {std::make_pair(SNOW_3, XPOS),},


//        {std::make_pair(SNOW_4, XPOS),},


//        {std::make_pair(SNOW_5, XPOS),},


//        {std::make_pair(SNOW_6, XPOS),},

//        {std::make_pair(SNOW_7, XPOS),},


//        {std::make_pair(SNOW_8, XPOS),},


//        {std::make_pair(ICE, XPOS),},


        {CEDAR_WOOD, "Cedar Wood"},


        {TEAK_WOOD, "Teak Wood"},


        {CHERRY_WOOD, "Cherry Wood"},


        {MAPLE_WOOD, "Maple Wood"},


        {PINE_WOOD, "Pine Wood"},


        {WISTERIA_WOOD, "Wisteria Wood"},


//        {std::make_pair(CEDAR_LEAVES, XPOS), },


//        {std::make_pair(TEAK_LEAVES, XPOS), },


//        {std::make_pair(CHERRY_BLOSSOMS_1, XPOS),},


//        {std::make_pair(CHERRY_BLOSSOMS_2, XPOS),},


//        {std::make_pair(CHERRY_BLOSSOMS_3, XPOS), },


//        {std::make_pair(CHERRY_BLOSSOMS_4, XPOS), },


//        {std::make_pair(MAPLE_LEAVES_1, XPOS),},


//        {std::make_pair(MAPLE_LEAVES_2, XPOS), },


//        {std::make_pair(MAPLE_LEAVES_3, XPOS), },


//        {std::make_pair(PINE_LEAVES, XPOS), },


//        {std::make_pair(WISTERIA_BLOSSOMS_1, XPOS), },


//        {std::make_pair(WISTERIA_BLOSSOMS_2, XPOS), },


//        {std::make_pair(WISTERIA_BLOSSOMS_3, XPOS), },


        {CEDAR_PLANKS, "Cedar Planks"},


        {TEAK_PLANKS, "Teak Planks"},


        {CHERRY_PLANKS, "Cherry Planks"},


        {MAPLE_PLANKS, "Maple Planks"},


        {PINE_PLANKS, "Pine Planks"},


        {WISTERIA_PLANKS, "Wisteria Planks"},


        {CEDAR_WINDOW, "Cedar Window"},


        {TEAK_WINDOW, "Teak Window"},


        {CHERRY_WINDOW, "Cherry Window"},


        {MAPLE_WINDOW, "Maple Window"},


        {PINE_WINDOW, "Pine Window"},


        {WISTERIA_WINDOW, "Wisteria Window"},


        {RED_PAINTED_WOOD, "Red Painted Wood"},


        {BLACK_PAINTED_WOOD, "Black Painted Wood"},


//        {std::make_pair(PLASTER, XPOS), },


        {ROOF_TILES_1, "Roof Tiles 1"},


        {ROOF_TILES_2, "Roof Tiles 2"},


        {ROOF_TILES, "Roof Tiles"},


//        {std::make_pair(STRAW_1, XPOS), },


//        {std::make_pair(STRAW_2, XPOS), },


//        {std::make_pair(STRAW, XPOS), },


//        {std::make_pair(LILY_PAD, YPOS),},


//        {std::make_pair(LOTUS_1, YPOS),},


//        {std::make_pair(LOTUS_2, YPOS),},


        {TILLED_DIRT, "Tilled Dirt"},


        {PATH, "Path"},


//        {std::make_pair(WHEAT_1, XPOS),},


//        {std::make_pair(WHEAT_2, XPOS),},


//        {std::make_pair(WHEAT_3, XPOS),},


//        {std::make_pair(WHEAT_4, XPOS),},


//        {std::make_pair(WHEAT_5, XPOS),},


//        {std::make_pair(WHEAT_6, XPOS),},


//        {std::make_pair(WHEAT_7, XPOS),},

//        {std::make_pair(WHEAT_8, XPOS),},


//        {std::make_pair(RICE_1, XPOS_ZPOS),},


//        {std::make_pair(RICE_2, XPOS_ZPOS),},


//        {std::make_pair(RICE_3, XPOS_ZPOS),},


//        {std::make_pair(RICE_4, XPOS_ZPOS),},


//        {std::make_pair(RICE_5, XPOS_ZPOS),},


//        {std::make_pair(RICE_6, XPOS_ZPOS),},


//        {std::make_pair(BAMBOO_1, XPOS), },
//        {std::make_pair(BAMBOO_2, XPOS),},


//        {std::make_pair(BAMBOO_3, XPOS), },

//        {std::make_pair(TATAMI, XPOS), },


//        {std::make_pair(PAPER_LANTERN, XPOS), },


//        {std::make_pair(WOOD_LANTERN, XPOS), },


//        {std::make_pair(PAINTING_1, XPOS), },


//        {std::make_pair(PAINTING_2, XPOS), },


//        {std::make_pair(PAINTING_3, XPOS), },

//        {std::make_pair(PAINTING_4, XPOS), },


//        {std::make_pair(PAINTING_5, XPOS), },


//        {std::make_pair(PAINTING_6L, XPOS),},


//        {std::make_pair(PAINTING_6R, XPOS), },


//        {std::make_pair(PAINTING_7T, XPOS), },


//        {std::make_pair(PAINTING_7B, XPOS), },


//        {std::make_pair(BONSAI_TREE, XPOS),},


//        {std::make_pair(MAGNOLIA_IKEBANA, XPOS),},


//        {std::make_pair(LOTUS_IKEBANA, XPOS),},


//        {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XPOS),},


//        {std::make_pair(CHRYSANTHEMUM_IKEBANA, XPOS),},


//        {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XPOS),},


//        {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XPOS),},

//        {std::make_pair(TULIP_IKEBANA, XPOS),},


//        {std::make_pair(DAFFODIL_IKEBANA, XPOS),},


//        {std::make_pair(PLUM_BLOSSOM_IKEBANA, XPOS),},


//        {std::make_pair(MAGNOLIA_BUD_IKEBANA, XPOS),},


//        {std::make_pair(POPPY_IKEBANA, XPOS),},


//        {std::make_pair(MAPLE_IKEBANA, XPOS),},

//        {std::make_pair(ONCIDIUM_IKEBANA, XPOS),},


//        {std::make_pair(GHOST_LILY, XPOS_ZPOS),},


//        {std::make_pair(GHOST_WEED, XPOS_ZPOS), },


//        {std::make_pair(CORAL_1, XPOS_ZPOS),},


//        {std::make_pair(CORAL_2, XPOS_ZPOS), },


//        {std::make_pair(CORAL_3, XPOS_ZPOS), },


//        {std::make_pair(CORAL_4, XPOS_ZPOS), },


//        {std::make_pair(KELP_1, XPOS_ZPOS),},


//        {std::make_pair(KELP_2, XPOS_ZPOS), },


//        {std::make_pair(SEA_GRASS, XPOS),},

    };

public slots:

    void slot_blockSelected(QListWidgetItem*);

private:
    Ui::InventoryWindow *ui;
};

#endif // INVENTORYWINDOW_H
