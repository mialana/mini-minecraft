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
    GRASS, DIRT, STONE, COBBLESTONE, MOSS_STONE, SAND,
    TALL_GRASS, WATER, LAVA, BEDROCK, SNOW_1, SNOW_2, SNOW_3, SNOW_4, SNOW_5, SNOW_6, SNOW_7, SNOW_8, ICE,
    RED_PAINTED_WOOD, BLACK_PAINTED_WOOD, PLASTER,
    ROOF_TILES_1, ROOF_TILES_2, ROOF_TILES, STRAW_1, STRAW_2, STRAW,
    CEDAR_WOOD_X, TEAK_WOOD_X, CHERRY_WOOD_X, MAPLE_WOOD_X, PINE_WOOD_X, WISTERIA_WOOD_X,
    CEDAR_WOOD_Y, TEAK_WOOD_Y, CHERRY_WOOD_Y, MAPLE_WOOD_Y, PINE_WOOD_Y, WISTERIA_WOOD_Y,
    CEDAR_WOOD_Z, TEAK_WOOD_Z, CHERRY_WOOD_Z, MAPLE_WOOD_Z, PINE_WOOD_Z, WISTERIA_WOOD_Z,
    CEDAR_LEAVES, TEAK_LEAVES, CHERRY_BLOSSOMS_1, CHERRY_BLOSSOMS_2, CHERRY_BLOSSOMS_3, CHERRY_BLOSSOMS_4,
    MAPLE_LEAVES_1, MAPLE_LEAVES_2, MAPLE_LEAVES_3,
    PINE_LEAVES, WISTERIA_BLOSSOMS_1, WISTERIA_BLOSSOMS_2, WISTERIA_BLOSSOMS_3,
    CEDAR_PLANKS, TEAK_PLANKS, CHERRY_PLANKS, MAPLE_PLANKS, PINE_PLANKS, WISTERIA_PLANKS,
    CEDAR_PLANKS_1, TEAK_PLANKS_1, CHERRY_PLANKS_1, MAPLE_PLANKS_1, PINE_PLANKS_1, WISTERIA_PLANKS_1,
    CEDAR_PLANKS_2, TEAK_PLANKS_2, CHERRY_PLANKS_2, MAPLE_PLANKS_2, PINE_PLANKS_2, WISTERIA_PLANKS_2,
    CEDAR_WINDOW_X, TEAK_WINDOW_X, CHERRY_WINDOW_X, MAPLE_WINDOW_X, PINE_WINDOW_X, WISTERIA_WINDOW_X,
    CEDAR_WINDOW_Z, TEAK_WINDOW_Z, CHERRY_WINDOW_Z, MAPLE_WINDOW_Z, PINE_WINDOW_Z, WISTERIA_WINDOW_Z,
    LILY_PAD, LOTUS_1, LOTUS_2, TILLED_DIRT, IRRIGATED_SOIL, PATH,
    WHEAT_1, WHEAT_2, WHEAT_3, WHEAT_4, WHEAT_5, WHEAT_6, WHEAT_7, WHEAT_8,
    RICE_1, RICE_2, RICE_3, RICE_4, RICE_5, RICE_6, RICE_01, RICE_02,
    BAMBOO_1, BAMBOO_2, BAMBOO_3,
    TATAMI_XL, TATAMI_XR, TATAMI_ZT, TATAMI_ZB, PAPER_LANTERN, WOOD_LANTERN,
    CLOTH_1, CLOTH_2, CLOTH_3, CLOTH_4, CLOTH_5, CLOTH_6, CLOTH_7, CLOTH_8,
    PAINTING_1_XP, PAINTING_2_XP, PAINTING_3_XP, PAINTING_4_XP, PAINTING_5_XP, PAINTING_6L_XP, PAINTING_6R_XP, PAINTING_7T_XP, PAINTING_7B_XP,
    PAINTING_1_XN, PAINTING_2_XN, PAINTING_3_XN, PAINTING_4_XN, PAINTING_5_XN, PAINTING_6L_XN, PAINTING_6R_XN, PAINTING_7T_XN, PAINTING_7B_XN,
    PAINTING_1_ZP, PAINTING_2_ZP, PAINTING_3_ZP, PAINTING_4_ZP, PAINTING_5_ZP, PAINTING_6L_ZP, PAINTING_6R_ZP, PAINTING_7T_ZP, PAINTING_7B_ZP,
    PAINTING_1_ZN, PAINTING_2_ZN, PAINTING_3_ZN, PAINTING_4_ZN, PAINTING_5_ZN, PAINTING_6L_ZN, PAINTING_6R_ZN, PAINTING_7T_ZN, PAINTING_7B_ZN,
    BONSAI_TREE, MAGNOLIA_IKEBANA, LOTUS_IKEBANA,
    GREEN_HYDRANGEA_IKEBANA, CHRYSANTHEMUM_IKEBANA,
    CHERRY_BLOSSOM_IKEBANA, BLUE_HYDRANGEA_IKEBANA, TULIP_IKEBANA, DAFFODIL_IKEBANA,
    PLUM_BLOSSOM_IKEBANA, MAGNOLIA_BUD_IKEBANA, POPPY_IKEBANA, MAPLE_IKEBANA, ONCIDIUM_IKEBANA,
    GHOST_LILY, GHOST_WEED,
    CORAL_1, CORAL_2, CORAL_3, CORAL_4,
    KELP_1, KELP_2, SEA_GRASS, EMPTY
};

enum BiomeEnum : unsigned char {
    MOUNTAINS, HILLS, FOREST, ISLANDS, CAVES
};

// The six cardinal directions in 3D space + diagonals (rotated 45 degrees)
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG, XPOS_ZPOS, XPOS_ZNEG, XNEG_ZNEG, XNEG_ZPOS
};

typedef std::pair<BlockType, Direction> faceDef;

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

struct PairEnumHash {
    size_t operator()(const faceDef fd) const {
        return static_cast<size_t>(fd.first)
            * static_cast<size_t>(EMPTY)
            + static_cast<size_t>(fd.second);
    }
};

const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

