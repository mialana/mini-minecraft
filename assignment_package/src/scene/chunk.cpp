#include "chunk.h"
#include <iostream>;


Chunk::Chunk(OpenGLContext* context) : Drawable(context), m_blocks(), m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

glm::vec4 Chunk::getBiomeAt(unsigned int x, unsigned int z) const {
    return m_biomes.at(x + 16 * z);
}

glm::vec4 Chunk::getBiomeAt(int x, int z) const {
    return getBiomeAt(static_cast<unsigned int>(x), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
}

void Chunk::setBiomeAt(unsigned int x, unsigned int z, glm::vec4 b) {
    m_biomes.at(x + 16 * z) = b;
}

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

bool Chunk::isInBounds(glm::ivec3 pos) {
    return (pos.x >= 0 && pos.x < 16 &&
            pos.y >= 0 && pos.y < 256 &&
            pos.z >= 0 && pos.z < 16);
}

bool Chunk::isHPlane(BlockType bt) {
    return hPlane.find(bt) != hPlane.end();
}
bool Chunk::isCross2(BlockType bt) {
    return cross2.find(bt) != cross2.end();
}
bool Chunk::isCross4(BlockType bt) {
    return cross4.find(bt) != cross4.end();
}
bool Chunk::isPartialX(BlockType bt) {
    return partialX.find(bt) != partialX.end();
}
bool Chunk::isPartialY(BlockType bt) {
    return partialY.find(bt) != partialY.end();
}
bool Chunk::isPartialZ(BlockType bt) {
    return partialZ.find(bt) != partialZ.end();
}
bool Chunk::isFullCube(BlockType bt) {
    return fullCube.find(bt) != fullCube.end();
}
bool Chunk::isTransparent(BlockType bt) {
    return transparent.find(bt) != transparent.end();
}
bool Chunk::isVisible(int x, int y, int z, BlockType bt) {
    for (const DirectionVector& dv : directionIter) {
        glm::ivec3 adjBlockPos = glm::ivec3(x, y, z) + dv.vec;
        bool inSameChunk = Chunk::isInBounds(adjBlockPos);

        BlockType adjBlockType;

        if (inSameChunk) {
            adjBlockType = this->getBlockAt(adjBlockPos.x, adjBlockPos.y, adjBlockPos.z);
        } else {
            adjBlockType = this->getAdjBlockType(dv.dir, adjBlockPos);
        }
        if ((!isFullCube(adjBlockType) || isTransparent(adjBlockType)) && adjBlockType != bt) {
            return true;
        }
    }
    return false;
}
bool Chunk::isVisible(int x, int y, int z, DirectionVector dv, BlockType bt) {
    glm::ivec3 adjBlockPos = glm::ivec3(x, y, z) + dv.vec;
    bool inSameChunk = Chunk::isInBounds(adjBlockPos);
    Direction d = dv.dir;

    BlockType adjBlockType;


    if (inSameChunk) {
        adjBlockType = this->getBlockAt(adjBlockPos.x, adjBlockPos.y, adjBlockPos.z);
    } else {
        adjBlockType = this->getAdjBlockType(dv.dir, adjBlockPos);
    }

    // if block is completely enclosed by non-transparent blocks
    if (!isVisible(x, y, z, bt)) {
        return false;
    }

    if (isFullCube(adjBlockType) && !isTransparent(adjBlockType)) {
        return false;
    }

    if ((bt == WATER || bt == LAVA || bt == ICE) &&
            (bt == adjBlockType ||
            ((isCross4(adjBlockType) || isCross2(adjBlockType)) && d != YPOS))) {
        return false;
    }

    if (isPartialX(bt) && !isPartialY(bt) && !isPartialZ(bt) && bt == adjBlockType) {
        return false;
    }

    if (isPartialY(bt) && !isPartialX(bt) && !isPartialZ(bt) && bt == adjBlockType) {
        return false;
    }

    if (isPartialZ(bt) && !isPartialX(bt) && !isPartialY(bt) && bt == adjBlockType) {
        return false;
    }

    if (adjBlockType == EMPTY) {
        return true;
    }

    // if the block adjacent to this face is EMPTY or transparent and is of a different type
    if (isTransparent(adjBlockType) && adjBlockType != bt) {
        return true;
    }

    if (isHPlane(adjBlockType) || isCross2(adjBlockType) || isCross4(adjBlockType)) {
        return true;
    }

    // if not a cube or partial block
    if (isHPlane(bt) || isCross2(bt) || isCross4(bt)) {
        return true;
    }

    // if this face or adjacent block face doesn't reach the bounds of a full block
    if (((d == XPOS || d == XNEG) && (isPartialX(bt) || isPartialX(adjBlockType) || isPartialY(adjBlockType) || isPartialZ(adjBlockType))) ||
            ((d == YPOS) && (isPartialY(bt) || isPartialX(adjBlockType) || isPartialZ(adjBlockType))) ||
            ((d == YNEG) && (isPartialX(adjBlockType) || isPartialY(adjBlockType) || isPartialZ(adjBlockType))) ||
            ((d == ZPOS || d == ZNEG) && (isPartialZ(bt) || isPartialX(adjBlockType) || isPartialY(adjBlockType) || isPartialZ(adjBlockType)))) {
        return true;
    }

    return false;
}

BlockType Chunk::getAdjBlockType(Direction d, glm::ivec3 pos) {
    Chunk* neighborChunk = m_neighbors[d];

    if (neighborChunk != nullptr) {
        switch (d) {
            case XPOS:
                return neighborChunk->getBlockAt(0, pos.y, pos.z);
            case XNEG:
                return neighborChunk->getBlockAt(15, pos.y, pos.z);
            case ZPOS:
                return neighborChunk->getBlockAt(pos.x, pos.y, 0);
            case ZNEG:
                return neighborChunk->getBlockAt(pos.x, pos.y, 15);
            default:
                return EMPTY;
        }
    }
    return EMPTY;
}

void Chunk::createFaceVBOData(std::vector<Vertex>& verts, float currX, float currY, float currZ, DirectionVector dirVec, BlockType bt, glm::vec4 bWts) {
    Direction d = dirVec.dir;

    float offsetXPOS = 1.f;
    float offsetXNEG = 0.f;
    float offsetYPOS = 1.f;
    float offsetYNEG = 0.f;
    float offsetZPOS = 1.f;
    float offsetZNEG = 0.f;
    float offsetDiag = (std::sqrt(0.5) - 0.5) / std::sqrt(2);
    bool keepEdges = false; // keep portions of face outside intersection ex: wheat vs lantern

    float x1 = currX;
    float x2 = currX;
    float y1 = currY;
    float y2 = currY;
    float z1 = currZ;
    float z2 = currZ;

    switch(bt) {
        case LILY_PAD: case LOTUS_1: case LOTUS_2:
            offsetYPOS = 0.005f;
            break;
        case WHEAT_1: case WHEAT_2: case WHEAT_3: case WHEAT_4: case WHEAT_5: case WHEAT_6: case WHEAT_7: case WHEAT_8: case SEA_GRASS:
            offsetXPOS = 0.75f;
            offsetXNEG = 0.25f;
            offsetZPOS = 0.75f;
            offsetZNEG = 0.25f;
            keepEdges = true;
            break;
        case SNOW_1: case TATAMI:
            offsetYPOS = 0.125;
            break;
        case SNOW_2:
            offsetYPOS = 0.25;
            break;
        case SNOW_3:
            offsetYPOS = 0.375;
            break;
        case SNOW_4: case ROOF_TILES_1: case STRAW_1:
            offsetYPOS = 0.5;
            break;
        case ROOF_TILES_2: case STRAW_2:
            offsetYNEG = 0.5;
            break;
        case SNOW_5:
            offsetYPOS = 0.625;
            break;
        case SNOW_6:
            offsetYPOS = 0.75;
            break;
        case SNOW_7:
            offsetYPOS = 0.875;
            break;
        case CEDAR_WINDOW: case TEAK_WINDOW: case CHERRY_WINDOW: case MAPLE_WINDOW: case PINE_WINDOW: case WISTERIA_WINDOW:
            offsetXPOS = 0.5625; // may be offsetX or offsetZ
            offsetXNEG = 0.4375;
            break;
        case TILLED_DIRT: case PATH:
            offsetYPOS = 0.9375;
            break;
        case BAMBOO_1: case BAMBOO_2: case BAMBOO_3:
            offsetXPOS = 0.5625;
            offsetXNEG = 0.4375;
            offsetZPOS = 0.5625;
            offsetZNEG = 0.4375;
            break;
        case PAPER_LANTERN:
            offsetXPOS = 0.8125;
            offsetXNEG = 0.1875;
            offsetZPOS = 0.8125;
            offsetZNEG = 0.1875;
            offsetYPOS = 0.75;
            break;
        case WOOD_LANTERN:
            offsetXPOS = 0.9375;
            offsetXNEG = 0.0625;
            offsetZPOS = 0.9375;
            offsetZNEG = 0.0625;
            break;
        case PAINTING_1: case PAINTING_2: case PAINTING_3: case PAINTING_4: case PAINTING_5:
        case PAINTING_7T: case PAINTING_7B: case PAINTING_6L: case PAINTING_6R:
            offsetXPOS = 0.0625;

//            // or
//            offsetYPOS = 0.0625;
//            // or
//            offsetXNEG = 0.9375;
//            // or
//            offsetYNEG = 0.9375;
            break;
        case BONSAI_TREE: case MAGNOLIA_IKEBANA: case LOTUS_IKEBANA:
            offsetXPOS = 0.9375;
            offsetXNEG = 0.0625;
            offsetZPOS = 0.9375;
            offsetZNEG = 0.0625;
            offsetYPOS = 0.125;
            break;
        case GREEN_HYDRANGEA_IKEBANA: case CHRYSANTHEMUM_IKEBANA:
            offsetXPOS = 0.75;
            offsetXNEG = 0.25;
            offsetZPOS = 0.75;
            offsetZNEG = 0.25;
            offsetYPOS = 0.25;
            break;
        case CHERRY_BLOSSOM_IKEBANA: case BLUE_HYDRANGEA_IKEBANA: case TULIP_IKEBANA: case DAFFODIL_IKEBANA:
            offsetXPOS = 0.6875;
            offsetXNEG = 0.3125;
            offsetZPOS = 0.6875;
            offsetZNEG = 0.3125;
            offsetYPOS = 0.375;
            break;
        case PLUM_BLOSSOM_IKEBANA: case MAGNOLIA_BUD_IKEBANA: case POPPY_IKEBANA: case MAPLE_IKEBANA: case ONCIDIUM_IKEBANA:
            offsetXPOS = 0.5625;
            offsetXNEG = 0.4375;
            offsetZPOS = 0.5625;
            offsetZNEG = 0.4375;
            offsetYPOS = 0.5;
            break;
        default:
            break;
    }

    switch (d) {
        case XPOS: case XNEG:
            if (!keepEdges) {
                y1 += offsetYNEG;
                y2 += offsetYPOS;
                z1 += offsetZNEG;
                z2 += offsetZPOS;
            } else {
                y2 += 1.f;
                z2 += 1.f;
                offsetYNEG = 0.f;
                offsetYPOS = 1.f;
                offsetZNEG = 0.f;
                offsetZPOS = 1.f;
            }
            x1 = currX + offsetXNEG;
            x2 = currX + offsetXPOS;
            if (d == XNEG) {
                verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG, offsetYNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG, offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS, offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS, offsetYNEG), bWts));

                if (isCross4(bt)) {
                    verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG, offsetYNEG), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG, offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS, offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS, offsetYNEG), bWts));
                }
            } else {
                verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS, offsetYNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS, offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG, offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG, offsetYNEG), bWts));

                if (isCross4(bt)) {
                    verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS, offsetYNEG), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS, offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG, offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG, offsetYNEG), bWts));
                }
            }
            break;

        case YPOS: case YNEG:
            if (!keepEdges) {
                x1 += offsetXNEG;
                x2 += offsetXPOS;
                z1 += offsetZNEG;
                z2 += offsetZPOS;
            } else {
                x2 += 1.f;
                z2 += 1.f;
                offsetXNEG = 0.f;
                offsetXPOS = 1.f;
                offsetZNEG = 0.f;
                offsetZPOS = 1.f;
            }
            y1 = currY + offsetYNEG;
            y2 = currY + offsetYPOS;
            if (d == YNEG) {
                verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetZPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetZPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetZNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetZNEG), bWts));
            } else {
                verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetZNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetZNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetZPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetZPOS), bWts));
            }
            break;

        case ZPOS: case ZNEG:
            if (!keepEdges) {
                x1 += offsetXNEG;
                x2 += offsetXPOS;
                y1 += offsetYNEG;
                y2 += offsetYPOS;
            } else {
                x2 += 1.f;
                y2 += 1.f;
                offsetXNEG = 0.f;
                offsetXPOS = 1.f;
                offsetYNEG = 0.f;
                offsetYPOS = 1.f;
            }
            z1 = currZ + offsetZNEG;
            z2 = currZ + offsetZPOS;
            if (d == ZNEG) {
                verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetYNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetYNEG), bWts));

                if (isCross4(bt)) {
                    verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetYNEG), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetYNEG), bWts));
                }
            } else {
                verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetYNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetYNEG), bWts));

                if (isCross4(bt)) {
                    verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetYNEG), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetYNEG), bWts));
                }
            }
            break;
        case XPOS_ZPOS:
            x1 += offsetDiag;
            x2 += 1 - offsetDiag;
            y2 += 1;
            z1 += offsetDiag;
            z2 += 1 - offsetDiag;

            verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(0, 0), bWts));
            verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(1, 0), bWts));
            verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(1, 1), bWts));
            verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(0, 1), bWts));


            break;
        case XNEG_ZNEG:
            x1 += offsetDiag;
            x2 += 1 - offsetDiag;
            y2 += 1;
            z1 += offsetDiag;
            z2 += 1 - offsetDiag;

            verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(0, 0), bWts));
            verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(1, 0), bWts));
            verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(1, 1), bWts));
            verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(0, 1), bWts));

            break;
        case XPOS_ZNEG:
            x1 += offsetDiag;
            x2 += 1 - offsetDiag;
            y2 += 1;
            z1 += offsetDiag;
            z2 += 1 - offsetDiag;

            verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(0, 0), bWts));
            verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(1, 0), bWts));
            verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(1, 1), bWts));
            verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(0, 1), bWts));

            break;
        case XNEG_ZPOS:
            x1 += offsetDiag;
            x2 += 1 - offsetDiag;
            y2 += 1;
            z1 += offsetDiag;
            z2 += 1 - offsetDiag;

            verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(0, 0), bWts));
            verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(1, 0), bWts));
            verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(1, 1), bWts));
            verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(0, 1), bWts));

            break;
    }
}

