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
    EMPTY, GRASS, DIRT, STONE, WATER
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

    Vertex(glm::vec4 p, glm::ivec3 n, BlockType b) {
        position = p;
        normal = glm::vec4(n, 1);

        switch (b) {
            case GRASS:
                color = glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f;
                break;
            case DIRT:
                color = glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f;
                break;
            case STONE:
                color = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
                break;
            case WATER:
                color = glm::vec4(0.f, 0.f, 0.75f, 1.f);
                break;
            default:
                color = glm::vec4(1.f, 0.f, 1.f, 1.f);
                break;
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

    static bool isInBounds(glm::ivec3);
    BlockType getAdjBlockType(Direction, glm::ivec3);

    static void createFaceVBOData(std::vector<Vertex>&, int, int, int, DirectionVector, BlockType);

    void redistributeVertexData(std::vector<glm::vec4>, std::vector<GLuint>);

public:
    Chunk(OpenGLContext* context);
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    void createVBOdata() override;
    GLenum drawMode() override {
        return GL_TRIANGLES;
    }

};