// maps blocktype and direction to texture flag and uv coord
const static std::unordered_map<std::pair<BlockType, Direction>, std::pair<int, glm::vec2>, PairEnumHash> btToUV {
    {std::make_pair(GRASS, XPOS), std::make_pair(1, glm::vec2(0, 11))},
    {std::make_pair(GRASS, XNEG), std::make_pair(1, glm::vec2(0, 11))},
    {std::make_pair(GRASS, ZPOS), std::make_pair(1, glm::vec2(0, 11))},
    {std::make_pair(GRASS, ZNEG), std::make_pair(1, glm::vec2(0, 11))},
    {std::make_pair(GRASS, YPOS), std::make_pair(1, glm::vec2(0, 11))},
    {std::make_pair(GRASS, YNEG), std::make_pair(1, glm::vec2(0, 11))},

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

    {std::make_pair(TALL_GRASS, XPOS_ZPOS), std::make_pair(1, glm::vec2(0, 10))},
    {std::make_pair(TALL_GRASS, XPOS_ZNEG), std::make_pair(1, glm::vec2(0, 10))},
    {std::make_pair(TALL_GRASS, XNEG_ZPOS), std::make_pair(1, glm::vec2(0, 10))},
    {std::make_pair(TALL_GRASS, XNEG_ZNEG), std::make_pair(1, glm::vec2(0, 10))},

    {std::make_pair(WATER, XPOS), std::make_pair(2, glm::vec2(13, 3))},
    {std::make_pair(WATER, XNEG), std::make_pair(2, glm::vec2(13, 3))},
    {std::make_pair(WATER, ZPOS), std::make_pair(2, glm::vec2(13, 3))},
    {std::make_pair(WATER, ZNEG), std::make_pair(2, glm::vec2(13, 3))},
    {std::make_pair(WATER, YPOS), std::make_pair(2, glm::vec2(13, 3))},
    {std::make_pair(WATER, YNEG), std::make_pair(2, glm::vec2(13, 3))},

    {std::make_pair(LAVA, XPOS), std::make_pair(3, glm::vec2(14, 0))},
    {std::make_pair(LAVA, XNEG), std::make_pair(3, glm::vec2(14, 0))},
    {std::make_pair(LAVA, ZPOS), std::make_pair(3, glm::vec2(14, 0))},
    {std::make_pair(LAVA, ZNEG), std::make_pair(3, glm::vec2(14, 0))},
    {std::make_pair(LAVA, YPOS), std::make_pair(3, glm::vec2(14, 0))},
    {std::make_pair(LAVA, YNEG), std::make_pair(3, glm::vec2(14, 0))},

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

    {std::make_pair(CEDAR_WOOD_X, XPOS), std::make_pair(0, glm::vec2(5, 12))},
    {std::make_pair(CEDAR_WOOD_X, XNEG), std::make_pair(0, glm::vec2(5, 12))},
    {std::make_pair(CEDAR_WOOD_X, ZPOS), std::make_pair(0, glm::vec2(5, 13))},
    {std::make_pair(CEDAR_WOOD_X, ZNEG), std::make_pair(0, glm::vec2(5, 13))},
    {std::make_pair(CEDAR_WOOD_X, YPOS), std::make_pair(0, glm::vec2(5, 13))},
    {std::make_pair(CEDAR_WOOD_X, YNEG), std::make_pair(0, glm::vec2(5, 13))},

    {std::make_pair(TEAK_WOOD_X, XPOS), std::make_pair(0, glm::vec2(6, 12))},
    {std::make_pair(TEAK_WOOD_X, XNEG), std::make_pair(0, glm::vec2(6, 12))},
    {std::make_pair(TEAK_WOOD_X, ZPOS), std::make_pair(0, glm::vec2(6, 13))},
    {std::make_pair(TEAK_WOOD_X, ZNEG), std::make_pair(0, glm::vec2(6, 13))},
    {std::make_pair(TEAK_WOOD_X, YPOS), std::make_pair(0, glm::vec2(6, 13))},
    {std::make_pair(TEAK_WOOD_X, YNEG), std::make_pair(0, glm::vec2(6, 13))},

    {std::make_pair(CHERRY_WOOD_X, XPOS), std::make_pair(0, glm::vec2(7, 12))},
    {std::make_pair(CHERRY_WOOD_X, XNEG), std::make_pair(0, glm::vec2(7, 12))},
    {std::make_pair(CHERRY_WOOD_X, ZPOS), std::make_pair(0, glm::vec2(7, 13))},
    {std::make_pair(CHERRY_WOOD_X, ZNEG), std::make_pair(0, glm::vec2(7, 13))},
    {std::make_pair(CHERRY_WOOD_X, YPOS), std::make_pair(0, glm::vec2(7, 13))},
    {std::make_pair(CHERRY_WOOD_X, YNEG), std::make_pair(0, glm::vec2(7, 13))},

    {std::make_pair(MAPLE_WOOD_X, XPOS), std::make_pair(0, glm::vec2(8, 12))},
    {std::make_pair(MAPLE_WOOD_X, XNEG), std::make_pair(0, glm::vec2(8, 12))},
    {std::make_pair(MAPLE_WOOD_X, ZPOS), std::make_pair(0, glm::vec2(8, 13))},
    {std::make_pair(MAPLE_WOOD_X, ZNEG), std::make_pair(0, glm::vec2(8, 13))},
    {std::make_pair(MAPLE_WOOD_X, YPOS), std::make_pair(0, glm::vec2(8, 13))},
    {std::make_pair(MAPLE_WOOD_X, YNEG), std::make_pair(0, glm::vec2(8, 13))},

    {std::make_pair(PINE_WOOD_X, XPOS), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_X, XNEG), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_X, ZPOS), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_X, ZNEG), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_X, YPOS), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_X, YNEG), std::make_pair(0, glm::vec2(9, 13))},

    {std::make_pair(WISTERIA_WOOD_X, XPOS), std::make_pair(0, glm::vec2(10, 12))},
    {std::make_pair(WISTERIA_WOOD_X, XNEG), std::make_pair(0, glm::vec2(10, 12))},
    {std::make_pair(WISTERIA_WOOD_X, ZPOS), std::make_pair(0, glm::vec2(10, 13))},
    {std::make_pair(WISTERIA_WOOD_X, ZNEG), std::make_pair(0, glm::vec2(10, 13))},
    {std::make_pair(WISTERIA_WOOD_X, YPOS), std::make_pair(0, glm::vec2(10, 13))},
    {std::make_pair(WISTERIA_WOOD_X, YNEG), std::make_pair(0, glm::vec2(10, 13))},

    {std::make_pair(CEDAR_WOOD_Y, XPOS), std::make_pair(0, glm::vec2(5, 13))},
    {std::make_pair(CEDAR_WOOD_Y, XNEG), std::make_pair(0, glm::vec2(5, 13))},
    {std::make_pair(CEDAR_WOOD_Y, ZPOS), std::make_pair(0, glm::vec2(5, 13))},
    {std::make_pair(CEDAR_WOOD_Y, ZNEG), std::make_pair(0, glm::vec2(5, 13))},
    {std::make_pair(CEDAR_WOOD_Y, YPOS), std::make_pair(0, glm::vec2(5, 12))},
    {std::make_pair(CEDAR_WOOD_Y, YNEG), std::make_pair(0, glm::vec2(5, 12))},

    {std::make_pair(TEAK_WOOD_Y, XPOS), std::make_pair(0, glm::vec2(6, 13))},
    {std::make_pair(TEAK_WOOD_Y, XNEG), std::make_pair(0, glm::vec2(6, 13))},
    {std::make_pair(TEAK_WOOD_Y, ZPOS), std::make_pair(0, glm::vec2(6, 13))},
    {std::make_pair(TEAK_WOOD_Y, ZNEG), std::make_pair(0, glm::vec2(6, 13))},
    {std::make_pair(TEAK_WOOD_Y, YPOS), std::make_pair(0, glm::vec2(6, 12))},
    {std::make_pair(TEAK_WOOD_Y, YNEG), std::make_pair(0, glm::vec2(6, 12))},

    {std::make_pair(CHERRY_WOOD_Y, XPOS), std::make_pair(0, glm::vec2(7, 13))},
    {std::make_pair(CHERRY_WOOD_Y, XNEG), std::make_pair(0, glm::vec2(7, 13))},
    {std::make_pair(CHERRY_WOOD_Y, ZPOS), std::make_pair(0, glm::vec2(7, 13))},
    {std::make_pair(CHERRY_WOOD_Y, ZNEG), std::make_pair(0, glm::vec2(7, 13))},
    {std::make_pair(CHERRY_WOOD_Y, YPOS), std::make_pair(0, glm::vec2(7, 12))},
    {std::make_pair(CHERRY_WOOD_Y, YNEG), std::make_pair(0, glm::vec2(7, 12))},

    {std::make_pair(MAPLE_WOOD_Y, XPOS), std::make_pair(0, glm::vec2(8, 13))},
    {std::make_pair(MAPLE_WOOD_Y, XNEG), std::make_pair(0, glm::vec2(8, 13))},
    {std::make_pair(MAPLE_WOOD_Y, ZPOS), std::make_pair(0, glm::vec2(8, 13))},
    {std::make_pair(MAPLE_WOOD_Y, ZNEG), std::make_pair(0, glm::vec2(8, 13))},
    {std::make_pair(MAPLE_WOOD_Y, YPOS), std::make_pair(0, glm::vec2(8, 12))},
    {std::make_pair(MAPLE_WOOD_Y, YNEG), std::make_pair(0, glm::vec2(8, 12))},

    {std::make_pair(PINE_WOOD_Y, XPOS), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_Y, XNEG), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_Y, ZPOS), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_Y, ZNEG), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_Y, YPOS), std::make_pair(0, glm::vec2(9, 12))},
    {std::make_pair(PINE_WOOD_Y, YNEG), std::make_pair(0, glm::vec2(9, 12))},

    {std::make_pair(WISTERIA_WOOD_Y, XPOS), std::make_pair(0, glm::vec2(10, 13))},
    {std::make_pair(WISTERIA_WOOD_Y, XNEG), std::make_pair(0, glm::vec2(10, 13))},
    {std::make_pair(WISTERIA_WOOD_Y, ZPOS), std::make_pair(0, glm::vec2(10, 13))},
    {std::make_pair(WISTERIA_WOOD_Y, ZNEG), std::make_pair(0, glm::vec2(10, 13))},
    {std::make_pair(WISTERIA_WOOD_Y, YPOS), std::make_pair(0, glm::vec2(10, 12))},
    {std::make_pair(WISTERIA_WOOD_Y, YNEG), std::make_pair(0, glm::vec2(10, 12))},

    {std::make_pair(CEDAR_WOOD_Z, XPOS), std::make_pair(0, glm::vec2(5, 13))},
    {std::make_pair(CEDAR_WOOD_Z, XNEG), std::make_pair(0, glm::vec2(5, 13))},
    {std::make_pair(CEDAR_WOOD_Z, ZPOS), std::make_pair(0, glm::vec2(5, 12))},
    {std::make_pair(CEDAR_WOOD_Z, ZNEG), std::make_pair(0, glm::vec2(5, 12))},
    {std::make_pair(CEDAR_WOOD_Z, YPOS), std::make_pair(0, glm::vec2(5, 13))},
    {std::make_pair(CEDAR_WOOD_Z, YNEG), std::make_pair(0, glm::vec2(5, 13))},

    {std::make_pair(TEAK_WOOD_Z, XPOS), std::make_pair(0, glm::vec2(6, 13))},
    {std::make_pair(TEAK_WOOD_Z, XNEG), std::make_pair(0, glm::vec2(6, 13))},
    {std::make_pair(TEAK_WOOD_Z, ZPOS), std::make_pair(0, glm::vec2(6, 12))},
    {std::make_pair(TEAK_WOOD_Z, ZNEG), std::make_pair(0, glm::vec2(6, 12))},
    {std::make_pair(TEAK_WOOD_Z, YPOS), std::make_pair(0, glm::vec2(6, 13))},
    {std::make_pair(TEAK_WOOD_Z, YNEG), std::make_pair(0, glm::vec2(6, 13))},

    {std::make_pair(CHERRY_WOOD_Z, XPOS), std::make_pair(0, glm::vec2(7, 13))},
    {std::make_pair(CHERRY_WOOD_Z, XNEG), std::make_pair(0, glm::vec2(7, 13))},
    {std::make_pair(CHERRY_WOOD_Z, ZPOS), std::make_pair(0, glm::vec2(7, 12))},
    {std::make_pair(CHERRY_WOOD_Z, ZNEG), std::make_pair(0, glm::vec2(7, 12))},
    {std::make_pair(CHERRY_WOOD_Z, YPOS), std::make_pair(0, glm::vec2(7, 13))},
    {std::make_pair(CHERRY_WOOD_Z, YNEG), std::make_pair(0, glm::vec2(7, 13))},

    {std::make_pair(MAPLE_WOOD_Z, XPOS), std::make_pair(0, glm::vec2(8, 13))},
    {std::make_pair(MAPLE_WOOD_Z, XNEG), std::make_pair(0, glm::vec2(8, 13))},
    {std::make_pair(MAPLE_WOOD_Z, ZPOS), std::make_pair(0, glm::vec2(8, 12))},
    {std::make_pair(MAPLE_WOOD_Z, ZNEG), std::make_pair(0, glm::vec2(8, 12))},
    {std::make_pair(MAPLE_WOOD_Z, YPOS), std::make_pair(0, glm::vec2(8, 13))},
    {std::make_pair(MAPLE_WOOD_Z, YNEG), std::make_pair(0, glm::vec2(8, 13))},

    {std::make_pair(PINE_WOOD_Z, XPOS), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_Z, XNEG), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_Z, ZPOS), std::make_pair(0, glm::vec2(9, 12))},
    {std::make_pair(PINE_WOOD_Z, ZNEG), std::make_pair(0, glm::vec2(9, 12))},
    {std::make_pair(PINE_WOOD_Z, YPOS), std::make_pair(0, glm::vec2(9, 13))},
    {std::make_pair(PINE_WOOD_Z, YNEG), std::make_pair(0, glm::vec2(9, 13))},

    {std::make_pair(WISTERIA_WOOD_Z, XPOS), std::make_pair(0, glm::vec2(10, 13))},
    {std::make_pair(WISTERIA_WOOD_Z, XNEG), std::make_pair(0, glm::vec2(10, 13))},
    {std::make_pair(WISTERIA_WOOD_Z, ZPOS), std::make_pair(0, glm::vec2(10, 12))},
    {std::make_pair(WISTERIA_WOOD_Z, ZNEG), std::make_pair(0, glm::vec2(10, 12))},
    {std::make_pair(WISTERIA_WOOD_Z, YPOS), std::make_pair(0, glm::vec2(10, 13))},
    {std::make_pair(WISTERIA_WOOD_Z, YNEG), std::make_pair(0, glm::vec2(10, 13))},

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

    {std::make_pair(WISTERIA_BLOSSOMS_1, XPOS), std::make_pair(4, glm::vec2(10, 9))},
    {std::make_pair(WISTERIA_BLOSSOMS_1, XNEG), std::make_pair(4, glm::vec2(10, 9))},
    {std::make_pair(WISTERIA_BLOSSOMS_1, ZPOS), std::make_pair(4, glm::vec2(10, 9))},
    {std::make_pair(WISTERIA_BLOSSOMS_1, ZNEG), std::make_pair(4, glm::vec2(10, 9))},
    {std::make_pair(WISTERIA_BLOSSOMS_1, YPOS), std::make_pair(4, glm::vec2(10, 9))},
    {std::make_pair(WISTERIA_BLOSSOMS_1, YNEG), std::make_pair(4, glm::vec2(10, 9))},

    {std::make_pair(WISTERIA_BLOSSOMS_2, XPOS), std::make_pair(4, glm::vec2(10, 10))},
    {std::make_pair(WISTERIA_BLOSSOMS_2, XNEG), std::make_pair(4, glm::vec2(10, 10))},
    {std::make_pair(WISTERIA_BLOSSOMS_2, ZPOS), std::make_pair(4, glm::vec2(10, 10))},
    {std::make_pair(WISTERIA_BLOSSOMS_2, ZNEG), std::make_pair(4, glm::vec2(10, 10))},
    {std::make_pair(WISTERIA_BLOSSOMS_2, YPOS), std::make_pair(4, glm::vec2(10, 10))},
    {std::make_pair(WISTERIA_BLOSSOMS_2, YNEG), std::make_pair(4, glm::vec2(10, 10))},

    {std::make_pair(WISTERIA_BLOSSOMS_3, XPOS), std::make_pair(4, glm::vec2(10, 11))},
    {std::make_pair(WISTERIA_BLOSSOMS_3, XNEG), std::make_pair(4, glm::vec2(10, 11))},
    {std::make_pair(WISTERIA_BLOSSOMS_3, ZPOS), std::make_pair(4, glm::vec2(10, 11))},
    {std::make_pair(WISTERIA_BLOSSOMS_3, ZNEG), std::make_pair(4, glm::vec2(10, 11))},
    {std::make_pair(WISTERIA_BLOSSOMS_3, YPOS), std::make_pair(4, glm::vec2(10, 11))},
    {std::make_pair(WISTERIA_BLOSSOMS_3, YNEG), std::make_pair(4, glm::vec2(10, 11))},

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

    {std::make_pair(CEDAR_PLANKS_1, XPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS_1, XNEG), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS_1, ZPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS_1, ZNEG), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS_1, YPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS_1, YNEG), std::make_pair(0, glm::vec2(5, 15))},

    {std::make_pair(TEAK_PLANKS_1, XPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS_1, XNEG), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS_1, ZPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS_1, ZNEG), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS_1, YPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS_1, YNEG), std::make_pair(0, glm::vec2(6, 15))},

    {std::make_pair(CHERRY_PLANKS_1, XPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS_1, XNEG), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS_1, ZPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS_1, ZNEG), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS_1, YPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS_1, YNEG), std::make_pair(0, glm::vec2(7, 15))},

    {std::make_pair(MAPLE_PLANKS_1, XPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS_1, XNEG), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS_1, ZPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS_1, ZNEG), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS_1, YPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS_1, YNEG), std::make_pair(0, glm::vec2(8, 15))},

    {std::make_pair(PINE_PLANKS_1, XPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS_1, XNEG), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS_1, ZPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS_1, ZNEG), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS_1, YPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS_1, YNEG), std::make_pair(0, glm::vec2(9, 15))},

    {std::make_pair(WISTERIA_PLANKS_1, XPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS_1, XNEG), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS_1, ZPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS_1, ZNEG), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS_1, YPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS_1, YNEG), std::make_pair(0, glm::vec2(10, 15))},

    {std::make_pair(CEDAR_PLANKS_2, XPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS_2, XNEG), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS_2, ZPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS_2, ZNEG), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS_2, YPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_PLANKS_2, YNEG), std::make_pair(0, glm::vec2(5, 15))},

    {std::make_pair(TEAK_PLANKS_2, XPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS_2, XNEG), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS_2, ZPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS_2, ZNEG), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS_2, YPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_PLANKS_2, YNEG), std::make_pair(0, glm::vec2(6, 15))},

    {std::make_pair(CHERRY_PLANKS_2, XPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS_2, XNEG), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS_2, ZPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS_2, ZNEG), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS_2, YPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_PLANKS_2, YNEG), std::make_pair(0, glm::vec2(7, 15))},

    {std::make_pair(MAPLE_PLANKS_2, XPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS_2, XNEG), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS_2, ZPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS_2, ZNEG), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS_2, YPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_PLANKS_2, YNEG), std::make_pair(0, glm::vec2(8, 15))},

    {std::make_pair(PINE_PLANKS_2, XPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS_2, XNEG), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS_2, ZPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS_2, ZNEG), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS_2, YPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_PLANKS_2, YNEG), std::make_pair(0, glm::vec2(9, 15))},

    {std::make_pair(WISTERIA_PLANKS_2, XPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS_2, XNEG), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS_2, ZPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS_2, ZNEG), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS_2, YPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_PLANKS_2, YNEG), std::make_pair(0, glm::vec2(10, 15))},

    {std::make_pair(CEDAR_WINDOW_X, XPOS), std::make_pair(0, glm::vec2(5, 14))},
    {std::make_pair(CEDAR_WINDOW_X, XNEG), std::make_pair(0, glm::vec2(5, 14))},
    {std::make_pair(CEDAR_WINDOW_X, ZPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_WINDOW_X, ZNEG), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_WINDOW_X, YPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_WINDOW_X, YNEG), std::make_pair(0, glm::vec2(5, 15))},

    {std::make_pair(TEAK_WINDOW_X, XPOS), std::make_pair(0, glm::vec2(6, 14))},
    {std::make_pair(TEAK_WINDOW_X, XNEG), std::make_pair(0, glm::vec2(6, 14))},
    {std::make_pair(TEAK_WINDOW_X, ZPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_WINDOW_X, ZNEG), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_WINDOW_X, YPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_WINDOW_X, YNEG), std::make_pair(0, glm::vec2(6, 15))},

    {std::make_pair(CHERRY_WINDOW_X, XPOS), std::make_pair(0, glm::vec2(7, 14))},
    {std::make_pair(CHERRY_WINDOW_X, XNEG), std::make_pair(0, glm::vec2(7, 14))},
    {std::make_pair(CHERRY_WINDOW_X, ZPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_WINDOW_X, ZNEG), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_WINDOW_X, YPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_WINDOW_X, YNEG), std::make_pair(0, glm::vec2(7, 15))},

    {std::make_pair(MAPLE_WINDOW_X, XPOS), std::make_pair(0, glm::vec2(8, 14))},
    {std::make_pair(MAPLE_WINDOW_X, XNEG), std::make_pair(0, glm::vec2(8, 14))},
    {std::make_pair(MAPLE_WINDOW_X, ZPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_WINDOW_X, ZNEG), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_WINDOW_X, YPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_WINDOW_X, YNEG), std::make_pair(0, glm::vec2(8, 15))},

    {std::make_pair(PINE_WINDOW_X, XPOS), std::make_pair(0, glm::vec2(9, 14))},
    {std::make_pair(PINE_WINDOW_X, XNEG), std::make_pair(0, glm::vec2(9, 14))},
    {std::make_pair(PINE_WINDOW_X, ZPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_WINDOW_X, ZNEG), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_WINDOW_X, YPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_WINDOW_X, YNEG), std::make_pair(0, glm::vec2(9, 15))},

    {std::make_pair(WISTERIA_WINDOW_X, XPOS), std::make_pair(0, glm::vec2(10, 14))},
    {std::make_pair(WISTERIA_WINDOW_X, XNEG), std::make_pair(0, glm::vec2(10, 14))},
    {std::make_pair(WISTERIA_WINDOW_X, ZPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_WINDOW_X, ZNEG), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_WINDOW_X, YPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_WINDOW_X, YNEG), std::make_pair(0, glm::vec2(10, 15))},

    {std::make_pair(CEDAR_WINDOW_Z, XPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_WINDOW_Z, XNEG), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_WINDOW_Z, ZPOS), std::make_pair(0, glm::vec2(5, 14))},
    {std::make_pair(CEDAR_WINDOW_Z, ZNEG), std::make_pair(0, glm::vec2(5, 14))},
    {std::make_pair(CEDAR_WINDOW_Z, YPOS), std::make_pair(0, glm::vec2(5, 15))},
    {std::make_pair(CEDAR_WINDOW_Z, YNEG), std::make_pair(0, glm::vec2(5, 15))},

    {std::make_pair(TEAK_WINDOW_Z, XPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_WINDOW_Z, XNEG), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_WINDOW_Z, ZPOS), std::make_pair(0, glm::vec2(6, 14))},
    {std::make_pair(TEAK_WINDOW_Z, ZNEG), std::make_pair(0, glm::vec2(6, 14))},
    {std::make_pair(TEAK_WINDOW_Z, YPOS), std::make_pair(0, glm::vec2(6, 15))},
    {std::make_pair(TEAK_WINDOW_Z, YNEG), std::make_pair(0, glm::vec2(6, 15))},

    {std::make_pair(CHERRY_WINDOW_Z, XPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_WINDOW_Z, XNEG), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_WINDOW_Z, ZPOS), std::make_pair(0, glm::vec2(7, 14))},
    {std::make_pair(CHERRY_WINDOW_Z, ZNEG), std::make_pair(0, glm::vec2(7, 14))},
    {std::make_pair(CHERRY_WINDOW_Z, YPOS), std::make_pair(0, glm::vec2(7, 15))},
    {std::make_pair(CHERRY_WINDOW_Z, YNEG), std::make_pair(0, glm::vec2(7, 15))},

    {std::make_pair(MAPLE_WINDOW_Z, XPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_WINDOW_Z, XNEG), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_WINDOW_Z, ZPOS), std::make_pair(0, glm::vec2(8, 14))},
    {std::make_pair(MAPLE_WINDOW_Z, ZNEG), std::make_pair(0, glm::vec2(8, 14))},
    {std::make_pair(MAPLE_WINDOW_Z, YPOS), std::make_pair(0, glm::vec2(8, 15))},
    {std::make_pair(MAPLE_WINDOW_Z, YNEG), std::make_pair(0, glm::vec2(8, 15))},

    {std::make_pair(PINE_WINDOW_Z, XPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_WINDOW_Z, XNEG), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_WINDOW_Z, ZPOS), std::make_pair(0, glm::vec2(9, 14))},
    {std::make_pair(PINE_WINDOW_Z, ZNEG), std::make_pair(0, glm::vec2(9, 14))},
    {std::make_pair(PINE_WINDOW_Z, YPOS), std::make_pair(0, glm::vec2(9, 15))},
    {std::make_pair(PINE_WINDOW_Z, YNEG), std::make_pair(0, glm::vec2(9, 15))},

    {std::make_pair(WISTERIA_WINDOW_Z, XPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_WINDOW_Z, XNEG), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_WINDOW_Z, ZPOS), std::make_pair(0, glm::vec2(10, 14))},
    {std::make_pair(WISTERIA_WINDOW_Z, ZNEG), std::make_pair(0, glm::vec2(10, 14))},
    {std::make_pair(WISTERIA_WINDOW_Z, YPOS), std::make_pair(0, glm::vec2(10, 15))},
    {std::make_pair(WISTERIA_WINDOW_Z, YNEG), std::make_pair(0, glm::vec2(10, 15))},

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

    {std::make_pair(ROOF_TILES, XPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES, XNEG), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES, ZPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES, ZNEG), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES, YPOS), std::make_pair(0, glm::vec2(11, 15))},
    {std::make_pair(ROOF_TILES, YNEG), std::make_pair(0, glm::vec2(11, 15))},

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

    {std::make_pair(STRAW, XPOS), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW, XNEG), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW, ZPOS), std::make_pair(0, glm::vec2(11, 14))},
    {std::make_pair(STRAW, ZNEG), std::make_pair(0, glm::vec2(11, 14))},
    {std::make_pair(STRAW, YPOS), std::make_pair(0, glm::vec2(11, 13))},
    {std::make_pair(STRAW, YNEG), std::make_pair(0, glm::vec2(11, 13))},

    {std::make_pair(LILY_PAD, YPOS), std::make_pair(0, glm::vec2(0, 9))},
    {std::make_pair(LILY_PAD, YNEG), std::make_pair(0, glm::vec2(0, 9))},

    {std::make_pair(LOTUS_1, YPOS), std::make_pair(0, glm::vec2(0, 9))},
    {std::make_pair(LOTUS_1, YNEG), std::make_pair(0, glm::vec2(0, 9))},
    {std::make_pair(LOTUS_1, XPOS_ZPOS), std::make_pair(0, glm::vec2(1, 9))},
    {std::make_pair(LOTUS_1, XPOS_ZNEG), std::make_pair(0, glm::vec2(1, 9))},
    {std::make_pair(LOTUS_1, XNEG_ZPOS), std::make_pair(0, glm::vec2(1, 9))},
    {std::make_pair(LOTUS_1, XNEG_ZNEG), std::make_pair(0, glm::vec2(1, 9))},

    {std::make_pair(LOTUS_2, YPOS), std::make_pair(0, glm::vec2(0, 9))},
    {std::make_pair(LOTUS_2, YNEG), std::make_pair(0, glm::vec2(0, 9))},
    {std::make_pair(LOTUS_2, XPOS_ZPOS), std::make_pair(0, glm::vec2(1, 10))},
    {std::make_pair(LOTUS_2, XPOS_ZNEG), std::make_pair(0, glm::vec2(1, 10))},
    {std::make_pair(LOTUS_2, XNEG_ZPOS), std::make_pair(0, glm::vec2(1, 10))},
    {std::make_pair(LOTUS_2, XNEG_ZNEG), std::make_pair(0, glm::vec2(1, 10))},

    {std::make_pair(TILLED_DIRT, XPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(TILLED_DIRT, XNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(TILLED_DIRT, ZPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(TILLED_DIRT, ZNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(TILLED_DIRT, YPOS), std::make_pair(0, glm::vec2(0, 8))},
    {std::make_pair(TILLED_DIRT, YNEG), std::make_pair(0, glm::vec2(0, 13))},

    {std::make_pair(IRRIGATED_SOIL, XPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(IRRIGATED_SOIL, XNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(IRRIGATED_SOIL, ZPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(IRRIGATED_SOIL, ZNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(IRRIGATED_SOIL, YPOS), std::make_pair(0, glm::vec2(2, 8))},
    {std::make_pair(IRRIGATED_SOIL, YNEG), std::make_pair(0, glm::vec2(0, 13))},

    {std::make_pair(PATH, XPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(PATH, XNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(PATH, ZPOS), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(PATH, ZNEG), std::make_pair(0, glm::vec2(0, 13))},
    {std::make_pair(PATH, YPOS), std::make_pair(0, glm::vec2(1, 8))},
    {std::make_pair(PATH, YNEG), std::make_pair(0, glm::vec2(0, 13))},

    {std::make_pair(WHEAT_1, XPOS), std::make_pair(0, glm::vec2(0, 7))},
    {std::make_pair(WHEAT_1, XNEG), std::make_pair(0, glm::vec2(0, 7))},
    {std::make_pair(WHEAT_1, ZPOS), std::make_pair(0, glm::vec2(0, 7))},
    {std::make_pair(WHEAT_1, ZNEG), std::make_pair(0, glm::vec2(0, 7))},

    {std::make_pair(WHEAT_2, XPOS), std::make_pair(0, glm::vec2(1, 7))},
    {std::make_pair(WHEAT_2, XNEG), std::make_pair(0, glm::vec2(1, 7))},
    {std::make_pair(WHEAT_2, ZPOS), std::make_pair(0, glm::vec2(1, 7))},
    {std::make_pair(WHEAT_2, ZNEG), std::make_pair(0, glm::vec2(1, 7))},

    {std::make_pair(WHEAT_3, XPOS), std::make_pair(0, glm::vec2(2, 7))},
    {std::make_pair(WHEAT_3, XNEG), std::make_pair(0, glm::vec2(2, 7))},
    {std::make_pair(WHEAT_3, ZPOS), std::make_pair(0, glm::vec2(2, 7))},
    {std::make_pair(WHEAT_3, ZNEG), std::make_pair(0, glm::vec2(2, 7))},

    {std::make_pair(WHEAT_4, XPOS), std::make_pair(0, glm::vec2(3, 7))},
    {std::make_pair(WHEAT_4, XNEG), std::make_pair(0, glm::vec2(3, 7))},
    {std::make_pair(WHEAT_4, ZPOS), std::make_pair(0, glm::vec2(3, 7))},
    {std::make_pair(WHEAT_4, ZNEG), std::make_pair(0, glm::vec2(3, 7))},

    {std::make_pair(WHEAT_5, XPOS), std::make_pair(0, glm::vec2(4, 7))},
    {std::make_pair(WHEAT_5, XNEG), std::make_pair(0, glm::vec2(4, 7))},
    {std::make_pair(WHEAT_5, ZPOS), std::make_pair(0, glm::vec2(4, 7))},
    {std::make_pair(WHEAT_5, ZNEG), std::make_pair(0, glm::vec2(4, 7))},

    {std::make_pair(WHEAT_6, XPOS), std::make_pair(0, glm::vec2(5, 7))},
    {std::make_pair(WHEAT_6, XNEG), std::make_pair(0, glm::vec2(5, 7))},
    {std::make_pair(WHEAT_6, ZPOS), std::make_pair(0, glm::vec2(5, 7))},
    {std::make_pair(WHEAT_6, ZNEG), std::make_pair(0, glm::vec2(5, 7))},

    {std::make_pair(WHEAT_7, XPOS), std::make_pair(0, glm::vec2(6, 7))},
    {std::make_pair(WHEAT_7, XNEG), std::make_pair(0, glm::vec2(6, 7))},
    {std::make_pair(WHEAT_7, ZPOS), std::make_pair(0, glm::vec2(6, 7))},
    {std::make_pair(WHEAT_7, ZNEG), std::make_pair(0, glm::vec2(6, 7))},

    {std::make_pair(WHEAT_8, XPOS), std::make_pair(0, glm::vec2(7, 7))},
    {std::make_pair(WHEAT_8, XNEG), std::make_pair(0, glm::vec2(7, 7))},
    {std::make_pair(WHEAT_8, ZPOS), std::make_pair(0, glm::vec2(7, 7))},
    {std::make_pair(WHEAT_8, ZNEG), std::make_pair(0, glm::vec2(7, 7))},

    {std::make_pair(RICE_1, XPOS_ZPOS), std::make_pair(0, glm::vec2(0, 6))},
    {std::make_pair(RICE_1, XPOS_ZNEG), std::make_pair(0, glm::vec2(0, 6))},
    {std::make_pair(RICE_1, XNEG_ZPOS), std::make_pair(0, glm::vec2(0, 6))},
    {std::make_pair(RICE_1, XNEG_ZNEG), std::make_pair(0, glm::vec2(0, 6))},

    {std::make_pair(RICE_2, XPOS_ZPOS), std::make_pair(0, glm::vec2(1, 6))},
    {std::make_pair(RICE_2, XPOS_ZNEG), std::make_pair(0, glm::vec2(1, 6))},
    {std::make_pair(RICE_2, XNEG_ZPOS), std::make_pair(0, glm::vec2(1, 6))},
    {std::make_pair(RICE_2, XNEG_ZNEG), std::make_pair(0, glm::vec2(1, 6))},

    {std::make_pair(RICE_3, XPOS_ZPOS), std::make_pair(0, glm::vec2(2, 6))},
    {std::make_pair(RICE_3, XPOS_ZNEG), std::make_pair(0, glm::vec2(2, 6))},
    {std::make_pair(RICE_3, XNEG_ZPOS), std::make_pair(0, glm::vec2(2, 6))},
    {std::make_pair(RICE_3, XNEG_ZNEG), std::make_pair(0, glm::vec2(2, 6))},

    {std::make_pair(RICE_4, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 6))},
    {std::make_pair(RICE_4, XPOS_ZNEG), std::make_pair(0, glm::vec2(3, 6))},
    {std::make_pair(RICE_4, XNEG_ZPOS), std::make_pair(0, glm::vec2(3, 6))},
    {std::make_pair(RICE_4, XNEG_ZNEG), std::make_pair(0, glm::vec2(3, 6))},

    {std::make_pair(RICE_5, XPOS_ZPOS), std::make_pair(0, glm::vec2(4, 6))},
    {std::make_pair(RICE_5, XPOS_ZNEG), std::make_pair(0, glm::vec2(4, 6))},
    {std::make_pair(RICE_5, XNEG_ZPOS), std::make_pair(0, glm::vec2(4, 6))},
    {std::make_pair(RICE_5, XNEG_ZNEG), std::make_pair(0, glm::vec2(4, 6))},

    {std::make_pair(RICE_6, XPOS_ZPOS), std::make_pair(0, glm::vec2(5, 6))},
    {std::make_pair(RICE_6, XPOS_ZNEG), std::make_pair(0, glm::vec2(5, 6))},
    {std::make_pair(RICE_6, XNEG_ZPOS), std::make_pair(0, glm::vec2(5, 6))},
    {std::make_pair(RICE_6, XNEG_ZNEG), std::make_pair(0, glm::vec2(5, 6))},

    {std::make_pair(RICE_01, XPOS_ZPOS), std::make_pair(0, glm::vec2(6, 6))},
    {std::make_pair(RICE_01, XPOS_ZNEG), std::make_pair(0, glm::vec2(6, 6))},
    {std::make_pair(RICE_01, XNEG_ZPOS), std::make_pair(0, glm::vec2(6, 6))},
    {std::make_pair(RICE_01, XNEG_ZNEG), std::make_pair(0, glm::vec2(6, 6))},
    {std::make_pair(RICE_01, YPOS), std::make_pair(2, glm::vec2(13, 3))},

    {std::make_pair(RICE_02, XPOS_ZPOS), std::make_pair(0, glm::vec2(7, 6))},
    {std::make_pair(RICE_02, XPOS_ZNEG), std::make_pair(0, glm::vec2(7, 6))},
    {std::make_pair(RICE_02, XNEG_ZPOS), std::make_pair(0, glm::vec2(7, 6))},
    {std::make_pair(RICE_02, XNEG_ZNEG), std::make_pair(0, glm::vec2(7, 6))},
    {std::make_pair(RICE_02, YPOS), std::make_pair(2, glm::vec2(13, 3))},

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
    {std::make_pair(BAMBOO_2, XPOS_ZNEG), std::make_pair(0, glm::vec2(12, 14))},
    {std::make_pair(BAMBOO_2, XNEG_ZPOS), std::make_pair(0, glm::vec2(12, 14))},
    {std::make_pair(BAMBOO_2, XNEG_ZNEG), std::make_pair(0, glm::vec2(12, 14))},

    {std::make_pair(BAMBOO_3, XPOS), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_3, XNEG), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_3, ZPOS), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_3, ZNEG), std::make_pair(0, glm::vec2(12, 13))},
    {std::make_pair(BAMBOO_3, YPOS), std::make_pair(0, glm::vec2(12, 12))},
    {std::make_pair(BAMBOO_3, YNEG), std::make_pair(0, glm::vec2(12, 12))},
    {std::make_pair(BAMBOO_3, XPOS_ZPOS), std::make_pair(0, glm::vec2(12, 15))},
    {std::make_pair(BAMBOO_3, XPOS_ZNEG), std::make_pair(0, glm::vec2(12, 15))},
    {std::make_pair(BAMBOO_3, XNEG_ZPOS), std::make_pair(0, glm::vec2(12, 15))},
    {std::make_pair(BAMBOO_3, XNEG_ZNEG), std::make_pair(0, glm::vec2(12, 15))},

    {std::make_pair(TATAMI_XL, XPOS), std::make_pair(0, glm::vec2(12, 11))},
    {std::make_pair(TATAMI_XL, XNEG), std::make_pair(0, glm::vec2(12, 11))},
    {std::make_pair(TATAMI_XL, ZPOS), std::make_pair(0, glm::vec2(12, 11))},
    {std::make_pair(TATAMI_XL, ZNEG), std::make_pair(0, glm::vec2(11, 11))},
    {std::make_pair(TATAMI_XL, YPOS), std::make_pair(0, glm::vec2(12, 11))},
    {std::make_pair(TATAMI_XL, YNEG), std::make_pair(0, glm::vec2(12, 11))},

    {std::make_pair(TATAMI_XR, XPOS), std::make_pair(0, glm::vec2(12, 11))},
    {std::make_pair(TATAMI_XR, XNEG), std::make_pair(0, glm::vec2(12, 10))},
    {std::make_pair(TATAMI_XR, ZPOS), std::make_pair(0, glm::vec2(11, 11))},
    {std::make_pair(TATAMI_XR, ZNEG), std::make_pair(0, glm::vec2(12, 10))},
    {std::make_pair(TATAMI_XR, YPOS), std::make_pair(0, glm::vec2(12, 10))},
    {std::make_pair(TATAMI_XR, YNEG), std::make_pair(0, glm::vec2(12, 10))},

    {std::make_pair(TATAMI_ZT, XPOS), std::make_pair(0, glm::vec2(11, 11))},
    {std::make_pair(TATAMI_ZT, XNEG), std::make_pair(0, glm::vec2(11, 10))},
    {std::make_pair(TATAMI_ZT, ZPOS), std::make_pair(0, glm::vec2(11, 10))},
    {std::make_pair(TATAMI_ZT, ZNEG), std::make_pair(0, glm::vec2(11, 10))},
    {std::make_pair(TATAMI_ZT, YPOS), std::make_pair(0, glm::vec2(11, 11))},
    {std::make_pair(TATAMI_ZT, YNEG), std::make_pair(0, glm::vec2(11, 11))},

    {std::make_pair(TATAMI_ZB, XPOS), std::make_pair(0, glm::vec2(11, 10))},
    {std::make_pair(TATAMI_ZB, XNEG), std::make_pair(0, glm::vec2(11, 11))},
    {std::make_pair(TATAMI_ZB, ZPOS), std::make_pair(0, glm::vec2(11, 10))},
    {std::make_pair(TATAMI_ZB, ZNEG), std::make_pair(0, glm::vec2(11, 10))},
    {std::make_pair(TATAMI_ZB, YPOS), std::make_pair(0, glm::vec2(11, 10))},
    {std::make_pair(TATAMI_ZB, YNEG), std::make_pair(0, glm::vec2(11, 10))},

    {std::make_pair(CLOTH_1, XPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_1, XNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_1, ZPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_1, ZNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_1, YPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_1, YNEG), std::make_pair(0, glm::vec2(12, 9))},

    {std::make_pair(CLOTH_2, XPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_2, XNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_2, ZPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_2, ZNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_2, YPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_2, YNEG), std::make_pair(0, glm::vec2(12, 9))},

    {std::make_pair(CLOTH_3, XPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_3, XNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_3, ZPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_3, ZNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_3, YPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_3, YNEG), std::make_pair(0, glm::vec2(12, 9))},

    {std::make_pair(CLOTH_4, XPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_4, XNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_4, ZPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_4, ZNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_4, YPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_4, YNEG), std::make_pair(0, glm::vec2(12, 9))},

    {std::make_pair(CLOTH_5, XPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_5, XNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_5, ZPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_5, ZNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_5, YPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_5, YNEG), std::make_pair(0, glm::vec2(12, 9))},

    {std::make_pair(CLOTH_6, XPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_6, XNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_6, ZPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_6, ZNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_6, YPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_6, YNEG), std::make_pair(0, glm::vec2(12, 9))},

    {std::make_pair(CLOTH_7, XPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_7, XNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_7, ZPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_7, ZNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_7, YPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_7, YNEG), std::make_pair(0, glm::vec2(12, 9))},

    {std::make_pair(CLOTH_8, XPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_8, XNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_8, ZPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_8, ZNEG), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_8, YPOS), std::make_pair(0, glm::vec2(12, 9))},
    {std::make_pair(CLOTH_8, YNEG), std::make_pair(0, glm::vec2(12, 9))},

    {std::make_pair(PAPER_LANTERN, XPOS), std::make_pair(4, glm::vec2(13, 15))},
    {std::make_pair(PAPER_LANTERN, XNEG), std::make_pair(4, glm::vec2(13, 15))},
    {std::make_pair(PAPER_LANTERN, ZPOS), std::make_pair(4, glm::vec2(13, 15))},
    {std::make_pair(PAPER_LANTERN, ZNEG), std::make_pair(4, glm::vec2(13, 15))},
    {std::make_pair(PAPER_LANTERN, YPOS), std::make_pair(4, glm::vec2(13, 14))},
    {std::make_pair(PAPER_LANTERN, YNEG), std::make_pair(4, glm::vec2(13, 14))},

    {std::make_pair(WOOD_LANTERN, XPOS), std::make_pair(4, glm::vec2(13, 13))},
    {std::make_pair(WOOD_LANTERN, XNEG), std::make_pair(4, glm::vec2(13, 13))},
    {std::make_pair(WOOD_LANTERN, ZPOS), std::make_pair(4, glm::vec2(13, 13))},
    {std::make_pair(WOOD_LANTERN, ZNEG), std::make_pair(4, glm::vec2(13, 13))},
    {std::make_pair(WOOD_LANTERN, YPOS), std::make_pair(4, glm::vec2(13, 12))},
    {std::make_pair(WOOD_LANTERN, YNEG), std::make_pair(4, glm::vec2(13, 12))},

    {std::make_pair(PAINTING_1_XP, XPOS), std::make_pair(0, glm::vec2(14, 14))},
    {std::make_pair(PAINTING_1_XP, XNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_XP, ZPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_XP, ZNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_XP, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_XP, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_2_XP, XPOS), std::make_pair(0, glm::vec2(14, 12))},
    {std::make_pair(PAINTING_2_XP, XNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_XP, ZPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_XP, ZNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_XP, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_XP, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_3_XP, XPOS), std::make_pair(0, glm::vec2(14, 10))},
    {std::make_pair(PAINTING_3_XP, XNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_XP, ZPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_XP, ZNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_XP, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_XP, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_4_XP, XPOS), std::make_pair(0, glm::vec2(13, 7))},
    {std::make_pair(PAINTING_4_XP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_XP, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_XP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_XP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_XP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_5_XP, XPOS), std::make_pair(0, glm::vec2(13, 4))},
    {std::make_pair(PAINTING_5_XP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_XP, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_XP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_XP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_XP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_6L_XP, XPOS), std::make_pair(0, glm::vec2(9, 7))},
    {std::make_pair(PAINTING_6L_XP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_XP, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_XP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_XP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_XP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_6R_XP, XPOS), std::make_pair(0, glm::vec2(11, 7))},
    {std::make_pair(PAINTING_6R_XP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_XP, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_XP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_XP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_XP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_7T_XP, XPOS), std::make_pair(0, glm::vec2(11, 5))},
    {std::make_pair(PAINTING_7T_XP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_XP, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_XP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_XP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_XP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_7B_XP, XPOS), std::make_pair(0, glm::vec2(11, 3))},
    {std::make_pair(PAINTING_7B_XP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_XP, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_XP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_XP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_XP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_1_XN, XPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_XN, XNEG), std::make_pair(0, glm::vec2(14, 14))},
    {std::make_pair(PAINTING_1_XN, ZPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_XN, ZNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_XN, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_XN, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_2_XN, XPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_XN, XNEG), std::make_pair(0, glm::vec2(14, 12))},
    {std::make_pair(PAINTING_2_XN, ZPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_XN, ZNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_XN, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_XN, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_3_XN, XPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_XN, XNEG), std::make_pair(0, glm::vec2(14, 10))},
    {std::make_pair(PAINTING_3_XN, ZPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_XN, ZNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_XN, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_XN, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_4_XN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_XN, XNEG), std::make_pair(0, glm::vec2(13, 7))},
    {std::make_pair(PAINTING_4_XN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_XN, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_XN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_XN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_5_XN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_XN, XNEG), std::make_pair(0, glm::vec2(13, 4))},
    {std::make_pair(PAINTING_5_XN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_XN, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_XN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_XN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_6L_XN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_XN, XNEG), std::make_pair(0, glm::vec2(9, 7))},
    {std::make_pair(PAINTING_6L_XN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_XN, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_XN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_XN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_6R_XN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_XN, XNEG), std::make_pair(0, glm::vec2(11, 7))},
    {std::make_pair(PAINTING_6R_XN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_XN, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_XN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_XN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_7T_XN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_XN, XNEG), std::make_pair(0, glm::vec2(11, 5))},
    {std::make_pair(PAINTING_7T_XN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_XN, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_XN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_XN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_7B_XN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_XN, XNEG), std::make_pair(0, glm::vec2(11, 3))},
    {std::make_pair(PAINTING_7B_XN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_XN, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_XN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_XN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_1_ZP, XPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_ZP, XNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_ZP, ZPOS), std::make_pair(0, glm::vec2(14, 14))},
    {std::make_pair(PAINTING_1_ZP, ZNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_ZP, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_ZP, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_2_ZP, XPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_ZP, XNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_ZP, ZPOS), std::make_pair(0, glm::vec2(14, 12))},
    {std::make_pair(PAINTING_2_ZP, ZNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_ZP, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_ZP, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_3_ZP, XPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_ZP, XNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_ZP, ZPOS), std::make_pair(0, glm::vec2(14, 10))},
    {std::make_pair(PAINTING_3_ZP, ZNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_ZP, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_ZP, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_4_ZP, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_ZP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_ZP, ZPOS), std::make_pair(0, glm::vec2(13, 7))},
    {std::make_pair(PAINTING_4_ZP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_ZP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_ZP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_5_ZP, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_ZP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_ZP, ZPOS), std::make_pair(0, glm::vec2(13, 4))},
    {std::make_pair(PAINTING_5_ZP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_ZP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_ZP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_6L_ZP, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_ZP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_ZP, ZPOS), std::make_pair(0, glm::vec2(9, 7))},
    {std::make_pair(PAINTING_6L_ZP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_ZP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_ZP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_6R_ZP, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_ZP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_ZP, ZPOS), std::make_pair(0, glm::vec2(11, 7))},
    {std::make_pair(PAINTING_6R_ZP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_ZP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_ZP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_7T_ZP, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_ZP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_ZP, ZPOS), std::make_pair(0, glm::vec2(11, 5))},
    {std::make_pair(PAINTING_7T_ZP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_ZP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_ZP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_7B_ZP, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_ZP, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_ZP, ZPOS), std::make_pair(0, glm::vec2(11, 3))},
    {std::make_pair(PAINTING_7B_ZP, ZNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_ZP, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_ZP, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_1_ZN, XPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_ZN, XNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_ZN, ZPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_ZN, ZNEG), std::make_pair(0, glm::vec2(14, 14))},
    {std::make_pair(PAINTING_1_ZN, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_1_ZN, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_2_ZN, XPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_ZN, XNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_ZN, ZPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_ZN, ZNEG), std::make_pair(0, glm::vec2(14, 12))},
    {std::make_pair(PAINTING_2_ZN, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_2_ZN, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_3_ZN, XPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_ZN, XNEG), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_ZN, ZPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_ZN, ZNEG), std::make_pair(0, glm::vec2(14, 10))},
    {std::make_pair(PAINTING_3_ZN, YPOS), std::make_pair(0, glm::vec2(13, 11))},
    {std::make_pair(PAINTING_3_ZN, YNEG), std::make_pair(0, glm::vec2(13, 11))},

    {std::make_pair(PAINTING_4_ZN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_ZN, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_ZN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_ZN, ZNEG), std::make_pair(0, glm::vec2(13, 7))},
    {std::make_pair(PAINTING_4_ZN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_4_ZN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_5_ZN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_ZN, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_ZN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_ZN, ZNEG), std::make_pair(0, glm::vec2(13, 4))},
    {std::make_pair(PAINTING_5_ZN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_5_ZN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_6L_ZN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_ZN, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_ZN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_ZN, ZNEG), std::make_pair(0, glm::vec2(9, 7))},
    {std::make_pair(PAINTING_6L_ZN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6L_ZN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_6R_ZN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_ZN, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_ZN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_ZN, ZNEG), std::make_pair(0, glm::vec2(11, 7))},
    {std::make_pair(PAINTING_6R_ZN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_6R_ZN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_7T_ZN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_ZN, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_ZN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_ZN, ZNEG), std::make_pair(0, glm::vec2(11, 5))},
    {std::make_pair(PAINTING_7T_ZN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7T_ZN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(PAINTING_7B_ZN, XPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_ZN, XNEG), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_ZN, ZPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_ZN, ZNEG), std::make_pair(0, glm::vec2(11, 3))},
    {std::make_pair(PAINTING_7B_ZN, YPOS), std::make_pair(0, glm::vec2(13, 10))},
    {std::make_pair(PAINTING_7B_ZN, YNEG), std::make_pair(0, glm::vec2(13, 10))},

    {std::make_pair(BONSAI_TREE, XPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(BONSAI_TREE, XNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(BONSAI_TREE, ZPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(BONSAI_TREE, ZNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(BONSAI_TREE, YPOS), std::make_pair(0, glm::vec2(0, 2))},
    {std::make_pair(BONSAI_TREE, YNEG), std::make_pair(0, glm::vec2(0, 3))},
    {std::make_pair(BONSAI_TREE, XPOS_ZPOS), std::make_pair(0, glm::vec2(0, 5))},
    {std::make_pair(BONSAI_TREE, XPOS_ZNEG), std::make_pair(0, glm::vec2(0, 5))},
    {std::make_pair(BONSAI_TREE, XNEG_ZPOS), std::make_pair(0, glm::vec2(0, 5))},
    {std::make_pair(BONSAI_TREE, XNEG_ZNEG), std::make_pair(0, glm::vec2(0, 5))},

    {std::make_pair(MAGNOLIA_IKEBANA, XPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(MAGNOLIA_IKEBANA, XNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(MAGNOLIA_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(MAGNOLIA_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(MAGNOLIA_IKEBANA, YPOS), std::make_pair(0, glm::vec2(0, 2))},
    {std::make_pair(MAGNOLIA_IKEBANA, YNEG), std::make_pair(0, glm::vec2(0, 3))},
    {std::make_pair(MAGNOLIA_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(0, 4))},
    {std::make_pair(MAGNOLIA_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(0, 4))},
    {std::make_pair(MAGNOLIA_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(0, 4))},
    {std::make_pair(MAGNOLIA_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(0, 4))},

    {std::make_pair(LOTUS_IKEBANA, XPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(LOTUS_IKEBANA, XNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(LOTUS_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(LOTUS_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(0, 1))},
    {std::make_pair(LOTUS_IKEBANA, YPOS), std::make_pair(0, glm::vec2(0, 2))},
    {std::make_pair(LOTUS_IKEBANA, YNEG), std::make_pair(0, glm::vec2(0, 3))},
    {std::make_pair(LOTUS_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 5))},
    {std::make_pair(LOTUS_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(3, 5))},
    {std::make_pair(LOTUS_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(3, 5))},
    {std::make_pair(LOTUS_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(3, 5))},

    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XPOS), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XNEG), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, YPOS), std::make_pair(0, glm::vec2(1, 2))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, YNEG), std::make_pair(0, glm::vec2(1, 3))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(4, 4))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(4, 4))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(4, 4))},
    {std::make_pair(GREEN_HYDRANGEA_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(4, 4))},

    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XPOS), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XNEG), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(1, 1))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, YPOS), std::make_pair(0, glm::vec2(1, 2))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, YNEG), std::make_pair(0, glm::vec2(1, 3))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(5, 5))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(5, 5))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(5, 5))},
    {std::make_pair(CHRYSANTHEMUM_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(5, 5))},

    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, YPOS), std::make_pair(0, glm::vec2(2, 2))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, YNEG), std::make_pair(0, glm::vec2(2, 3))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(1, 5))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(1, 5))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(1, 5))},
    {std::make_pair(CHERRY_BLOSSOM_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(1, 5))},

    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, YPOS), std::make_pair(0, glm::vec2(2, 2))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, YNEG), std::make_pair(0, glm::vec2(2, 3))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 4))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(3, 4))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(3, 4))},
    {std::make_pair(BLUE_HYDRANGEA_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(3, 4))},
    {std::make_pair(TULIP_IKEBANA, XPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(TULIP_IKEBANA, XNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(TULIP_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(TULIP_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(TULIP_IKEBANA, YPOS), std::make_pair(0, glm::vec2(2, 2))},
    {std::make_pair(TULIP_IKEBANA, YNEG), std::make_pair(0, glm::vec2(2, 3))},
    {std::make_pair(TULIP_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(6, 5))},
    {std::make_pair(TULIP_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(6, 5))},
    {std::make_pair(TULIP_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(6, 5))},
    {std::make_pair(TULIP_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(6, 5))},

    {std::make_pair(DAFFODIL_IKEBANA, XPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(DAFFODIL_IKEBANA, XNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(DAFFODIL_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(DAFFODIL_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(2, 1))},
    {std::make_pair(DAFFODIL_IKEBANA, YPOS), std::make_pair(0, glm::vec2(2, 2))},
    {std::make_pair(DAFFODIL_IKEBANA, YNEG), std::make_pair(0, glm::vec2(2, 3))},
    {std::make_pair(DAFFODIL_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(6, 4))},
    {std::make_pair(DAFFODIL_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(6, 4))},
    {std::make_pair(DAFFODIL_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(6, 4))},
    {std::make_pair(DAFFODIL_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(6, 4))},

    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, YPOS), std::make_pair(0, glm::vec2(3, 2))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, YNEG), std::make_pair(0, glm::vec2(3, 3))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(2, 5))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(2, 5))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(2, 5))},
    {std::make_pair(PLUM_BLOSSOM_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(2, 5))},

    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, YPOS), std::make_pair(0, glm::vec2(3, 2))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, YNEG), std::make_pair(0, glm::vec2(3, 3))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(1, 4))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(1, 4))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(1, 4))},
    {std::make_pair(MAGNOLIA_BUD_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(1, 4))},

    {std::make_pair(POPPY_IKEBANA, XPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(POPPY_IKEBANA, XNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(POPPY_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(POPPY_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(POPPY_IKEBANA, YPOS), std::make_pair(0, glm::vec2(3, 2))},
    {std::make_pair(POPPY_IKEBANA, YNEG), std::make_pair(0, glm::vec2(3, 3))},
    {std::make_pair(POPPY_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(2, 4))},
    {std::make_pair(POPPY_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(2, 4))},
    {std::make_pair(POPPY_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(2, 4))},
    {std::make_pair(POPPY_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(2, 4))},

    {std::make_pair(MAPLE_IKEBANA, XPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAPLE_IKEBANA, XNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAPLE_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAPLE_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(MAPLE_IKEBANA, YPOS), std::make_pair(0, glm::vec2(3, 2))},
    {std::make_pair(MAPLE_IKEBANA, YNEG), std::make_pair(0, glm::vec2(3, 3))},
    {std::make_pair(MAPLE_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(4, 5))},
    {std::make_pair(MAPLE_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(4, 5))},
    {std::make_pair(MAPLE_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(4, 5))},
    {std::make_pair(MAPLE_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(4, 5))},

    {std::make_pair(ONCIDIUM_IKEBANA, XPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(ONCIDIUM_IKEBANA, XNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(ONCIDIUM_IKEBANA, ZPOS), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(ONCIDIUM_IKEBANA, ZNEG), std::make_pair(0, glm::vec2(3, 1))},
    {std::make_pair(ONCIDIUM_IKEBANA, YPOS), std::make_pair(0, glm::vec2(3, 2))},
    {std::make_pair(ONCIDIUM_IKEBANA, YNEG), std::make_pair(0, glm::vec2(3, 3))},
    {std::make_pair(ONCIDIUM_IKEBANA, XPOS_ZPOS), std::make_pair(0, glm::vec2(5, 4))},
    {std::make_pair(ONCIDIUM_IKEBANA, XPOS_ZNEG), std::make_pair(0, glm::vec2(5, 4))},
    {std::make_pair(ONCIDIUM_IKEBANA, XNEG_ZPOS), std::make_pair(0, glm::vec2(5, 4))},
    {std::make_pair(ONCIDIUM_IKEBANA, XNEG_ZNEG), std::make_pair(0, glm::vec2(5, 4))},

    {std::make_pair(GHOST_LILY, XPOS_ZPOS), std::make_pair(4, glm::vec2(2, 9))},
    {std::make_pair(GHOST_LILY, XPOS_ZNEG), std::make_pair(4, glm::vec2(2, 9))},
    {std::make_pair(GHOST_LILY, XNEG_ZPOS), std::make_pair(4, glm::vec2(2, 9))},
    {std::make_pair(GHOST_LILY, XNEG_ZNEG), std::make_pair(4, glm::vec2(2, 9))},

    {std::make_pair(GHOST_WEED, XPOS_ZPOS), std::make_pair(4, glm::vec2(2, 10))},
    {std::make_pair(GHOST_WEED, XPOS_ZNEG), std::make_pair(4, glm::vec2(2, 10))},
    {std::make_pair(GHOST_WEED, XNEG_ZPOS), std::make_pair(4, glm::vec2(2, 10))},
    {std::make_pair(GHOST_WEED, XNEG_ZNEG), std::make_pair(4, glm::vec2(2, 10))},

    {std::make_pair(CORAL_1, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 9))},
    {std::make_pair(CORAL_1, XPOS_ZNEG), std::make_pair(0, glm::vec2(3, 9))},
    {std::make_pair(CORAL_1, XNEG_ZPOS), std::make_pair(0, glm::vec2(3, 9))},
    {std::make_pair(CORAL_1, XNEG_ZNEG), std::make_pair(0, glm::vec2(3, 9))},

    {std::make_pair(CORAL_2, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 10))},
    {std::make_pair(CORAL_2, XPOS_ZNEG), std::make_pair(0, glm::vec2(3, 10))},
    {std::make_pair(CORAL_2, XNEG_ZPOS), std::make_pair(0, glm::vec2(3, 10))},
    {std::make_pair(CORAL_2, XNEG_ZNEG), std::make_pair(0, glm::vec2(3, 10))},

    {std::make_pair(CORAL_3, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 11))},
    {std::make_pair(CORAL_3, XPOS_ZNEG), std::make_pair(0, glm::vec2(3, 11))},
    {std::make_pair(CORAL_3, XNEG_ZPOS), std::make_pair(0, glm::vec2(3, 11))},
    {std::make_pair(CORAL_3, XNEG_ZNEG), std::make_pair(0, glm::vec2(3, 11))},

    {std::make_pair(CORAL_4, XPOS_ZPOS), std::make_pair(0, glm::vec2(3, 12))},
    {std::make_pair(CORAL_4, XPOS_ZNEG), std::make_pair(0, glm::vec2(3, 12))},
    {std::make_pair(CORAL_4, XNEG_ZPOS), std::make_pair(0, glm::vec2(3, 12))},
    {std::make_pair(CORAL_4, XNEG_ZNEG), std::make_pair(0, glm::vec2(3, 12))},

    {std::make_pair(KELP_1, XPOS_ZPOS), std::make_pair(0, glm::vec2(4, 9))},
    {std::make_pair(KELP_1, XPOS_ZNEG), std::make_pair(0, glm::vec2(4, 9))},
    {std::make_pair(KELP_1, XNEG_ZPOS), std::make_pair(0, glm::vec2(4, 9))},
    {std::make_pair(KELP_1, XNEG_ZNEG), std::make_pair(0, glm::vec2(4, 9))},

    {std::make_pair(KELP_2, XPOS_ZPOS), std::make_pair(0, glm::vec2(4, 10))},
    {std::make_pair(KELP_2, XPOS_ZNEG), std::make_pair(0, glm::vec2(4, 10))},
    {std::make_pair(KELP_2, XNEG_ZPOS), std::make_pair(0, glm::vec2(4, 10))},
    {std::make_pair(KELP_2, XNEG_ZNEG), std::make_pair(0, glm::vec2(4, 10))},

    {std::make_pair(SEA_GRASS, XPOS), std::make_pair(0, glm::vec2(5, 9))},
    {std::make_pair(SEA_GRASS, XNEG), std::make_pair(0, glm::vec2(5, 9))},
    {std::make_pair(SEA_GRASS, ZPOS), std::make_pair(0, glm::vec2(5, 9))},
    {std::make_pair(SEA_GRASS, ZNEG), std::make_pair(0, glm::vec2(5, 9))}
};

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
    DirectionVector(ZNEG, glm::ivec3(0, 0, -1))
};

const static std::vector<DirectionVector> planeDirIter = {
    DirectionVector(YPOS, glm::ivec3(0, 1, 0)),
    DirectionVector(YNEG, glm::ivec3(0, -1, 0))
};

const static std::vector<DirectionVector> cross2DirIter = {
    DirectionVector(XPOS_ZPOS, glm::ivec3(1, 0, 1)),
    DirectionVector(XNEG_ZNEG, glm::ivec3(-1, 0, -1)),
    DirectionVector(XPOS_ZNEG, glm::ivec3(1, 0, -1)),
    DirectionVector(XNEG_ZPOS, glm::ivec3(-1, 0, 1))
};

const static std::vector<DirectionVector> cross4DirIter = {
    DirectionVector(XPOS, glm::ivec3(1, 0, 0)),
    DirectionVector(XNEG, glm::ivec3(-1, 0, 0)),
    DirectionVector(ZPOS, glm::ivec3(0, 0, 1)),
    DirectionVector(ZNEG, glm::ivec3(0, 0, -1))
};

const static std::unordered_set<BlockType, EnumHash> hPlane = {
    LILY_PAD, LOTUS_1, LOTUS_2, RICE_01, RICE_02
};

const static std::unordered_set<BlockType, EnumHash> cross2 = {
    TALL_GRASS, LOTUS_1, LOTUS_2,
    RICE_1, RICE_2, RICE_3, RICE_4, RICE_5, RICE_6, RICE_01, RICE_02,
    BAMBOO_2, BAMBOO_3,
    BONSAI_TREE, MAGNOLIA_IKEBANA, LOTUS_IKEBANA, GREEN_HYDRANGEA_IKEBANA, CHRYSANTHEMUM_IKEBANA,
    CHERRY_BLOSSOM_IKEBANA, BLUE_HYDRANGEA_IKEBANA, TULIP_IKEBANA, DAFFODIL_IKEBANA,
    PLUM_BLOSSOM_IKEBANA, MAGNOLIA_BUD_IKEBANA, POPPY_IKEBANA, MAPLE_IKEBANA, ONCIDIUM_IKEBANA,
    GHOST_LILY, GHOST_WEED,
    CORAL_1, CORAL_2, CORAL_3, CORAL_4,
    KELP_1, KELP_2
};

const static std::unordered_set<BlockType, EnumHash> cross4 = {
    WHEAT_1, WHEAT_2, WHEAT_3, WHEAT_4, WHEAT_5, WHEAT_6, WHEAT_7, WHEAT_8,
    SEA_GRASS
};

const static std::unordered_set<BlockType, EnumHash> partialX = {
    CEDAR_WINDOW_X, TEAK_WINDOW_X, CHERRY_WINDOW_X, MAPLE_WINDOW_X, PINE_WINDOW_X, WISTERIA_WINDOW_X,
    PAPER_LANTERN, WOOD_LANTERN,
    PAINTING_1_XP, PAINTING_2_XP, PAINTING_3_XP, PAINTING_4_XP, PAINTING_5_XP, PAINTING_6L_XP, PAINTING_6R_XP, PAINTING_7T_XP, PAINTING_7B_XP,
    PAINTING_1_XN, PAINTING_2_XN, PAINTING_3_XN, PAINTING_4_XN, PAINTING_5_XN, PAINTING_6L_XN, PAINTING_6R_XN, PAINTING_7T_XN, PAINTING_7B_XN,
    BAMBOO_1, BAMBOO_2, BAMBOO_3,
    BONSAI_TREE, MAGNOLIA_IKEBANA, LOTUS_IKEBANA, GREEN_HYDRANGEA_IKEBANA, CHRYSANTHEMUM_IKEBANA,
    CHERRY_BLOSSOM_IKEBANA, BLUE_HYDRANGEA_IKEBANA, TULIP_IKEBANA, DAFFODIL_IKEBANA,
    PLUM_BLOSSOM_IKEBANA, MAGNOLIA_BUD_IKEBANA, POPPY_IKEBANA, MAPLE_IKEBANA, ONCIDIUM_IKEBANA
};

const static std::unordered_set<BlockType, EnumHash> partialY = {
    WATER, LAVA,
    SNOW_1, SNOW_2, SNOW_3, SNOW_4, SNOW_5, SNOW_6, SNOW_7,
    CEDAR_PLANKS_1, TEAK_PLANKS_1, CHERRY_PLANKS_1, MAPLE_PLANKS_1, PINE_PLANKS_1, WISTERIA_PLANKS_1,
    CEDAR_PLANKS_2, TEAK_PLANKS_2, CHERRY_PLANKS_2, MAPLE_PLANKS_2, PINE_PLANKS_2, WISTERIA_PLANKS_2,
    ROOF_TILES_1, ROOF_TILES_2, STRAW_1, STRAW_2,
    TILLED_DIRT, PATH, IRRIGATED_SOIL,
    TATAMI_XL, TATAMI_XR, TATAMI_ZT, TATAMI_ZB,
    PAPER_LANTERN,
    BONSAI_TREE, MAGNOLIA_IKEBANA, LOTUS_IKEBANA, GREEN_HYDRANGEA_IKEBANA, CHRYSANTHEMUM_IKEBANA,
    CHERRY_BLOSSOM_IKEBANA, BLUE_HYDRANGEA_IKEBANA, TULIP_IKEBANA, DAFFODIL_IKEBANA,
    PLUM_BLOSSOM_IKEBANA, MAGNOLIA_BUD_IKEBANA, POPPY_IKEBANA, MAPLE_IKEBANA, ONCIDIUM_IKEBANA,
    CLOTH_1, CLOTH_2, CLOTH_3, CLOTH_4, CLOTH_5, CLOTH_6, CLOTH_7
};

const static std::unordered_set<BlockType, EnumHash> partialZ = {
    CEDAR_WINDOW_Z, TEAK_WINDOW_Z, CHERRY_WINDOW_Z, MAPLE_WINDOW_Z, PINE_WINDOW_Z, WISTERIA_WINDOW_Z,
    PAPER_LANTERN, WOOD_LANTERN,
    PAINTING_1_ZP, PAINTING_2_ZP, PAINTING_3_ZP, PAINTING_4_ZP, PAINTING_5_ZP, PAINTING_6L_ZP, PAINTING_6R_ZP, PAINTING_7T_ZP, PAINTING_7B_ZP,
    PAINTING_1_ZN, PAINTING_2_ZN, PAINTING_3_ZN, PAINTING_4_ZN, PAINTING_5_ZN, PAINTING_6L_ZN, PAINTING_6R_ZN, PAINTING_7T_ZN, PAINTING_7B_ZN,
    BAMBOO_1, BAMBOO_2, BAMBOO_3,
    BONSAI_TREE, MAGNOLIA_IKEBANA, LOTUS_IKEBANA, GREEN_HYDRANGEA_IKEBANA, CHRYSANTHEMUM_IKEBANA,
    CHERRY_BLOSSOM_IKEBANA, BLUE_HYDRANGEA_IKEBANA, TULIP_IKEBANA, DAFFODIL_IKEBANA,
    PLUM_BLOSSOM_IKEBANA, MAGNOLIA_BUD_IKEBANA, POPPY_IKEBANA, MAPLE_IKEBANA, ONCIDIUM_IKEBANA
};

const static std::unordered_set<BlockType, EnumHash> fullCube = {
    GRASS, DIRT, STONE, COBBLESTONE, MOSS_STONE, SAND, BEDROCK, SNOW_8, ICE,
    CEDAR_WOOD_X, TEAK_WOOD_X, CHERRY_WOOD_X, MAPLE_WOOD_X, PINE_WOOD_X, WISTERIA_WOOD_X,
    CEDAR_WOOD_Y, TEAK_WOOD_Y, CHERRY_WOOD_Y, MAPLE_WOOD_Y, PINE_WOOD_Y, WISTERIA_WOOD_Y,
    CEDAR_WOOD_Z, TEAK_WOOD_Z, CHERRY_WOOD_Z, MAPLE_WOOD_Z, PINE_WOOD_Z, WISTERIA_WOOD_Z,
    CEDAR_LEAVES, TEAK_LEAVES,
    CHERRY_BLOSSOMS_1, CHERRY_BLOSSOMS_2, CHERRY_BLOSSOMS_3, CHERRY_BLOSSOMS_4,
    MAPLE_LEAVES_1, MAPLE_LEAVES_2, MAPLE_LEAVES_3,
    PINE_LEAVES,
    WISTERIA_BLOSSOMS_1, WISTERIA_BLOSSOMS_2, WISTERIA_BLOSSOMS_3,
    CEDAR_PLANKS, TEAK_PLANKS, CHERRY_PLANKS, MAPLE_PLANKS, PINE_PLANKS, WISTERIA_PLANKS,
    RED_PAINTED_WOOD, BLACK_PAINTED_WOOD, PLASTER, ROOF_TILES, STRAW, CLOTH_8
};

const static std::unordered_set<BlockType, EnumHash> transparent = {
    WATER, ICE, TALL_GRASS,
    CEDAR_LEAVES, TEAK_LEAVES,
    CHERRY_BLOSSOMS_1, CHERRY_BLOSSOMS_2, CHERRY_BLOSSOMS_3, CHERRY_BLOSSOMS_4,
    MAPLE_LEAVES_1, MAPLE_LEAVES_2, MAPLE_LEAVES_3,
    PINE_LEAVES,
    WISTERIA_BLOSSOMS_1, WISTERIA_BLOSSOMS_2, WISTERIA_BLOSSOMS_3,
    CEDAR_WINDOW_X, TEAK_WINDOW_X, CHERRY_WINDOW_X, MAPLE_WINDOW_X, PINE_WINDOW_X, WISTERIA_WINDOW_X,
    CEDAR_WINDOW_Z, TEAK_WINDOW_Z, CHERRY_WINDOW_Z, MAPLE_WINDOW_Z, PINE_WINDOW_Z, WISTERIA_WINDOW_Z,
    LILY_PAD, LOTUS_1, LOTUS_2,
    WHEAT_1, WHEAT_2, WHEAT_3, WHEAT_4, WHEAT_5, WHEAT_6, WHEAT_7, WHEAT_8,
    RICE_1, RICE_2, RICE_3, RICE_4, RICE_5, RICE_6,
    GHOST_LILY, GHOST_WEED,
    CORAL_1, CORAL_2, CORAL_3, CORAL_4,
    KELP_1, KELP_2,
    SEA_GRASS
};

struct Vertex {
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec4 color;
    glm::vec4 uvCoords; // contains a second set of uv coords when overlaying another texture
    glm::vec4 blockType; // texture flag
    glm::vec4 biomeWts; // contains biome weightings at this xz coord

    Vertex(glm::vec4 p, glm::ivec3 n, BlockType b, Direction d, glm::vec2 uv, glm::vec4 bWts) {
        position = p;
        normal = glm::vec4(n, 1);
        biomeWts = bWts;
        if (btToUV.find(std::make_pair(b, d)) != btToUV.end()) {
            blockType = glm::vec4(btToUV.at(std::make_pair(b, d)).first, 0, 0, 0);


            // 32x32 (double) res
            if ((d == XPOS && (b == PAINTING_1_XP || b == PAINTING_2_XP || b == PAINTING_3_XP ||
                    b == PAINTING_6L_XP || b == PAINTING_6R_XP ||
                    b == PAINTING_7T_XP || b == PAINTING_7B_XP)) ||
                (d == XNEG && (b == PAINTING_1_XN || b == PAINTING_2_XN || b == PAINTING_3_XN ||
                    b == PAINTING_6L_XN || b == PAINTING_6R_XN ||
                    b == PAINTING_7T_XN || b == PAINTING_7B_XN)) ||
                (d == ZPOS && (b == PAINTING_1_ZP || b == PAINTING_2_ZP || b == PAINTING_3_ZP ||
                    b == PAINTING_6L_ZP || b == PAINTING_6R_ZP ||
                    b == PAINTING_7T_ZP || b == PAINTING_7B_ZP)) ||
                (d == ZNEG && (b == PAINTING_1_ZN || b == PAINTING_2_ZN || b == PAINTING_3_ZN ||
                    b == PAINTING_6L_ZN || b == PAINTING_6R_ZN ||
                    b == PAINTING_7T_ZN || b == PAINTING_7B_ZN))) {
                uv *= 2;

                if (d == XNEG || d == ZNEG) {
                    if (uv.x == 0) {
                        uv.x = 2;
                    } else if (uv.x == 2) {
                        uv.x = 0;
                    }
                }

            // 48x48 (triple) res
            } else if ((d == XPOS && (b == PAINTING_4_XP || b == PAINTING_5_XP)) ||
                        (d == XNEG && (b == PAINTING_4_XN || b == PAINTING_5_XN)) ||
                        (d == ZPOS && (b == PAINTING_4_ZP || b == PAINTING_5_ZP)) ||
                        (d == ZNEG && (b == PAINTING_4_ZN || b == PAINTING_5_ZN))) {
                uv *= 3;
            }

            // rotate texture
            if ((b == CEDAR_WOOD_X ||
                b == TEAK_WOOD_X ||
                b == CHERRY_WOOD_X ||
                b == MAPLE_WOOD_X ||
                b == PINE_WOOD_X ||
                b == WISTERIA_WOOD_X)
                && d != YPOS && d != YNEG) {

                float x = uv.x;
                float y = uv.y;
                uv.x = y;
                uv.y = x;
            }

            if (b == CEDAR_WOOD_Z ||
                b == TEAK_WOOD_Z ||
                b == CHERRY_WOOD_Z ||
                b == MAPLE_WOOD_Z ||
                b == PINE_WOOD_Z ||
                b == WISTERIA_WOOD_Z) {

                float x = uv.x;
                float y = uv.y;
                uv.x = y;
                uv.y = x;
            }

            uvCoords = glm::vec4(uv + btToUV.at(std::make_pair(b, d)).second, 0, 0);
            uvCoords /= 16.f;
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
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;

    std::array<glm::vec4, 256> m_biomes;
    static bool isInBounds(glm::ivec3);
    BlockType getAdjBlockType(Direction, glm::ivec3);

    // coords given in block space
    static void createFaceVBOData(std::vector<Vertex>&, float, float, float, DirectionVector, BlockType, glm::vec4);

    void redistributeVertexData(std::vector<glm::vec4>, std::vector<GLuint>, std::vector<glm::vec4>, std::vector<GLuint>);

public:
    Chunk(OpenGLContext* context);
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);

    // mountains = 0, hills = 1, forest = 2, islands = 3, caves = 4
    glm::vec4 getBiomeAt(unsigned int x, unsigned int z) const;
    glm::vec4 getBiomeAt(int x, int z) const;
    void setBiomeAt(unsigned int x, unsigned int z, glm::vec4 b);

    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    static bool isHPlane(BlockType);
    static bool isCross2(BlockType);
    static bool isCross4(BlockType);
    static bool isPartialX(BlockType);
    static bool isPartialY(BlockType);
    static bool isPartialZ(BlockType);
    static bool isFullCube(BlockType);
    static bool isTransparent(BlockType);

    bool isVisible(int x, int y, int z, BlockType bt); // checks whether block is enclosed on all sides
    bool isVisible(int x, int y, int z, DirectionVector dv, BlockType bt); // checks whether a x/y/z face is visible

    void createVBOdata() override;
    GLenum drawMode() override {
        return GL_TRIANGLES;
    }


};