void Chunk::createVBOdata() {
    // opaque
    std::vector<GLuint> oIndices = std::vector<GLuint>();
    std::vector<glm::vec4> oVertData = std::vector<glm::vec4>();
    // transparent
    std::vector<GLuint> tIndices = std::vector<GLuint>();
    std::vector<glm::vec4> tVertData = std::vector<glm::vec4>();

    int oVertCount = 0;
    int tVertCount = 0;

    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 256; y++) {
            for (int z = 0; z < 16; z++) {
                BlockType currType = this->getBlockAt(x, y, z);
                glm::vec4 biomeWts = this->getBiomeAt(x, z);

                if (currType != EMPTY) {
                    if (isHPlane(currType)) {
                        if (isVisible(x, y, z, currType)) {
                            for (const DirectionVector& dv : planeDirIter) {
                                std::vector<Vertex> faceVerts;
                                Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                for (const Vertex& v : faceVerts) {
                                    tVertData.push_back(v.position);
                                    tVertData.push_back(v.normal);
                                    tVertData.push_back(v.color);
                                    tVertData.push_back(v.uvCoords);
                                    tVertData.push_back(v.blockType);
                                    tVertData.push_back(v.biomeWts);
                                }
                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 1);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount + 3);

                                tVertCount += 4;
                            }
                        }
                    }
                    if (isCross2(currType)) {
                        // check if the block is exposed to air
                        if (isVisible(x, y, z, currType)) {
                            for (const DirectionVector& dv : cross2DirIter) {
                                std::vector<Vertex> faceVerts;
                                Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                for (const Vertex& v : faceVerts) {
                                    tVertData.push_back(v.position);
                                    tVertData.push_back(v.normal);
                                    tVertData.push_back(v.color);
                                    tVertData.push_back(v.uvCoords);
                                    tVertData.push_back(v.blockType);
                                    tVertData.push_back(v.biomeWts);
                                }

                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 1);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount + 3);

                                tVertCount += 4;
                            }
                        }
                    }
                    if (isCross4(currType)) {
                        // check if the block is exposed to air
                        if (isVisible(x, y, z, currType)) {
                            for (const DirectionVector& dv : cross4DirIter) {

                                std::vector<Vertex> faceVerts;
                                Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                for (const Vertex& v : faceVerts) {
                                    tVertData.push_back(v.position);
                                    tVertData.push_back(v.normal);
                                    tVertData.push_back(v.color);
                                    tVertData.push_back(v.uvCoords);
                                    tVertData.push_back(v.blockType);
                                    tVertData.push_back(v.biomeWts);
                                }

                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 1);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount + 3);

                                tIndices.push_back(tVertCount + 4);
                                tIndices.push_back(tVertCount + 5);
                                tIndices.push_back(tVertCount + 6);
                                tIndices.push_back(tVertCount + 4);
                                tIndices.push_back(tVertCount + 6);
                                tIndices.push_back(tVertCount + 7);

                                tVertCount += 8;
                            }
                        }
                    }
                    if (isPartialX(currType) || isPartialY(currType) || isPartialZ(currType)) {
                        for (const DirectionVector& dv : directionIter) {
                            if (isVisible(x, y, z, dv, currType)) {
                                if (!isTransparent(currType)) {
                                    std::vector<Vertex> faceVerts;
                                    Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                    for (const Vertex& v : faceVerts) {
                                        oVertData.push_back(v.position);
                                        oVertData.push_back(v.normal);
                                        oVertData.push_back(v.color);
                                        oVertData.push_back(v.uvCoords);
                                        oVertData.push_back(v.blockType);
                                        oVertData.push_back(v.biomeWts);
                                    }
                                    oIndices.push_back(oVertCount);
                                    oIndices.push_back(oVertCount + 1);
                                    oIndices.push_back(oVertCount + 2);
                                    oIndices.push_back(oVertCount);
                                    oIndices.push_back(oVertCount + 2);
                                    oIndices.push_back(oVertCount + 3);

                                    oVertCount += 4;
                                } else {
                                    std::vector<Vertex> faceVerts;
                                    Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                    for (const Vertex& v : faceVerts) {
                                        tVertData.push_back(v.position);
                                        tVertData.push_back(v.normal);
                                        tVertData.push_back(v.color);
                                        tVertData.push_back(v.uvCoords);
                                        tVertData.push_back(v.blockType);
                                        tVertData.push_back(v.biomeWts);
                                    }
                                    tIndices.push_back(tVertCount);
                                    tIndices.push_back(tVertCount + 1);
                                    tIndices.push_back(tVertCount + 2);
                                    tIndices.push_back(tVertCount);
                                    tIndices.push_back(tVertCount + 2);
                                    tIndices.push_back(tVertCount + 3);

                                    tVertCount += 4;
                                }
                            }
                        }
                    }
                    if (isFullCube(currType)) {
                        for (const DirectionVector& dv : directionIter) {
                            if (isVisible(x, y, z, dv, currType)) {
                                if (!isTransparent(currType)) {
                                    std::vector<Vertex> faceVerts;
                                    Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                    for (const Vertex& v : faceVerts) {
                                        oVertData.push_back(v.position);
                                        oVertData.push_back(v.normal);
                                        oVertData.push_back(v.color);
                                        oVertData.push_back(v.uvCoords);
                                        oVertData.push_back(v.blockType);
                                        oVertData.push_back(v.biomeWts);
                                    }
                                    oIndices.push_back(oVertCount);
                                    oIndices.push_back(oVertCount + 1);
                                    oIndices.push_back(oVertCount + 2);
                                    oIndices.push_back(oVertCount);
                                    oIndices.push_back(oVertCount + 2);
                                    oIndices.push_back(oVertCount + 3);

                                    oVertCount += 4;
                                } else {
                                    std::vector<Vertex> faceVerts;
                                    Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);
                                    for (const Vertex& v : faceVerts) {
                                        tVertData.push_back(v.position);
                                        tVertData.push_back(v.normal);
                                        tVertData.push_back(v.color);
                                        tVertData.push_back(v.uvCoords);
                                        tVertData.push_back(v.blockType);
                                        tVertData.push_back(v.biomeWts);
                                    }
                                    tIndices.push_back(tVertCount);
                                    tIndices.push_back(tVertCount + 1);
                                    tIndices.push_back(tVertCount + 2);
                                    tIndices.push_back(tVertCount);
                                    tIndices.push_back(tVertCount + 2);
                                    tIndices.push_back(tVertCount + 3);
                                    tVertCount += 4;
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    m_oCount = oIndices.size();

    generateOIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, oIndices.size() * sizeof(GLuint), oIndices.data(), GL_STATIC_DRAW);

    generateOVertData();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufVertData);
    mp_context->glBufferData(GL_ARRAY_BUFFER, oVertData.size() * sizeof(glm::vec4), oVertData.data(), GL_STATIC_DRAW);

    m_tCount = tIndices.size();

    generateTIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_tBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, tIndices.size() * sizeof(GLuint), tIndices.data(), GL_STATIC_DRAW);

    generateTVertData();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_tBufVertData);
    mp_context->glBufferData(GL_ARRAY_BUFFER, tVertData.size() * sizeof(glm::vec4), tVertData.data(), GL_STATIC_DRAW);
}

