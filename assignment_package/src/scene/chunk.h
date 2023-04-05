#pragma once
#include "drawable.h"
#include "smartpointerhelp.h"
#include <array>
#include <unordered_map>
#include <cstddef>


//using namespace std;

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, COBBLESTONE, MOSS_STONE, SAND,
    TALL_GRASS, WATER, LAVA, BEDROCK, SNOW_1, SNOW_2, SNOW_3, SNOW_4, SNOW_5, SNOW_6, SNOW_7, SNOW_8, ICE,
    RED_PAINTED_WOOD, BLACK_PAINTED_WOOD, PLASTER, ROOF_TILES, STRAW,
    CEDAR_WOOD, TEAK_WOOD, CHERRY_WOOD, MAPLE_WOOD, PINE_WOOD, WISTERIA_WOOD,
    CEDAR_LEAVES, TEAK_LEAVES, CHERRY_BLOSSOMS_1, CHERRY_BLOSSOMS_2, CHERRY_BLOSSOMS_3, CHERRY_BLOSSOMS_4,
    MAPLE_LEAVES_1, MAPLE_LEAVES_2, MAPLE_LEAVES_3,
    PINE_LEAVES, WISTERIA_BLOSSOMS_1, WISTERIA_BLOSSOMS_2, WISTERIA_BLOSSOMS_3,
    CEDAR_PLANKS, TEAK_PLANKS, CHERRY_PLANKS, MAPLE_PLANKS, PINE_PLANKS, WISTERIA_PLANKS,
    CEDAR_WINDOW, TEAK_WINDOW, CHERRY_WINDOW, MAPLE_WINDOW, PINE_WINDOW, WISTERIA_WINDOW,
    LILY_PAD, LOTUS, TILLED_DIRT, WHEAT, RICE, BAMBOO,
    TATAMI, PAPER_LANTERN, WOOD_LANTERN,
    PAINTING, PAINTING_T, PAINTING_B, PAINTING_L, PAINTING_R,
    BONSAI_TREE, IKEBANA_1, IKEBANA_2, IKEBANA_3, TEA_KETTLE,
    GHOST_LILY, CORAL, KELP
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
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
    {ZNEG, ZPOS}
};

