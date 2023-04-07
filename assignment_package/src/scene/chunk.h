#pragma once
#include "drawable.h"
#include "smartpointerhelp.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include <unordered_set>


//using namespace std;

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, COBBLESTONE, MOSS_STONE, SAND,
    TALL_GRASS, WATER, LAVA, BEDROCK, SNOW_1, SNOW_2, SNOW_3, SNOW_4, SNOW_5, SNOW_6, SNOW_7, SNOW_8, ICE,
    RED_PAINTED_WOOD, BLACK_PAINTED_WOOD, PLASTER,
    ROOF_TILES_1, ROOF_TILES_2, ROOF_TILES_3, ROOF_TILES_4, STRAW_1, STRAW_2, STRAW_3, STRAW_4,
    CEDAR_WOOD, TEAK_WOOD, CHERRY_WOOD, MAPLE_WOOD, PINE_WOOD, WISTERIA_WOOD,
    CEDAR_LEAVES, TEAK_LEAVES, CHERRY_BLOSSOMS_1, CHERRY_BLOSSOMS_2, CHERRY_BLOSSOMS_3, CHERRY_BLOSSOMS_4,
    MAPLE_LEAVES_1, MAPLE_LEAVES_2, MAPLE_LEAVES_3,
    PINE_LEAVES, WISTERIA_BLOSSOMS_1, WISTERIA_BLOSSOMS_2, WISTERIA_BLOSSOMS_3,
    CEDAR_PLANKS, TEAK_PLANKS, CHERRY_PLANKS, MAPLE_PLANKS, PINE_PLANKS, WISTERIA_PLANKS,
    CEDAR_WINDOW, TEAK_WINDOW, CHERRY_WINDOW, MAPLE_WINDOW, PINE_WINDOW, WISTERIA_WINDOW,
    LILY_PAD, LOTUS_1, LOTUS_2, LOTUS_3, TILLED_DIRT, PATH, WHEAT, RICE, BAMBOO_1, BAMBOO_2, BAMBOO_3,
    TATAMI, PAPER_LANTERN, WOOD_LANTERN,
    PAINTING, PAINTING_T, PAINTING_B, PAINTING_L, PAINTING_R, TEA_KETTLE,
    BONSAI_TREE, MAGNOLIA_IKEBANA, LOTUS_IKEBANA,
    GREEN_HYDRANGEA_IKEBANA, CHRYSANTHEMUM_IKEBANA,
    CHERRY_BLOSSOM_IKEBANA, BLUE_HYDRANGEA_IKEBANA, TULIP_IKEBANA, ONCIDIUM_IKEBANA, DAFFODIL_IKEBANA,
    PLUM_BLOSSOM_IKEBANA, MAGNOLIA_BUD_IKEBANA, POPPY_IKEBANA, MAPLE_IKEBANA,
    GHOST_LILY, GHOST_WEED, CORAL, KELP, SEA_GRASS
};

// The six cardinal directions in 3D space + diagonals (rotated 45 degrees)
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG, XPOS_ZPOS, XNEG_ZNEG, XPOS_ZNEG, XNEG_ZPOS
};

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS},
    {XPOS_ZPOS, XNEG_ZNEG},
    {XNEG_ZNEG, XPOS_ZPOS},
    {XPOS_ZNEG, XNEG_ZPOS},
    {XNEG_ZPOS, XPOS_ZNEG}
};