void Chunk::redistributeVertexData(std::vector<glm::vec4> ovd, std::vector<GLuint> oIndices,
                                   std::vector<glm::vec4> tvd, std::vector<GLuint> tIndices) {
    std::vector<glm::vec4> o_positions;
    std::vector<glm::vec4> o_normals;
    std::vector<glm::vec4> o_colors;
    std::vector<glm::vec4> o_uvs;
    std::vector<glm::vec4> o_bts;
    std::vector<glm::vec4> o_bWts;

    std::vector<glm::vec4> t_positions;
    std::vector<glm::vec4> t_normals;
    std::vector<glm::vec4> t_colors;
    std::vector<glm::vec4> t_uvs;
    std::vector<glm::vec4> t_bts;
    std::vector<glm::vec4> t_bWts;

    for (int i = 0; i < (int)ovd.size(); i = i+6) {
        o_positions.push_back(ovd[i]);
        o_normals.push_back(ovd[i+1]);
        o_colors.push_back(ovd[i+2]);
        o_uvs.push_back(ovd[i+3]);
        o_bts.push_back(ovd[i+4]);
        o_bWts.push_back(ovd[i+5]);
    }
    for (int i = 0; i < (int)tvd.size(); i = i+6) {
        t_positions.push_back(tvd[i]);
        t_normals.push_back(tvd[i+1]);
        t_colors.push_back(tvd[i+2]);
        t_uvs.push_back(tvd[i+3]);
        t_bts.push_back(tvd[i+4]);
        t_bWts.push_back(tvd[i+5]);
    }

    m_oCount = oIndices.size();
    generateOIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, oIndices.size() * sizeof(GLuint), oIndices.data(), GL_STATIC_DRAW);
    generateOPos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_positions.size() * sizeof(glm::vec4), o_positions.data(), GL_STATIC_DRAW);
    generateONor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_normals.size() * sizeof(glm::vec4), o_normals.data(), GL_STATIC_DRAW);
    generateOCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_colors.size() * sizeof(glm::vec4), o_colors.data(), GL_STATIC_DRAW);
    generateOUVs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufUVs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_uvs.size() * sizeof(glm::vec4), o_uvs.data(), GL_STATIC_DRAW);
    generateOBTs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufBTs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_bts.size() * sizeof(glm::vec4), o_bts.data(), GL_STATIC_DRAW);
    generateOBWts();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufBWts);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_bWts.size() * sizeof(glm::vec4), o_bWts.data(), GL_STATIC_DRAW);

    m_tCount = tIndices.size();
    generateTIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_tBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, tIndices.size() * sizeof(GLuint), tIndices.data(), GL_STATIC_DRAW);
    generateTPos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_tBufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_positions.size() * sizeof(glm::vec4), t_positions.data(), GL_STATIC_DRAW);
    generateTNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_tBufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_normals.size() * sizeof(glm::vec4), t_normals.data(), GL_STATIC_DRAW);
    generateTCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_colors.size() * sizeof(glm::vec4), t_colors.data(), GL_STATIC_DRAW);
    generateTUVs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufUVs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_uvs.size() * sizeof(glm::vec4), t_uvs.data(), GL_STATIC_DRAW);
    generateTBTs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufBTs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_bts.size() * sizeof(glm::vec4), t_bts.data(), GL_STATIC_DRAW);
    generateTBWts();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_tBufBWts);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_bWts.size() * sizeof(glm::vec4), t_bWts.data(), GL_STATIC_DRAW);

}