const static std::map<BlockType, glm::vec2> btToUV {
    {GRASS, glm::vec2(0, 11)},
    {DIRT, glm::vec2(0, 13)},
    {STONE, glm::vec2(1, 15)},
    {COBBLESTONE, glm::vec2(1, 15)},
    {MOSS_STONE, glm::vec2(3, 15)},
    {SAND, glm::vec2(0, 14)},
    {TALL_GRASS, glm::vec2(0, 10)},
    {WATER, glm::vec2(13, 3)},
    {LAVA, glm::vec2(13, 1)},
    {BEDROCK, glm::vec2(2, 15)},
    {SNOW_1, glm::vec2(1, 14)},
    {SNOW_2, glm::vec2(1, 14)},
    {SNOW_3, glm::vec2(1, 14)},
    {SNOW_4, glm::vec2(1, 14)},
    {SNOW_5, glm::vec2(1, 14)},
    {SNOW_6, glm::vec2(1, 14)},
    {SNOW_7, glm::vec2(1, 14)},
    {ICE, glm::vec2(2, 14)},
    {RED_PAINTED_WOOD, glm::vec2(4, 15)},
    {BLACK_PAINTED_WOOD, glm::vec2(4, 15)},
    {PLASTER, glm::vec2(11, 12)},
    {ROOF_TILES, glm::vec2(11, 15)},
    {STRAW, glm::vec2(11, 13)},
    {CEDAR_WOOD, glm::vec2(5, 12)},
    {TEAK_WOOD, glm::vec2(6, 12)},
    {CHERRY_WOOD, glm::vec2(7, 12)},
    {MAPLE_WOOD, glm::vec2(8, 12)},
    {PINE_WOOD, glm::vec2(9, 12)},
    {WISTERIA_WOOD, glm::vec2(10, 12)},
    {CEDAR_LEAVES, glm::vec2(5, 11)},
    {TEAK_LEAVES, glm::vec2(6, 11)},
    {CHERRY_BLOSSOMS_1, glm::vec2(7, 8)},
    {CHERRY_BLOSSOMS_2, glm::vec2(7, 9)},
    {CHERRY_BLOSSOMS_3, glm::vec2(7, 10)},
    {CHERRY_BLOSSOMS_4, glm::vec2(7, 11)},
    {MAPLE_LEAVES_1, glm::vec2(8, 9)},
    {MAPLE_LEAVES_2, glm::vec2(8, 10)},
    {MAPLE_LEAVES_3, glm::vec2(8, 11)},
    {PINE_LEAVES, glm::vec2(9, 11)},
    {WISTERIA_BLOSSOMS_1, glm::vec2(10, 9)},
    {WISTERIA_BLOSSOMS_2, glm::vec2(10, 10)},
    {WISTERIA_BLOSSOMS_3, glm::vec2(10, 11)},
    {CEDAR_PLANKS, glm::vec2(5, 15)},
    {TEAK_PLANKS, glm::vec2(6, 15)},
    {CHERRY_PLANKS, glm::vec2(7, 15)},
    {MAPLE_PLANKS, glm::vec2(8, 15)},
    {PINE_PLANKS, glm::vec2(9, 15)},
    {WISTERIA_PLANKS, glm::vec2(10, 15)},
    {CEDAR_WINDOW, glm::vec2(5, 14)},
    {TEAK_WINDOW, glm::vec2(6, 14)},
    {CHERRY_WINDOW, glm::vec2(7, 14)},
    {MAPLE_WINDOW, glm::vec2(8, 14)},
    {PINE_WINDOW, glm::vec2(9, 14)},
    {WISTERIA_WINDOW, glm::vec2(10, 14)},
    {LILY_PAD, glm::vec2(0, 9)},
    {LOTUS, glm::vec2(1, 9)},
    {TILLED_DIRT, glm::vec2(0, 8)},
    {WHEAT, glm::vec2(0, 7)},
    {RICE, glm::vec2(0, 6)},
    {BAMBOO, glm::vec2(12, 12)},
    {TATAMI, glm::vec2(11, 11)},
    {PAPER_LANTERN, glm::vec2(13, 14)},
    {WOOD_LANTERN, glm::vec2(13, 12)},
    {PAINTING, glm::vec2(13, 5)},
    {PAINTING_T, glm::vec2(12, 11)},
    {PAINTING_B, glm::vec2(12, 10)},
    {PAINTING_L, glm::vec2(14, 5)},
    {PAINTING_R, glm::vec2(15, 5)},
    {BONSAI_TREE, glm::vec2(1, 8)},
    {IKEBANA_1, glm::vec2(0, 5)},
    {IKEBANA_2, glm::vec2(0, 4)},
    {IKEBANA_3, glm::vec2(0, 3)},
    {TEA_KETTLE, glm::vec2(15, 15)},
    {GHOST_LILY, glm::vec2(2, 9)},
    {CORAL, glm::vec2(3, 9)},
    {KELP, glm::vec2(2, 8)}
};
const static std::map<BlockType, glm::vec3> btToBlockIdx;

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

struct Vertex {
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec4 color;
    glm::vec4 uvCoords;
    glm::vec4 blockType; // block idx, structure, structure details, biome