// maps blocktype and direction to texture flag and uv coord
const static std::unordered_map<std::pair<BlockType, Direction>, std::pair<int, glm::vec2>, std::pair<EnumHash, EnumHash>> btToUV {
    {std::make_pair(GRASS, XPOS), std::make_pair(1, glm::vec2(0, 13))},
    {std::make_pair(GRASS, XNEG), std::make_pair(1, glm::vec2(0, 13))},
    {std::make_pair(GRASS, ZPOS), std::make_pair(1, glm::vec2(0, 13))},
    {std::make_pair(GRASS, ZNEG), std::make_pair(1, glm::vec2(0, 13))},
    {std::make_pair(GRASS, YPOS), std::make_pair(0, glm::vec2(0, 11))},
    {std::make_pair(GRASS, YNEG), std::make_pair(0, glm::vec2(0, 13))},

    {std::make_pair(DIRT, XPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(DIRT, XNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(DIRT, ZPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(DIRT, ZNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(DIRT, YPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(DIRT, YNEG), std::make_pair(0, glm::vec2(0, 13))},

    {std::make_pair(STONE, XPOS), std::make_pair(0, glm::vec2(1, 15))},
    {std::make_pair(STONE, XNEG), std::make_pair(0, glm::vec2(1, 15))},
    {std::make_pair(STONE, ZPOS), std::make_pair(0, glm::vec2(1, 15))},
    {std::make_pair(STONE, ZNEG), std::make_pair(0, glm::vec2(1, 15))},
    {std::make_pair(STONE, YPOS), std::make_pair(0, glm::vec2(1, 15))},
    {std::make_pair(STONE, YNEG), std::make_pair(0, glm::vec2(1, 15))},

    {std::make_pair(COBBLESTONE, XPOS), std::make_pair(0, glm::vec2(0, 15))},
    {std::make_pair(COBBLESTONE, XNEG), std::make_pair(0, glm::vec2(0, 15))},
    {std::make_pair(COBBLESTONE, ZPOS), std::make_pair(0, glm::vec2(0, 15))},
    {std::make_pair(COBBLESTONE, ZNEG), std::make_pair(0, glm::vec2(0, 15))},
    {std::make_pair(COBBLESTONE, YPOS), std::make_pair(0, glm::vec2(0, 15))},
    {std::make_pair(COBBLESTONE, YNEG), std::make_pair(0, glm::vec2(0, 15))},

    {std::make_pair(MOSS_STONE, XPOS), std::make_pair(0, glm::vec2(3, 15))},
    {std::make_pair(MOSS_STONE, XNEG), std::make_pair(0, glm::vec2(3, 15))},
    {std::make_pair(MOSS_STONE, ZPOS), std::make_pair(0, glm::vec2(3, 15))},
    {std::make_pair(MOSS_STONE, ZNEG), std::make_pair(0, glm::vec2(3, 15))},
    {std::make_pair(MOSS_STONE, YPOS), std::make_pair(0, glm::vec2(3, 15))},
    {std::make_pair(MOSS_STONE, YNEG), std::make_pair(0, glm::vec2(3, 15))},

    {std::make_pair(SAND, XPOS), std::make_pair(0, glm::vec2(0, 14))},
    {std::make_pair(SAND, XNEG), std::make_pair(0, glm::vec2(0, 14))},
    {std::make_pair(SAND, ZPOS), std::make_pair(0, glm::vec2(0, 14))},
    {std::make_pair(SAND, ZNEG), std::make_pair(0, glm::vec2(0, 14))},
    {std::make_pair(SAND, YPOS), std::make_pair(0, glm::vec2(0, 14))},
    {std::make_pair(SAND, YNEG), std::make_pair(0, glm::vec2(0, 14))},

    {std::make_pair(TALL_GRASS, XPOS_ZPOS), std::make_pair(0, glm::vec2(0, 10))},
    {std::make_pair(TALL_GRASS, XNEG_ZNEG), std::make_pair(0, glm::vec2(5, 10))},
    {std::make_pair(TALL_GRASS, XPOS_ZNEG), std::make_pair(0, glm::vec2(0, 10))},
    {std::make_pair(TALL_GRASS, XNEG_ZPOS), std::make_pair(0, glm::vec2(5, 10))},

    {std::make_pair(WATER, XPOS), std::make_pair(2, glm::vec2(13, 3))},
    {std::make_pair(WATER, XNEG), std::make_pair(2, glm::vec2(13, 3))},
    {std::make_pair(WATER, ZPOS), std::make_pair(2, glm::vec2(13, 3))},
    {std::make_pair(WATER, ZNEG), std::make_pair(2, glm::vec2(13, 3))},
    {std::make_pair(WATER, YPOS), std::make_pair(2, glm::vec2(13, 3))},
    {std::make_pair(WATER, YNEG), std::make_pair(2, glm::vec2(13, 3))},

    {std::make_pair(LAVA, XPOS), std::make_pair(2, glm::vec2(13, 1))},
    {std::make_pair(LAVA, XNEG), std::make_pair(2, glm::vec2(13, 1))},
    {std::make_pair(LAVA, ZPOS), std::make_pair(2, glm::vec2(13, 1))},
    {std::make_pair(LAVA, ZNEG), std::make_pair(2, glm::vec2(13, 1))},
    {std::make_pair(LAVA, YPOS), std::make_pair(2, glm::vec2(13, 1))},
    {std::make_pair(LAVA, YNEG), std::make_pair(2, glm::vec2(13, 1))},

    {std::make_pair(BEDROCK, XPOS), std::pair(0, glm::vec2(2, 15))},
    {std::make_pair(BEDROCK, XNEG), std::pair(0, glm::vec2(2, 15))},
    {std::make_pair(BEDROCK, ZPOS), std::pair(0, glm::vec2(2, 15))},
    {std::make_pair(BEDROCK, ZNEG), std::pair(0, glm::vec2(2, 15))},
    {std::make_pair(BEDROCK, YPOS), std::pair(0, glm::vec2(2, 15))},
    {std::make_pair(BEDROCK, YNEG), std::pair(0, glm::vec2(2, 15))},

    {std::make_pair(SNOW_1, XPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_1, XNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_1, ZPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_1, ZNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_1, YPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_1, YNEG), std::make_pair(0, glm::vec2(1, 14))},

    {std::make_pair(SNOW_2, XPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_2, XNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_2, ZPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_2, ZNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_2, YPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_2, YNEG), std::make_pair(0, glm::vec2(1, 14))},

    {std::make_pair(SNOW_3, XPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_3, XNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_3, ZPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_3, ZNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_3, YPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_3, YNEG), std::make_pair(0, glm::vec2(1, 14))},

    {std::make_pair(SNOW_4, XPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_4, XNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_4, ZPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_4, ZNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_4, YPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_4, YNEG), std::make_pair(0, glm::vec2(1, 14))},

    {std::make_pair(SNOW_5, XPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_5, XNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_5, ZPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_5, ZNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_5, YPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_5, YNEG), std::make_pair(0, glm::vec2(1, 14))},

    {std::make_pair(SNOW_6, XPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_6, XNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_6, ZPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_6, ZNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_6, YPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_6, YNEG), std::make_pair(0, glm::vec2(1, 14))},

    {std::make_pair(SNOW_7, XPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_7, XNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_7, ZPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_7, ZNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_7, YPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_7, YNEG), std::make_pair(0, glm::vec2(1, 14))},

    {std::make_pair(SNOW_8, XPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_8, XNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_8, ZPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_8, ZNEG), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_8, YPOS), std::make_pair(0, glm::vec2(1, 14))},
    {std::make_pair(SNOW_8, YNEG), std::make_pair(0, glm::vec2(1, 14))},

    {std::make_pair(ICE, XPOS), std::make_pair(0, glm::vec2(2, 14))},
    {std::make_pair(ICE, XNEG), std::make_pair(0, glm::vec2(2, 14))},
    {std::make_pair(ICE, ZPOS), std::make_pair(0, glm::vec2(2, 14))},
    {std::make_pair(ICE, ZNEG), std::make_pair(0, glm::vec2(2, 14))},
    {std::make_pair(ICE, YPOS), std::make_pair(0, glm::vec2(2, 14))},
    {std::make_pair(ICE, YNEG), std::make_pair(0, glm::vec2(2, 14))},

    {std::make_pair(CEDAR_WOOD, XPOS), std::make_pair(0, glm::vec2(5, 12))},
    {std::make_pair(CEDAR_WOOD, XNEG), std::make_pair(0, glm::vec2(5, 12))},
    {std::make_pair(CEDAR_WOOD, ZPOS), std::make_pair(0, glm::vec2(5, 12))},
    {std::make_pair(CEDAR_WOOD, ZNEG), std::make_pair(0, glm::vec2(5, 12))},
    {std::make_pair(CEDAR_WOOD, YPOS), std::make_pair(0, glm::vec2(5, 12))},
    {std::make_pair(CEDAR_WOOD, YNEG), std::make_pair(0, glm::vec2(5, 12))},

    {std::make_pair(TEAK_WOOD, XPOS), std::make_pair(0, glm::vec2(6, 12))},
    {std::make_pair(TEAK_WOOD, XNEG), std::make_pair(0, glm::vec2(6, 12))},
    {std::make_pair(TEAK_WOOD, ZPOS), std::make_pair(0, glm::vec2(6, 12))},
    {std::make_pair(TEAK_WOOD, ZNEG), std::make_pair(0, glm::vec2(6, 12))},
    {std::make_pair(TEAK_WOOD, YPOS), std::make_pair(0, glm::vec2(6, 12))},
    {std::make_pair(TEAK_WOOD, YNEG), std::make_pair(0, glm::vec2(6, 12))},

    {std::make_pair(CHERRY_WOOD, XPOS), std::make_pair(0, glm::vec2(7, 12))},
    {std::make_pair(CHERRY_WOOD, XNEG), std::make_pair(0, glm::vec2(7, 12))},
    {std::make_pair(CHERRY_WOOD, ZPOS), std::make_pair(0, glm::vec2(7, 12))},
    {std::make_pair(CHERRY_WOOD, ZNEG), std::make_pair(0, glm::vec2(7, 12))},
    {std::make_pair(CHERRY_WOOD, YPOS), std::make_pair(0, glm::vec2(7, 12))},
    {std::make_pair(CHERRY_WOOD, YNEG), std::make_pair(0, glm::vec2(7, 12))},

    {std::make_pair(MAPLE_WOOD, XPOS), std::make_pair(0, glm::vec2(8, 12))},
    {std::make_pair(MAPLE_WOOD, XNEG), std::make_pair(0, glm::vec2(8, 12))},
    {std::make_pair(MAPLE_WOOD, ZPOS), std::make_pair(0, glm::vec2(8, 12))},
    {std::make_pair(MAPLE_WOOD, ZNEG), std::make_pair(0, glm::vec2(8, 12))},
    {std::make_pair(MAPLE_WOOD, YPOS), std::make_pair(0, glm::vec2(8, 12))},
    {std::make_pair(MAPLE_WOOD, YNEG), std::make_pair(0, glm::vec2(8, 12))},

    {std::make_pair(PINE_WOOD, XPOS), std::make_pair(0, glm::vec2(9, 12))},
    {std::make_pair(PINE_WOOD, XNEG), std::make_pair(0, glm::vec2(9, 12))},
    {std::make_pair(PINE_WOOD, ZPOS), std::make_pair(0, glm::vec2(9, 12))},
    {std::make_pair(PINE_WOOD, ZNEG), std::make_pair(0, glm::vec2(9, 12))},
    {std::make_pair(PINE_WOOD, YPOS), std::make_pair(0, glm::vec2(9, 12))},
    {std::make_pair(PINE_WOOD, YNEG), std::make_pair(0, glm::vec2(9, 12))},

    {std::make_pair(WISTERIA_WOOD, XPOS), std::make_pair(0, glm::vec2(10, 12))},
    {std::make_pair(WISTERIA_WOOD, XNEG), std::make_pair(0, glm::vec2(10, 12))},
    {std::make_pair(WISTERIA_WOOD, ZPOS), std::make_pair(0, glm::vec2(10, 12))},
    {std::make_pair(WISTERIA_WOOD, ZNEG), std::make_pair(0, glm::vec2(10, 12))},
    {std::make_pair(WISTERIA_WOOD, YPOS), std::make_pair(0, glm::vec2(10, 12))},
    {std::make_pair(WISTERIA_WOOD, YNEG), std::make_pair(0, glm::vec2(10, 12))},

    {std::make_pair(CEDAR_LEAVES, XPOS), std::make_pair(0, glm::vec2(5, 11))},
    {std::make_pair(CEDAR_LEAVES, XNEG), std::make_pair(0, glm::vec2(5, 11))},
    {std::make_pair(CEDAR_LEAVES, ZPOS), std::make_pair(0, glm::vec2(5, 11))},
    {std::make_pair(CEDAR_LEAVES, ZNEG), std::make_pair(0, glm::vec2(5, 11))},
    {std::make_pair(CEDAR_LEAVES, YPOS), std::make_pair(0, glm::vec2(5, 11))},
    {std::make_pair(CEDAR_LEAVES, YNEG), std::make_pair(0, glm::vec2(5, 11))},

    {std::make_pair(TEAK_LEAVES, XPOS), std::make_pair(0, glm::vec2(6, 11))},
    {std::make_pair(TEAK_LEAVES, XNEG), std::make_pair(0, glm::vec2(6, 11))},
    {std::make_pair(TEAK_LEAVES, ZPOS), std::make_pair(0, glm::vec2(6, 11))},
    {std::make_pair(TEAK_LEAVES, ZNEG), std::make_pair(0, glm::vec2(6, 11))},
    {std::make_pair(TEAK_LEAVES, YPOS), std::make_pair(0, glm::vec2(6, 11))},
    {std::make_pair(TEAK_LEAVES, YNEG), std::make_pair(0, glm::vec2(6, 11))},

    {std::make_pair(CHERRY_BLOSSOMS_1, XPOS), std::make_pair(0, glm::vec2(7, 8))},
    {std::make_pair(CHERRY_BLOSSOMS_1, XNEG), std::make_pair(0, glm::vec2(7, 8))},
    {std::make_pair(CHERRY_BLOSSOMS_1, ZPOS), std::make_pair(0, glm::vec2(7, 8))},
    {std::make_pair(CHERRY_BLOSSOMS_1, ZNEG), std::make_pair(0, glm::vec2(7, 8))},
    {std::make_pair(CHERRY_BLOSSOMS_1, YPOS), std::make_pair(0, glm::vec2(7, 8))},
    {std::make_pair(CHERRY_BLOSSOMS_1, YNEG), std::make_pair(0, glm::vec2(7, 8))},

    {std::make_pair(CHERRY_BLOSSOMS_2, XPOS), std::make_pair(0, glm::vec2(7, 9))},
    {std::make_pair(CHERRY_BLOSSOMS_2, XNEG), std::make_pair(0, glm::vec2(7, 9))},
    {std::make_pair(CHERRY_BLOSSOMS_2, ZPOS), std::make_pair(0, glm::vec2(7, 9))},
    {std::make_pair(CHERRY_BLOSSOMS_2, ZNEG), std::make_pair(0, glm::vec2(7, 9))},
    {std::make_pair(CHERRY_BLOSSOMS_2, YPOS), std::make_pair(0, glm::vec2(7, 9))},
    {std::make_pair(CHERRY_BLOSSOMS_2, YNEG), std::make_pair(0, glm::vec2(7, 9))},

    {std::make_pair(CHERRY_BLOSSOMS_3, XPOS), std::make_pair(0, glm::vec2(7, 10))},
    {std::make_pair(CHERRY_BLOSSOMS_3, XNEG), std::make_pair(0, glm::vec2(7, 10))},
    {std::make_pair(CHERRY_BLOSSOMS_3, ZPOS), std::make_pair(0, glm::vec2(7, 10))},
    {std::make_pair(CHERRY_BLOSSOMS_3, ZNEG), std::make_pair(0, glm::vec2(7, 10))},
    {std::make_pair(CHERRY_BLOSSOMS_3, YPOS), std::make_pair(0, glm::vec2(7, 10))},
    {std::make_pair(CHERRY_BLOSSOMS_3, YNEG), std::make_pair(0, glm::vec2(7, 10))},

    {std::make_pair(CHERRY_BLOSSOMS_4, XPOS), std::make_pair(0, glm::vec2(7, 11))},
    {std::make_pair(CHERRY_BLOSSOMS_4, XNEG), std::make_pair(0, glm::vec2(7, 11))},
    {std::make_pair(CHERRY_BLOSSOMS_4, ZPOS), std::make_pair(0, glm::vec2(7, 11))},
    {std::make_pair(CHERRY_BLOSSOMS_4, ZNEG), std::make_pair(0, glm::vec2(7, 11))},
    {std::make_pair(CHERRY_BLOSSOMS_4, YPOS), std::make_pair(0, glm::vec2(7, 11))},
    {std::make_pair(CHERRY_BLOSSOMS_4, YNEG), std::make_pair(0, glm::vec2(7, 11))},

    {std::make_pair(MAPLE_LEAVES_1, XPOS), std::make_pair(0, glm::vec2(8, 9))},
    {std::make_pair(MAPLE_LEAVES_1, XNEG), std::make_pair(0, glm::vec2(8, 9))},
    {std::make_pair(MAPLE_LEAVES_1, ZPOS), std::make_pair(0, glm::vec2(8, 9))},
    {std::make_pair(MAPLE_LEAVES_1, ZNEG), std::make_pair(0, glm::vec2(8, 9))},
    {std::make_pair(MAPLE_LEAVES_1, YPOS), std::make_pair(0, glm::vec2(8, 9))},
    {std::make_pair(MAPLE_LEAVES_1, YNEG), std::make_pair(0, glm::vec2(8, 9))},

    {std::make_pair(MAPLE_LEAVES_2, XPOS), std::make_pair(0, glm::vec2(8, 10))},
    {std::make_pair(MAPLE_LEAVES_2, XNEG), std::make_pair(0, glm::vec2(8, 10))},
    {std::make_pair(MAPLE_LEAVES_2, ZPOS), std::make_pair(0, glm::vec2(8, 10))},
    {std::make_pair(MAPLE_LEAVES_2, ZNEG), std::make_pair(0, glm::vec2(8, 10))},
    {std::make_pair(MAPLE_LEAVES_2, YPOS), std::make_pair(0, glm::vec2(8, 10))},
    {std::make_pair(MAPLE_LEAVES_2, YNEG), std::make_pair(0, glm::vec2(8, 10))},

    {std::make_pair(MAPLE_LEAVES_3, XPOS), std::make_pair(0, glm::vec2(8, 11))},
    {std::make_pair(MAPLE_LEAVES_3, XNEG), std::make_pair(0, glm::vec2(8, 11))},
    {std::make_pair(MAPLE_LEAVES_3, ZPOS), std::make_pair(0, glm::vec2(8, 11))},
    {std::make_pair(MAPLE_LEAVES_3, ZNEG), std::make_pair(0, glm::vec2(8, 11))},
    {std::make_pair(MAPLE_LEAVES_3, YPOS), std::make_pair(0, glm::vec2(8, 11))},
    {std::make_pair(MAPLE_LEAVES_3, YNEG), std::make_pair(0, glm::vec2(8, 11))},

    {std::make_pair(PINE_LEAVES, XPOS), std::make_pair(0, glm::vec2(9, 11))},
    {std::make_pair(PINE_LEAVES, XNEG), std::make_pair(0, glm::vec2(9, 11))},
    {std::make_pair(PINE_LEAVES, ZPOS), std::make_pair(0, glm::vec2(9, 11))},
    {std::make_pair(PINE_LEAVES, ZNEG), std::make_pair(0, glm::vec2(9, 11))},
    {std::make_pair(PINE_LEAVES, YPOS), std::make_pair(0, glm::vec2(9, 11))},
    {std::make_pair(PINE_LEAVES, YNEG), std::make_pair(0, glm::vec2(9, 11))},

    {std::make_pair(WISTERIA_BLOSSOMS_1, XPOS), std::make_pair(0, glm::vec2(10, 9))},
    {std::make_pair(WISTERIA_BLOSSOMS_1, XNEG), std::make_pair(0, glm::vec2(10, 9))},
    {std::make_pair(WISTERIA_BLOSSOMS_1, ZPOS), std::make_pair(0, glm::vec2(10, 9))},
    {std::make_pair(WISTERIA_BLOSSOMS_1, ZNEG), std::make_pair(0, glm::vec2(10, 9))},
    {std::make_pair(WISTERIA_BLOSSOMS_1, YPOS), std::make_pair(0, glm::vec2(10, 9))},
    {std::make_pair(WISTERIA_BLOSSOMS_1, YNEG), std::make_pair(0, glm::vec2(10, 9))},

    {std::make_pair(WISTERIA_BLOSSOMS_2, XPOS), std::make_pair(0, glm::vec2(10, 10))},
    {std::make_pair(WISTERIA_BLOSSOMS_2, XNEG), std::make_pair(0, glm::vec2(10, 10))},
    {std::make_pair(WISTERIA_BLOSSOMS_2, ZPOS), std::make_pair(0, glm::vec2(10, 10))},
    {std::make_pair(WISTERIA_BLOSSOMS_2, ZNEG), std::make_pair(0, glm::vec2(10, 10))},
    {std::make_pair(WISTERIA_BLOSSOMS_2, YPOS), std::make_pair(0, glm::vec2(10, 10))},
    {std::make_pair(WISTERIA_BLOSSOMS_2, YNEG), std::make_pair(0, glm::vec2(10, 10))},

    {std::make_pair(WISTERIA_BLOSSOMS_3, XPOS), std::make_pair(0, glm::vec2(10, 11))},
    {std::make_pair(WISTERIA_BLOSSOMS_3, XNEG), std::make_pair(0, glm::vec2(10, 11))},
    {std::make_pair(WISTERIA_BLOSSOMS_3, ZPOS), std::make_pair(0, glm::vec2(10, 11))},
    {std::make_pair(WISTERIA_BLOSSOMS_3, ZNEG), std::make_pair(0, glm::vec2(10, 11))},
    {std::make_pair(WISTERIA_BLOSSOMS_3, YPOS), std::make_pair(0, glm::vec2(10, 11))},
    {std::make_pair(WISTERIA_BLOSSOMS_3, YNEG), std::make_pair(0, glm::vec2(10, 11))},

    {std::make_pair(CEDAR_PLANKS, XPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS, XNEG), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS, ZPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS, ZNEG), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS, YPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS, YNEG), std::make_pair(0, glm::vec2(5, 15))},

    {std::make_pair(TEAK_PLANKS, XPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS, XNEG), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS, ZPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS, ZNEG), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS, YPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS, YNEG), std::make_pair(0, glm::vec2(6, 15))},

    {std::make_pair(CHERRY_PLANKS, XPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS, XNEG), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS, ZPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS, ZNEG), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS, YPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS, YNEG), std::make_pair(0, glm::vec2(7, 15))},

    {std::make_pair(MAPLE_PLANKS, XPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS, XNEG), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS, ZPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS, ZNEG), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS, YPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS, YNEG), std::make_pair(0, glm::vec2(8, 15))},

    {std::make_pair(PINE_PLANKS, XPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS, XNEG), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS, ZPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS, ZNEG), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS, YPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS, YNEG), std::make_pair(0, glm::vec2(9, 15))},

    {std::make_pair(WISTERIA_PLANKS, XPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS, XNEG), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS, ZPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS, ZNEG), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS, YPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS, YNEG), std::make_pair(0, glm::vec2(10, 15))},

    {std::make_pair(CEDAR_WINDOW, XPOS), std::make_pair(0, glm::vec2(5, 14))},
    {std::make_pair(CEDAR_WINDOW, XNEG), std::make_pair(0, glm::vec2(5, 14))},
    {std::make_pair(CEDAR_WINDOW, ZPOS), std::make_pair(0, glm::vec2(5, 14))},
    {std::make_pair(CEDAR_WINDOW, ZNEG), std::make_pair(0, glm::vec2(5, 14))},
    {std::make_pair(CEDAR_WINDOW, YPOS), std::make_pair(0, glm::vec2(5, 14))},
    {std::make_pair(CEDAR_WINDOW, YNEG), std::make_pair(0, glm::vec2(5, 14))},

    {std::make_pair(TEAK_WINDOW, XPOS), std::make_pair(0, glm::vec2(6, 14))},
    {std::make_pair(TEAK_WINDOW, XNEG), std::make_pair(0, glm::vec2(6, 14))},
    {std::make_pair(TEAK_WINDOW, ZPOS), std::make_pair(0, glm::vec2(6, 14))},
    {std::make_pair(TEAK_WINDOW, ZNEG), std::make_pair(0, glm::vec2(6, 14))},
    {std::make_pair(TEAK_WINDOW, YPOS), std::make_pair(0, glm::vec2(6, 14))},
    {std::make_pair(TEAK_WINDOW, YNEG), std::make_pair(0, glm::vec2(6, 14))},

    {std::make_pair(CHERRY_WINDOW, XPOS), std::make_pair(0, glm::vec2(7, 14))},
    {std::make_pair(CHERRY_WINDOW, XNEG), std::make_pair(0, glm::vec2(7, 14))},
    {std::make_pair(CHERRY_WINDOW, ZPOS), std::make_pair(0, glm::vec2(7, 14))},
    {std::make_pair(CHERRY_WINDOW, ZNEG), std::make_pair(0, glm::vec2(7, 14))},
    {std::make_pair(CHERRY_WINDOW, YPOS), std::make_pair(0, glm::vec2(7, 14))},
    {std::make_pair(CHERRY_WINDOW, YNEG), std::make_pair(0, glm::vec2(7, 14))},

    {std::make_pair(MAPLE_WINDOW, XPOS), std::make_pair(0, glm::vec2(8, 14))},
    {std::make_pair(MAPLE_WINDOW, XNEG), std::make_pair(0, glm::vec2(8, 14))},
    {std::make_pair(MAPLE_WINDOW, ZPOS), std::make_pair(0, glm::vec2(8, 14))},
    {std::make_pair(MAPLE_WINDOW, ZNEG), std::make_pair(0, glm::vec2(8, 14))},
    {std::make_pair(MAPLE_WINDOW, YPOS), std::make_pair(0, glm::vec2(8, 14))},
    {std::make_pair(MAPLE_WINDOW, YNEG), std::make_pair(0, glm::vec2(8, 14))},

    {std::make_pair(PINE_WINDOW, XPOS), std::make_pair(0, glm::vec2(9, 14))},
    {std::make_pair(PINE_WINDOW, XNEG), std::make_pair(0, glm::vec2(9, 14))},
    {std::make_pair(PINE_WINDOW, ZPOS), std::make_pair(0, glm::vec2(9, 14))},
    {std::make_pair(PINE_WINDOW, ZNEG), std::make_pair(0, glm::vec2(9, 14))},
    {std::make_pair(PINE_WINDOW, YPOS), std::make_pair(0, glm::vec2(9, 14))},
    {std::make_pair(PINE_WINDOW, YNEG), std::make_pair(0, glm::vec2(9, 14))},

    {std::make_pair(WISTERIA_WINDOW, XPOS), std::make_pair(0, glm::vec2(10, 14))},
    {std::make_pair(WISTERIA_WINDOW, XNEG), std::make_pair(0, glm::vec2(10, 14))},
    {std::make_pair(WISTERIA_WINDOW, ZPOS), std::make_pair(0, glm::vec2(10, 14))},
    {std::make_pair(WISTERIA_WINDOW, ZNEG), std::make_pair(0, glm::vec2(10, 14))},
    {std::make_pair(WISTERIA_WINDOW, YPOS), std::make_pair(0, glm::vec2(10, 14))},
    {std::make_pair(WISTERIA_WINDOW, YNEG), std::make_pair(0, glm::vec2(10, 14))},

    {std::make_pair(RED_PAINTED_WOOD, XPOS), std::make_pair(0, glm::vec2(4, 14))},
    {std::make_pair(RED_PAINTED_WOOD, XNEG), std::make_pair(0, glm::vec2(4, 14))},
    {std::make_pair(RED_PAINTED_WOOD, ZPOS), std::make_pair(0, glm::vec2(4, 14))},
    {std::make_pair(RED_PAINTED_WOOD, ZNEG), std::make_pair(0, glm::vec2(4, 14))},
    {std::make_pair(RED_PAINTED_WOOD, YPOS), std::make_pair(0, glm::vec2(4, 14))},
    {std::make_pair(RED_PAINTED_WOOD, YNEG), std::make_pair(0, glm::vec2(4, 14))},

    {std::make_pair(BLACK_PAINTED_WOOD, XPOS), std::make_pair(0, glm::vec2(4, 15))},
    {std::make_pair(BLACK_PAINTED_WOOD, XNEG), std::make_pair(0, glm::vec2(4, 15))},
    {std::make_pair(BLACK_PAINTED_WOOD, ZPOS), std::make_pair(0, glm::vec2(4, 15))},
    {std::make_pair(BLACK_PAINTED_WOOD, ZNEG), std::make_pair(0, glm::vec2(4, 15))},
    {std::make_pair(BLACK_PAINTED_WOOD, YPOS), std::make_pair(0, glm::vec2(4, 15))},
    {std::make_pair(BLACK_PAINTED_WOOD, YNEG), std::make_pair(0, glm::vec2(4, 15))},

    {std::make_pair(PLASTER, XPOS), std::make_pair(0, glm::vec2(11, 12))},
    {std::make_pair(PLASTER, XNEG), std::make_pair(0, glm::vec2(11, 12))},
    {std::make_pair(PLASTER, ZPOS), std::make_pair(0, glm::vec2(11, 12))},
    {std::make_pair(PLASTER, ZNEG), std::make_pair(0, glm::vec2(11, 12))},
    {std::make_pair(PLASTER, YPOS), std::make_pair(0, glm::vec2(11, 12))},
    {std::make_pair(PLASTER, YNEG), std::make_pair(0, glm::vec2(11, 12))},

    {std::make_pair(ROOF_TILES_1, XPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_1, XNEG), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_1, ZPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_1, ZNEG), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_1, YPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_1, YNEG), std::make_pair(0, glm::vec2(11, 15))},

    {std::make_pair(ROOF_TILES_2, XPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_2, XNEG), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_2, ZPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_2, ZNEG), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_2, YPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_2, YNEG), std::make_pair(0, glm::vec2(11, 15))},

    {std::make_pair(ROOF_TILES_3, XPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_3, XNEG), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_3, ZPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_3, ZNEG), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_3, YPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_3, YNEG), std::make_pair(0, glm::vec2(11, 15))},

    {std::make_pair(ROOF_TILES_4, XPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_4, XNEG), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_4, ZPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_4, ZNEG), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_4, YPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES_4, YNEG), std::make_pair(0, glm::vec2(11, 15))},

    {std::make_pair(STRAW_1, XPOS), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_1, XNEG), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_1, ZPOS), std::make_pair(0, glm::vec2(11, 14))},
    {std::make_pair(STRAW_1, ZNEG), std::make_pair(0, glm::vec2(11, 14))},
    {std::make_pair(STRAW_1, YPOS), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_1, YNEG), std::make_pair(0, glm::vec2(11, 13))},

    {std::make_pair(STRAW_2, XPOS), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_2, XNEG), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_2, ZPOS), std::make_pair(0, glm::vec2(11, 14))},
    {std::make_pair(STRAW_2, ZNEG), std::make_pair(0, glm::vec2(11, 14))},
    {std::make_pair(STRAW_2, YPOS), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_2, YNEG), std::make_pair(0, glm::vec2(11, 13))},

    {std::make_pair(STRAW_3, XPOS), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_3, XNEG), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_3, ZPOS), std::make_pair(0, glm::vec2(11, 14))},
    {std::make_pair(STRAW_3, ZNEG), std::make_pair(0, glm::vec2(11, 14))},
    {std::make_pair(STRAW_3, YPOS), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_3, YNEG), std::make_pair(0, glm::vec2(11, 13))},

    {std::make_pair(STRAW_4, XPOS), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_4, XNEG), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_4, ZPOS), std::make_pair(0, glm::vec2(11, 14))},
    {std::make_pair(STRAW_4, ZNEG), std::make_pair(0, glm::vec2(11, 14))},
    {std::make_pair(STRAW_4, YPOS), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW_4, YNEG), std::make_pair(0, glm::vec2(11, 13))},

    {std::make_pair(LILY_PAD, YPOS), std::make_pair(0, glm::vec2(0, 9))},
    {std::make_pair(LILY_PAD, YNEG), std::make_pair(0, glm::vec2(4, 9))},

    {std::make_pair(LOTUS_1, XPOS_ZPOS), std::make_pair(0, glm::vec2(1, 9))},
    {std::make_pair(LOTUS_1, XNEG_ZNEG), std::make_pair(4, glm::vec2(1, 9))},
    {std::make_pair(LOTUS_1, XPOS_ZNEG), std::make_pair(0, glm::vec2(1, 9))},
    {std::make_pair(LOTUS_1, XNEG_ZPOS), std::make_pair(4, glm::vec2(1, 9))},

    {std::make_pair(LOTUS_2, XPOS_ZPOS), std::make_pair(0, glm::vec2(1, 10))},
    {std::make_pair(LOTUS_2, XNEG_ZNEG), std::make_pair(4, glm::vec2(1, 10))},
    {std::make_pair(LOTUS_2, XPOS_ZNEG), std::make_pair(0, glm::vec2(1, 10))},
    {std::make_pair(LOTUS_2, XNEG_ZPOS), std::make_pair(4, glm::vec2(1, 10))},

    {std::make_pair(LOTUS_3, XPOS_ZPOS), std::make_pair(0, glm::vec2(1, 11))},
    {std::make_pair(LOTUS_3, XNEG_ZNEG), std::make_pair(4, glm::vec2(1, 11))},
    {std::make_pair(LOTUS_3, XPOS_ZNEG), std::make_pair(0, glm::vec2(1, 11))},
    {std::make_pair(LOTUS_3, XNEG_ZPOS), std::make_pair(4, glm::vec2(1, 11))},

    {std::make_pair(TILLED_DIRT, XPOS), std::make_pair(0, glm::vec2(0, 8))},
    {std::make_pair(TILLED_DIRT, XNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(TILLED_DIRT, ZPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(TILLED_DIRT, ZNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(TILLED_DIRT, YPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(TILLED_DIRT, YNEG), std::make_pair(0, glm::vec2(0, 13))},

    {std::make_pair(PATH, XPOS), std::make_pair(0, glm::vec2(1, 8))},
    {std::make_pair(PATH, XNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(PATH, ZPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(PATH, ZNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(PATH, YPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(PATH, YNEG), std::make_pair(0, glm::vec2(0, 13))},

    {std::make_pair(WHEAT, XPOS), std::make_pair(0, glm::vec2(0, 7))},
    {std::make_pair(WHEAT, XNEG), std::make_pair(4, glm::vec2(0, 7))},
    {std::make_pair(WHEAT, ZPOS), std::make_pair(0, glm::vec2(0, 7))},
    {std::make_pair(WHEAT, ZNEG), std::make_pair(4, glm::vec2(0, 7))},

    {std::make_pair(RICE, XPOS_ZPOS), std::make_pair(0, glm::vec2(0, 6))},
    {std::make_pair(RICE, XPOS_ZPOS), std::make_pair(4, glm::vec2(0, 6))},
    {std::make_pair(RICE, XPOS_ZNEG), std::make_pair(0, glm::vec2(0, 6))},
    {std::make_pair(RICE, XNEG_ZPOS), std::make_pair(4, glm::vec2(0, 6))},

    {std::make_pair(BAMBOO_1, XPOS), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_1, XNEG), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_1, ZPOS), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_1, ZNEG), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_1, YPOS), std::make_pair(0, glm::vec2(12, 12))},
    {std::make_pair(BAMBOO_1, YNEG), std::make_pair(0, glm::vec2(12, 12))},

    {std::make_pair(BAMBOO_2, XPOS), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_2, XNEG), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_2, ZPOS), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_2, ZNEG), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_2, YPOS), std::make_pair(0, glm::vec2(12, 12))},
    {std::make_pair(BAMBOO_2, YNEG), std::make_pair(0, glm::vec2(12, 12))},
    {std::make_pair(BAMBOO_2, XPOS_ZPOS), std::make_pair(0, glm::vec2(12, 14))},
    {std::make_pair(BAMBOO_2, XPOS_ZPOS), std::make_pair(4, glm::vec2(12, 14))},
    {std::make_pair(BAMBOO_2, XPOS_ZNEG), std::make_pair(0, glm::vec2(12, 14))},
    {std::make_pair(BAMBOO_2, XNEG_ZPOS), std::make_pair(4, glm::vec2(12, 14))},

    {std::make_pair(BAMBOO_3, XPOS), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_3, XNEG), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_3, ZPOS), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_3, ZNEG), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_3, YPOS), std::make_pair(0, glm::vec2(12, 12))},
    {std::make_pair(BAMBOO_3, YNEG), std::make_pair(0, glm::vec2(12, 12))},
    {std::make_pair(BAMBOO_3, XPOS_ZPOS), std::make_pair(0, glm::vec2(12, 15))},
    {std::make_pair(BAMBOO_3, XPOS_ZPOS), std::make_pair(4, glm::vec2(12, 15))},
    {std::make_pair(BAMBOO_3, XPOS_ZNEG), std::make_pair(0, glm::vec2(12, 15))},
    {std::make_pair(BAMBOO_3, XNEG_ZPOS), std::make_pair(4, glm::vec2(12, 15))},

    {std::make_pair(TATAMI, XPOS), std::make_pair(0, glm::vec2(11, 10))},
    {std::make_pair(TATAMI, XNEG), std::make_pair(0, glm::vec2(11, 10))},
    {std::make_pair(TATAMI, ZPOS), std::make_pair(0, glm::vec2(11, 9))},
    {std::make_pair(TATAMI, ZNEG), std::make_pair(0, glm::vec2(11, 9))},
    {std::make_pair(TATAMI, YPOS), std::make_pair(0, glm::vec2(11, 11))},
    {std::make_pair(TATAMI, YNEG), std::make_pair(0, glm::vec2(11, 11))},

    {std::make_pair(PAPER_LANTERN, XPOS), std::make_pair(0, glm::vec2(13, 15))},
    {std::make_pair(PAPER_LANTERN, XNEG), std::make_pair(0, glm::vec2(13, 15))},
    {std::make_pair(PAPER_LANTERN, ZPOS), std::make_pair(0, glm::vec2(13, 15))},
    {std::make_pair(PAPER_LANTERN, ZNEG), std::make_pair(0, glm::vec2(13, 15))},
    {std::make_pair(PAPER_LANTERN, YPOS), std::make_pair(0, glm::vec2(13, 14))},
    {std::make_pair(PAPER_LANTERN, YNEG), std::make_pair(0, glm::vec2(13, 14))},

    {std::make_pair(WOOD_LANTERN, XPOS), std::make_pair(0, glm::vec2(13, 13))},
    {std::make_pair(WOOD_LANTERN, XNEG), std::make_pair(0, glm::vec2(13, 13))},
    {std::make_pair(WOOD_LANTERN, ZPOS), std::make_pair(0, glm::vec2(13, 13))},
    {std::make_pair(WOOD_LANTERN, ZNEG), std::make_pair(0, glm::vec2(13, 13))},
    {std::make_pair(WOOD_LANTERN, YPOS), std::make_pair(0, glm::vec2(13, 12))},
    {std::make_pair(WOOD_LANTERN, YNEG), std::make_pair(0, glm::vec2(13, 12))},

    {std::make_pair(BONSAI_TREE, XPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(BONSAI_TREE, XNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(BONSAI_TREE, ZPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(BONSAI_TREE, ZNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(BONSAI_TREE, YPOS), std::make_pair(0, glm::vec2(0, 2))},
    {std::make_pair(BONSAI_TREE, YNEG), std::make_pair(0, glm::vec2(0, 3))},
    {std::make_pair(BONSAI_TREE, XPOS_ZPOS), std::make_pair(0, glm::vec2(0, 5))},
    {std::make_pair(BONSAI_TREE, XPOS_ZPOS), std::make_pair(4, glm::vec2(0, 5))},
    {std::make_pair(BONSAI_TREE, XPOS_ZNEG), std::make_pair(0, glm::vec2(0, 5))},
    {std::make_pair(BONSAI_TREE, XNEG_ZPOS), std::make_pair(4, glm::vec2(0, 5))},

    {std::make_pair(MAGNOLIA_IKEBANA, XPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(MAGNOLIA_IKEBANA, XNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(MAGNOLIA_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(MAGNOLIA_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(MAGNOLIA_IKEBANA, YPOS), std::make_pair(0, glm::vec2(0, 2))},
    {std::make_pair(MAGNOLIA_IKEBANA, YNEG), std::make_pair(0, glm::vec2(0, 3))},
    {std::make_pair(MAGNOLIA_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(0, 4))},
    {std::make_pair(MAGNOLIA_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(0, 4))},
    {std::make_pair(MAGNOLIA_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(0, 4))},
    {std::make_pair(MAGNOLIA_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(0, 4))},

    {std::make_pair(LOTUS_IKEBANA, XPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(LOTUS_IKEBANA, XNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(LOTUS_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(LOTUS_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(LOTUS_IKEBANA, YPOS), std::make_pair(0, glm::vec2(0, 2))},
    {std::make_pair(LOTUS_IKEBANA, YNEG), std::make_pair(0, glm::vec2(0, 3))},
    {std::make_pair(LOTUS_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 5))},
    {std::make_pair(LOTUS_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(3, 5))},
    {std::make_pair(LOTUS_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(3, 5))},
    {std::make_pair(LOTUS_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(3, 5))},

    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XPOS), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XNEG), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, YPOS), std::make_pair(0, glm::vec2(1, 2))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, YNEG), std::make_pair(0, glm::vec2(1, 3))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(4, 4))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(4, 4))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(4, 4))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(4, 4))},

    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XPOS), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XNEG), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, YPOS), std::make_pair(0, glm::vec2(1, 2))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, YNEG), std::make_pair(0, glm::vec2(1, 3))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(5, 5))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(5, 5))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(5, 5))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(5, 5))},

    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, YPOS), std::make_pair(0, glm::vec2(2, 2))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, YNEG), std::make_pair(0, glm::vec2(2, 3))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(1, 5))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(1, 5))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(1, 5))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(1, 5))},

    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, YPOS), std::make_pair(0, glm::vec2(2, 2))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, YNEG), std::make_pair(0, glm::vec2(2, 3))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 4))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(3, 4))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(3, 4))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(3, 4))},

    {std::make_pair(TULIP_IKEBANA, XPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(TULIP_IKEBANA, XNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(TULIP_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(TULIP_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(TULIP_IKEBANA, YPOS), std::make_pair(0, glm::vec2(2, 2))},
    {std::make_pair(TULIP_IKEBANA, YNEG), std::make_pair(0, glm::vec2(2, 3))},
    {std::make_pair(TULIP_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(6, 5))},
    {std::make_pair(TULIP_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(6, 5))},
    {std::make_pair(TULIP_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(6, 5))},
    {std::make_pair(TULIP_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(6, 5))},

    {std::make_pair(DAFFODIL_IKEBANA, XPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(DAFFODIL_IKEBANA, XNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(DAFFODIL_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(DAFFODIL_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(DAFFODIL_IKEBANA, YPOS), std::make_pair(0, glm::vec2(2, 2))},
    {std::make_pair(DAFFODIL_IKEBANA, YNEG), std::make_pair(0, glm::vec2(2, 3))},
    {std::make_pair(DAFFODIL_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(6, 4))},
    {std::make_pair(DAFFODIL_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(6, 4))},
    {std::make_pair(DAFFODIL_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(6, 4))},
    {std::make_pair(DAFFODIL_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(6, 4))},

    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, YPOS), std::make_pair(0, glm::vec2(3, 2))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, YNEG), std::make_pair(0, glm::vec2(3, 3))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(2, 5))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(2, 5))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(2, 5))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(2, 5))},

    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, YPOS), std::make_pair(0, glm::vec2(3, 2))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, YNEG), std::make_pair(0, glm::vec2(3, 3))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(1, 4))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(1, 4))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(1, 4))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(1, 4))},

    {std::make_pair(POPPY_IKEBANA, XPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(POPPY_IKEBANA, XNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(POPPY_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(POPPY_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(POPPY_IKEBANA, YPOS), std::make_pair(0, glm::vec2(3, 2))},
    {std::make_pair(POPPY_IKEBANA, YNEG), std::make_pair(0, glm::vec2(3, 3))},
    {std::make_pair(POPPY_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(2, 4))},
    {std::make_pair(POPPY_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(2, 4))},
    {std::make_pair(POPPY_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(2, 4))},
    {std::make_pair(POPPY_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(2, 4))},

    {std::make_pair(MAPLE_IKEBANA, XPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAPLE_IKEBANA, XNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAPLE_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAPLE_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAPLE_IKEBANA, YPOS), std::make_pair(0, glm::vec2(3, 2))},
    {std::make_pair(MAPLE_IKEBANA, YNEG), std::make_pair(0, glm::vec2(3, 3))},
    {std::make_pair(MAPLE_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(4, 5))},
    {std::make_pair(MAPLE_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(4, 5))},
    {std::make_pair(MAPLE_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(4, 5))},
    {std::make_pair(MAPLE_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(4, 5))},

    {std::make_pair(ONCIDIUM_IKEBANA, XPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(ONCIDIUM_IKEBANA, XNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(ONCIDIUM_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(ONCIDIUM_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(ONCIDIUM_IKEBANA, YPOS), std::make_pair(0, glm::vec2(3, 2))},
    {std::make_pair(ONCIDIUM_IKEBANA, YNEG), std::make_pair(0, glm::vec2(3, 3))},
    {std::make_pair(ONCIDIUM_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(5, 4))},
    {std::make_pair(ONCIDIUM_IKEBANA, XPOS_ZPOS), std::make_pair(4, glm::vec2(5, 4))},
    {std::make_pair(ONCIDIUM_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(5, 4))},
    {std::make_pair(ONCIDIUM_IKEBANA, XNEG_ZPOS), std::make_pair(4, glm::vec2(5, 4))},

    {std::make_pair(GHOST_LILY, XPOS_ZPOS), std::make_pair(0, glm::vec2(2, 9))},
    {std::make_pair(GHOST_LILY, XPOS_ZPOS), std::make_pair(4, glm::vec2(2, 9))},
    {std::make_pair(GHOST_LILY, XPOS_ZNEG), std::make_pair(0, glm::vec2(2, 9))},
    {std::make_pair(GHOST_LILY, XNEG_ZPOS), std::make_pair(4, glm::vec2(2, 9))},

    {std::make_pair(GHOST_WEED, XPOS_ZPOS), std::make_pair(0, glm::vec2(2, 10))},
    {std::make_pair(GHOST_WEED, XPOS_ZPOS), std::make_pair(4, glm::vec2(2, 10))},
    {std::make_pair(GHOST_WEED, XPOS_ZNEG), std::make_pair(0, glm::vec2(2, 10))},
    {std::make_pair(GHOST_WEED, XNEG_ZPOS), std::make_pair(4, glm::vec2(2, 10))},

    {std::make_pair(CORAL, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 9))},
    {std::make_pair(CORAL, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 9))},
    {std::make_pair(CORAL, XPOS_ZNEG), std::make_pair(0, glm::vec2(3, 9))},
    {std::make_pair(CORAL, XNEG_ZPOS), std::make_pair(0, glm::vec2(3, 9))},

    {std::make_pair(KELP, XPOS_ZPOS), std::make_pair(0, glm::vec2(4, 9))},
    {std::make_pair(KELP, XPOS_ZPOS), std::make_pair(0, glm::vec2(4, 9))},
    {std::make_pair(KELP, XPOS_ZNEG), std::make_pair(0, glm::vec2(4, 9))},
    {std::make_pair(KELP, XNEG_ZPOS), std::make_pair(0, glm::vec2(4, 9))},

    {std::make_pair(SEA_GRASS, XPOS_ZPOS), std::make_pair(0, glm::vec2(5, 9))},
    {std::make_pair(SEA_GRASS, XPOS_ZPOS), std::make_pair(0, glm::vec2(5, 9))},
    {std::make_pair(SEA_GRASS, XPOS_ZNEG), std::make_pair(0, glm::vec2(5, 9))},
    {std::make_pair(SEA_GRASS, XNEG_ZPOS), std::make_pair(0, glm::vec2(5, 9))}

//    {PAINTING, glm::vec2(13, 5)},
//    {PAINTING_T, glm::vec2(12, 11)},
//    {PAINTING_B, glm::vec2(12, 10)},
//    {PAINTING_L, glm::vec2(14, 5)},
//    {PAINTING_R, glm::vec2(15, 5)},

};
//const static std::map<BlockType, glm::vec3> btToBlockIdx;

struct DirectionVector {
    Direction dir;
    glm::ivec3 vec;

    DirectionVector(Direction d, glm::ivec3 v)
        : dir(d), vec(v)
    {}
};

const static std::vector<DirectionVector> directionIter = {
    DirectionVector(XPOS, glm::ivec3(1, 0, 0)),
    DirectionVector(XNEG, glm::ivec3(-1, 0, 0)),
    DirectionVector(YPOS, glm::ivec3(0, 1, 0)),
    DirectionVector(YNEG, glm::ivec3(0, -1, 0)),
    DirectionVector(ZPOS, glm::ivec3(0, 0, 1)),
    DirectionVector(ZNEG, glm::ivec3(0, 0, -1)),
};

const static std::vector<DirectionVector> planeDirIter = {
    DirectionVector(YPOS, glm::ivec3(0, 1, 0)),
    DirectionVector(YNEG, glm::ivec3(0, -1, 0)),
};

const static std::vector<DirectionVector> xzDirIter = {
    DirectionVector(XPOS, glm::ivec3(1, 0, 0)),
    DirectionVector(XNEG, glm::ivec3(-1, 0, 0)),
    DirectionVector(ZPOS, glm::ivec3(0, 0, 1)),
    DirectionVector(ZNEG, glm::ivec3(0, 0, -1)),
};

const static std::vector<DirectionVector> cross2DirIter = {
    DirectionVector(XPOS, glm::ivec3(1, 0, 0)),
    DirectionVector(XNEG, glm::ivec3(-1, 0, 0)),
    DirectionVector(YPOS, glm::ivec3(0, 1, 0)),
    DirectionVector(YNEG, glm::ivec3(0, -1, 0)),
    DirectionVector(ZPOS, glm::ivec3(0, 0, 1)),
    DirectionVector(ZNEG, glm::ivec3(0, 0, -1)),
};

const static std::vector<DirectionVector> cross4DirIter = {
    DirectionVector(XPOS, glm::ivec3(1, 0, 0)),
    DirectionVector(XNEG, glm::ivec3(-1, 0, 0)),
    DirectionVector(YPOS, glm::ivec3(0, 1, 0)),
    DirectionVector(YNEG, glm::ivec3(0, -1, 0)),
    DirectionVector(ZPOS, glm::ivec3(0, 0, 1)),
    DirectionVector(ZNEG, glm::ivec3(0, 0, -1)),
};

const static std::unordered_set<BlockType, EnumHash> hPlane = {
    LILY_PAD, LOTUS_1, LOTUS_2
};

const static std::unordered_set<BlockType, EnumHash> cross2 = {
    WATER,
    LOTUS_1, LOTUS_2, RICE,
    GHOST_LILY, GHOST_WEED, CORAL, KELP, SEA_GRASS,
    BAMBOO_1, BAMBOO_2, BAMBOO_3,
    BONSAI_TREE, MAGNOLIA_IKEBANA, LOTUS_IKEBANA, GREEN_HYDRANGEA_IKEBANA, CHRYSANTHEMUM_IKEBANA,
    CHERRY_BLOSSOM_IKEBANA, BLUE_HYDRANGEA_IKEBANA, TULIP_IKEBANA, DAFFODIL_IKEBANA,
    PLUM_BLOSSOM_IKEBANA, MAGNOLIA_BUD_IKEBANA, POPPY_IKEBANA, MAPLE_IKEBANA, ONCIDIUM_IKEBANA
};

const static std::unordered_set<BlockType, EnumHash> cross4 = {
    TALL_GRASS, WHEAT
};

const static std::unordered_set<BlockType, EnumHash> partialX = {
    CEDAR_WINDOW, TEAK_WINDOW, CHERRY_WINDOW, MAPLE_WINDOW, PINE_WINDOW, WISTERIA_WINDOW,
    PAPER_LANTERN, WOOD_LANTERN,
    PAINTING, PAINTING_T, PAINTING_B, PAINTING_L, PAINTING_R,
    BAMBOO_1, BAMBOO_2, BAMBOO_3,
    BONSAI_TREE, MAGNOLIA_IKEBANA, LOTUS_IKEBANA, GREEN_HYDRANGEA_IKEBANA, CHRYSANTHEMUM_IKEBANA,
    CHERRY_BLOSSOM_IKEBANA, BLUE_HYDRANGEA_IKEBANA, TULIP_IKEBANA, DAFFODIL_IKEBANA,
    PLUM_BLOSSOM_IKEBANA, MAGNOLIA_BUD_IKEBANA, POPPY_IKEBANA, MAPLE_IKEBANA, ONCIDIUM_IKEBANA
};

const static std::unordered_set<BlockType, EnumHash> partialY = {
    LAVA, SNOW_1, SNOW_2, SNOW_3, SNOW_4, SNOW_5, SNOW_6, SNOW_7,
    ROOF_TILES_1, ROOF_TILES_2, ROOF_TILES_3, ROOF_TILES_4, STRAW_1, STRAW_2, STRAW_3, STRAW_4,
    TILLED_DIRT, PATH, TATAMI,
    PAPER_LANTERN,
    BONSAI_TREE, MAGNOLIA_IKEBANA, LOTUS_IKEBANA, GREEN_HYDRANGEA_IKEBANA, CHRYSANTHEMUM_IKEBANA,
    CHERRY_BLOSSOM_IKEBANA, BLUE_HYDRANGEA_IKEBANA, TULIP_IKEBANA, DAFFODIL_IKEBANA,
    PLUM_BLOSSOM_IKEBANA, MAGNOLIA_BUD_IKEBANA, POPPY_IKEBANA, MAPLE_IKEBANA, ONCIDIUM_IKEBANA
};

const static std::unordered_set<BlockType, EnumHash> partialZ = {
    PAPER_LANTERN, WOOD_LANTERN,
    BAMBOO_1, BAMBOO_2, BAMBOO_3,
    BONSAI_TREE, MAGNOLIA_IKEBANA, LOTUS_IKEBANA, GREEN_HYDRANGEA_IKEBANA, CHRYSANTHEMUM_IKEBANA,
    CHERRY_BLOSSOM_IKEBANA, BLUE_HYDRANGEA_IKEBANA, TULIP_IKEBANA, DAFFODIL_IKEBANA,
    PLUM_BLOSSOM_IKEBANA, MAGNOLIA_BUD_IKEBANA, POPPY_IKEBANA, MAPLE_IKEBANA, ONCIDIUM_IKEBANA
};

struct Vertex {
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec4 color;
    glm::vec4 uvCoords;
    glm::vec4 blockType; // block idx, biome

    Vertex(glm::vec4 p, glm::ivec3 n, BlockType b, Direction d, glm::vec2 uv, int biome) {
        position = p;
        normal = glm::vec4(n, 1);
        if (btToUV.find(std::make_pair(b, d)) != btToUV.end()) {
            uvCoords = glm::vec4(uv + btToUV.at(std::make_pair(b, d)).second, 0, 0);
            uvCoords /= 16.f;
            blockType = glm::vec4();
        } else {
            color = glm::vec4(1.f, 0.f, 1.f, 1.f);
        }
    }
};

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

// TODO have Chunk inherit from Drawable
class Chunk : public Drawable {
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    std::array<int, 65536> m_biomes;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;

    static bool isInBounds(glm::ivec3);
    BlockType getAdjBlockType(Direction, glm::ivec3);

    // coords given in block space
    static void createFaceVBOData(std::vector<Vertex>&, float, float, float, DirectionVector, BlockType, int);

    void redistributeVertexData(std::vector<glm::vec4>, std::vector<GLuint>);

public:
    Chunk(OpenGLContext* context);
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    int getBiomeAt(unsigned int x, unsigned int y, unsigned int z) const;
    int getBiomeAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void setBiomeAt(unsigned int x, unsigned int y, unsigned int z, int b);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    boolean isHPlane(BlockType);
    boolean isCross2(BlockType);
    boolean isCross4(BlockType);
    boolean isPartialX(BlockType);
    boolean isPartialY(BlockType);
    boolean isPartialZ(BlockType);
    boolean isFullCube(BlockType);

    void createVBOdata() override;
    GLenum drawMode() override {
        return GL_TRIANGLES;
    }


};