void Chunk::create() {
    loadVBO();
}

void Chunk::loadVBO() {

    // opaque
    std::vector<GLuint> oIndices = chunkVBOData.m_OIndexeData;
    std::vector<glm::vec4> oVertData = chunkVBOData.m_OVertData;
    // transparent
    std::vector<GLuint> tIndices = chunkVBOData.m_TIndexData;
    std::vector<glm::vec4> tVertData = chunkVBOData.m_TVertData;

    m_oCount = oIndices.size();

    generateOIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, oIndices.size() * sizeof(GLuint), oIndices.data(), GL_STATIC_DRAW);

    generateOVertData();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufVertData);
    mp_context->glBufferData(GL_ARRAY_BUFFER, oVertData.size() * sizeof(glm::vec4), oVertData.data(), GL_STATIC_DRAW);

    m_tCount = tIndices.size();

    generateTIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_tBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, tIndices.size() * sizeof(GLuint), tIndices.data(), GL_STATIC_DRAW);

    generateTVertData();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_tBufVertData);
    mp_context->glBufferData(GL_ARRAY_BUFFER, tVertData.size() * sizeof(glm::vec4), tVertData.data(), GL_STATIC_DRAW);
}

void Chunk::generateVBOData() {
    // opaque
    std::vector<GLuint> oIndices = std::vector<GLuint>();
    std::vector<glm::vec4> oVertData = std::vector<glm::vec4>();
    // transparent
    std::vector<GLuint> tIndices = std::vector<GLuint>();
    std::vector<glm::vec4> tVertData = std::vector<glm::vec4>();

    int oVertCount = 0;
    int tVertCount = 0;

    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 256; y++) {
            for (int z = 0; z < 16; z++) {
                BlockType currType = this->getBlockAt(x, y, z);
                glm::vec4 biomeWts = this->getBiomeAt(x, z);

                if (currType != EMPTY) {
                    if (isHPlane(currType)) {
                        if (isVisible(x, y, z, currType)) {
                            for (const DirectionVector& dv : planeDirIter) {
                                std::vector<Vertex> faceVerts;
                                Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                for (const Vertex& v : faceVerts) {
                                    tVertData.push_back(v.position);
                                    tVertData.push_back(v.normal);
                                    tVertData.push_back(v.color);
                                    tVertData.push_back(v.uvCoords);
                                    tVertData.push_back(v.blockType);
                                    tVertData.push_back(v.biomeWts);
                                }
                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 1);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount + 3);

                                tVertCount += 4;
                            }
                        }
                    }
                    if (isCross2(currType)) {
                        // check if the block is exposed to air
                        if (isVisible(x, y, z, currType)) {
                            for (const DirectionVector& dv : cross2DirIter) {
                                std::vector<Vertex> faceVerts;
                                Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                for (const Vertex& v : faceVerts) {
                                    tVertData.push_back(v.position);
                                    tVertData.push_back(v.normal);
                                    tVertData.push_back(v.color);
                                    tVertData.push_back(v.uvCoords);
                                    tVertData.push_back(v.blockType);
                                    tVertData.push_back(v.biomeWts);
                                }

                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 1);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount + 3);

                                tVertCount += 4;
                            }
                        }
                    }
                    if (isCross4(currType)) {
                        // check if the block is exposed to air
                        if (isVisible(x, y, z, currType)) {
                            for (const DirectionVector& dv : cross4DirIter) {

                                std::vector<Vertex> faceVerts;
                                Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                for (const Vertex& v : faceVerts) {
                                    tVertData.push_back(v.position);
                                    tVertData.push_back(v.normal);
                                    tVertData.push_back(v.color);
                                    tVertData.push_back(v.uvCoords);
                                    tVertData.push_back(v.blockType);
                                    tVertData.push_back(v.biomeWts);
                                }

                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 1);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount);
                                tIndices.push_back(tVertCount + 2);
                                tIndices.push_back(tVertCount + 3);

                                tIndices.push_back(tVertCount + 4);
                                tIndices.push_back(tVertCount + 5);
                                tIndices.push_back(tVertCount + 6);
                                tIndices.push_back(tVertCount + 4);
                                tIndices.push_back(tVertCount + 6);
                                tIndices.push_back(tVertCount + 7);

                                tVertCount += 8;
                            }
                        }
                    }
                    if (isPartialX(currType) || isPartialY(currType) || isPartialZ(currType)) {
                        for (const DirectionVector& dv : directionIter) {
                            if (isVisible(x, y, z, dv, currType)) {
                                if (!isTransparent(currType)) {
                                    std::vector<Vertex> faceVerts;
                                    Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                    for (const Vertex& v : faceVerts) {
                                        oVertData.push_back(v.position);
                                        oVertData.push_back(v.normal);
                                        oVertData.push_back(v.color);
                                        oVertData.push_back(v.uvCoords);
                                        oVertData.push_back(v.blockType);
                                        oVertData.push_back(v.biomeWts);
                                    }
                                    oIndices.push_back(oVertCount);
                                    oIndices.push_back(oVertCount + 1);
                                    oIndices.push_back(oVertCount + 2);
                                    oIndices.push_back(oVertCount);
                                    oIndices.push_back(oVertCount + 2);
                                    oIndices.push_back(oVertCount + 3);

                                    oVertCount += 4;
                                } else {
                                    std::vector<Vertex> faceVerts;
                                    Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                    for (const Vertex& v : faceVerts) {
                                        tVertData.push_back(v.position);
                                        tVertData.push_back(v.normal);
                                        tVertData.push_back(v.color);
                                        tVertData.push_back(v.uvCoords);
                                        tVertData.push_back(v.blockType);
                                        tVertData.push_back(v.biomeWts);
                                    }
                                    tIndices.push_back(tVertCount);
                                    tIndices.push_back(tVertCount + 1);
                                    tIndices.push_back(tVertCount + 2);
                                    tIndices.push_back(tVertCount);
                                    tIndices.push_back(tVertCount + 2);
                                    tIndices.push_back(tVertCount + 3);

                                    tVertCount += 4;
                                }
                            }
                        }
                    }
                    if (isFullCube(currType)) {
                        for (const DirectionVector& dv : directionIter) {
                            if (isVisible(x, y, z, dv, currType)) {
                                if (!isTransparent(currType)) {
                                    std::vector<Vertex> faceVerts;
                                    Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);

                                    for (const Vertex& v : faceVerts) {
                                        oVertData.push_back(v.position);
                                        oVertData.push_back(v.normal);
                                        oVertData.push_back(v.color);
                                        oVertData.push_back(v.uvCoords);
                                        oVertData.push_back(v.blockType);
                                        oVertData.push_back(v.biomeWts);
                                    }
                                    oIndices.push_back(oVertCount);
                                    oIndices.push_back(oVertCount + 1);
                                    oIndices.push_back(oVertCount + 2);
                                    oIndices.push_back(oVertCount);
                                    oIndices.push_back(oVertCount + 2);
                                    oIndices.push_back(oVertCount + 3);

                                    oVertCount += 4;
                                } else {
                                    std::vector<Vertex> faceVerts;
                                    Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, biomeWts);
                                    for (const Vertex& v : faceVerts) {
                                        tVertData.push_back(v.position);
                                        tVertData.push_back(v.normal);
                                        tVertData.push_back(v.color);
                                        tVertData.push_back(v.uvCoords);
                                        tVertData.push_back(v.blockType);
                                        tVertData.push_back(v.biomeWts);
                                    }
                                    tIndices.push_back(tVertCount);
                                    tIndices.push_back(tVertCount + 1);
                                    tIndices.push_back(tVertCount + 2);
                                    tIndices.push_back(tVertCount);
                                    tIndices.push_back(tVertCount + 2);
                                    tIndices.push_back(tVertCount + 3);
                                    tVertCount += 4;
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    chunkVBOData.m_OIndexeData = oIndices;
    chunkVBOData.m_OVertData = oVertData;

    chunkVBOData.m_TIndexData = tIndices;
    chunkVBOData.m_TVertData = tVertData;
}

void Chunk::setWorldPos(int x, int z) {
    int x_floor = static_cast<int>(glm::floor(x / 16.f));
    int z_floor = static_cast<int>(glm::floor(z / 16.f));
    worldPos_x = 16 * x_floor;
    worldPos_z = 16 * z_floor;
}

glm::ivec2 Chunk::getWorldPos() {
    return glm::ivec2(worldPos_x, worldPos_z);
}