    Vertex(glm::vec4 p, glm::ivec3 n, BlockType b, glm::vec2 uv, int biome) {
        position = p;
        normal = glm::vec4(n, 1);

        uvCoords = glm::vec4(uv + btToUV[b], 0, 0);

        switch (b) {
            case GRASS:
                color = glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f;
                uvCoords.x += 0;
                uvCoords.y += 11;
                blockType = glm::vec4(0, 0, 1, biome);
            case DIRT:
                color = glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f;
                uvCoords.x += 0;
                uvCoords.y += 13;
                blockType = glm::vec4(1, 0, 0, biome);
                break;
            case STONE:
                color = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
                uvCoords.x += 1;
                uvCoords.y += 15;
                blockType = glm::vec4(2, 0, 0, biome);
                break;
            case COBBLESTONE:
                uvCoords.x += 0;
                uvCoords.y += 15;
                blockType = glm::vec4(3, 0, 0, biome);
            case MOSS_STONE:
                uvCoords.x += 3;
                uvCoords.y += 15;
                blockType = glm::vec4(4, 0, 0, biome);
            case SAND:
                color = glm::vec4(215.f, 195.f, 130.f, 255.f) / 255.f;
                uvCoords.x += 0;
                uvCoords.y += 14;
                blockType = glm::vec4(5, 0, 0, biome);
                break;
            case TALL_GRASS:
                uvCoords.x += 0;
                uvCoords.y += 10;
                blockType = glm::vec4(6, 3, 0, biome);
            case WATER:
                color = glm::vec4(0.f, 0.f, 0.75f, 1.f);
                uvCoords.x += 13;
                uvCoords.y += 3;
                blockType = glm::vec4(7, 6, 1, biome);
                break;
            case LAVA:
                uvCoords.x += 13;
                uvCoords.y += 1;
                blockType = glm::vec4(8, 6, 2, biome);
            case BEDROCK:
                uvCoords.x += 2;
                uvCoords.y += 15;
                blockType = glm::vec4(9, 0, 0, biome);
            case SNOW_1:
                color = glm::vec4(240.f, 245.f, 255.f, 255.f) / 255.f;
                uvCoords.x += 1;
                uvCoords.y += 14;
                blockType = glm::vec4(10, 1, 1, biome);
                break;
            case SNOW_2:
            case SNOW_3:
            case SNOW_4:
            case SNOW_5:
            case SNOW_6:
            case SNOW_7:
            case SNOW_8:
            case ICE:
                uvCoords.x += 2;
                uvCoords.y += 14;
                blockType = glm::vec4(11, 0, 0, biome);
                break;
            case RED_PAINTED_WOOD:
            case BLACK_PAINTED_WOOD:
            case PLASTER:
            case ROOF_TILES:
            case STRAW:
            case CEDAR_WOOD:
            case TEAK_WOOD:
            case CHERRY_WOOD:
            case MAPLE_WOOD:
            case PINE_WOOD:
            case WISTERIA_WOOD:
            case CEDAR_LEAVES:
            case TEAK_LEAVES:
            case CHERRY_BLOSSOMS_1:
            case CHERRY_BLOSSOMS_2:
            case CHERRY_BLOSSOMS_3:
            case CHERRY_BLOSSOMS_4:
            case MAPLE_LEAVES_1:
            case MAPLE_LEAVES_2:
            case MAPLE_LEAVES_3:
            case PINE_LEAVES:
            case WISTERIA_BLOSSOMS_1:
            case WISTERIA_BLOSSOMS_2:
            case WISTERIA_BLOSSOMS_3:
            case CEDAR_PLANKS:
            case TEAK_PLANKS:
            case CHERRY_PLANKS:
            case MAPLE_PLANKS:
            case PINE_PLANKS:
            case WISTERIA_PLANKS:
            case CEDAR_WINDOW:
            case TEAK_WINDOW:
            case CHERRY_WINDOW:
            case MAPLE_WINDOW:
            case PINE_WINDOW:
            case WISTERIA_WINDOW:
            case LILY_PAD:
            case LOTUS:
            case TILLED_DIRT:
            case WHEAT:
            case RICE:
            case BAMBOO:
            case TATAMI:
            case PAPER_LANTERN:
            case WOOD_LANTERN:
            case PAINTING:
            case PAINTING_T:
            case PAINTING_B:
            case PAINTING_L:
            case PAINTING_R:
            case BONSAI_TREE:
            case IKEBANA_1:
            case IKEBANA_2:
            case IKEBANA_3:
            case TEA_KETTLE:
            case GHOST_LILY:
            case CORAL:
            case KELP:
            default:
                color = glm::vec4(1.f, 0.f, 1.f, 1.f);
                break;
        }
        uvCoords /= 16.f;
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

    static void createFaceVBOData(std::vector<Vertex>&, int, int, int, DirectionVector, BlockType, int);

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

    void createVBOdata() override;
    GLenum drawMode() override {
        return GL_TRIANGLES;
    }


};
