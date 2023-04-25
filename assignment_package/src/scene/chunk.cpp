#include "chunk.h"
#include "terrain.h"
#include <iostream>
#include <algorithm>
#include "biome.h"


Chunk::Chunk(OpenGLContext* context) : Drawable(context), m_blocks(), m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}} {
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z));
}

glm::vec4 Chunk::getBiomeAt(unsigned int x, unsigned int z) const {
    return m_biomes.at(x + 16 * z);
}

glm::vec4 Chunk::getBiomeAt(int x, int z) const {
    return getBiomeAt(static_cast<unsigned int>(x), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    if (isInBounds(glm::ivec3(x, y, z))) {
        m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
    } else if (x < 0 && m_neighbors.at(XNEG) != nullptr) {
        m_neighbors.at(XNEG)->setBlockAt(16 + x, y, z, t);
    } else if (x > 15 && m_neighbors.at(XPOS) != nullptr) {
        m_neighbors.at(XPOS)->setBlockAt(x - 16, y, z, t);
    } else if (z < 0 && m_neighbors.at(ZNEG) != nullptr) {
        m_neighbors.at(ZNEG)->setBlockAt(x, y, 16 + z, t);
    } else if (z > 15 && m_neighbors.at(ZPOS) != nullptr) {
        m_neighbors.at(ZPOS)->setBlockAt(x, y, z - 16, t);
    }
}

void Chunk::setBiomeAt(unsigned int x, unsigned int z, glm::vec4 b) {
    m_biomes.at(x + 16 * z) = b;
}

void Chunk::linkNeighbor(uPtr<Chunk>& neighbor, Direction dir) {
    if (neighbor != nullptr) {
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

    if (adjBlockType == EMPTY) {
        return true;
    }
    // if block is completely enclosed by non-transparent blocks
    if (!isVisible(x, y, z, bt)) {
        return false;
    }

    if (isFullCube(bt) && isFullCube(adjBlockType) && !isTransparent(adjBlockType)) {
        return false;
    }

    if ((bt == WATER || bt == LAVA || bt == ICE) &&
        (bt == adjBlockType ||
         ((isCross4(adjBlockType) || isCross2(adjBlockType)) && d != YPOS))) {
        return false;
    }

    if (bt == WATER && (adjBlockType == RICE_01 || adjBlockType == RICE_02)) {
        return false;
    }

    if ((d == XPOS || d == XNEG) && isPartialX(bt) && !isPartialY(bt) && !isPartialZ(bt) && bt == adjBlockType) {
        return false;
    }

    if ((d == YPOS || d == YNEG) && isPartialY(bt) && !isPartialX(bt) && !isPartialZ(bt) && bt == adjBlockType) {
        return false;
    }

    if ((d == ZPOS || d == ZNEG) && isPartialZ(bt) && !isPartialX(bt) && !isPartialY(bt) && bt == adjBlockType) {
        return false;
    }
    // if the block adjacent to this face is EMPTY or transparent and is of a different type
    if (isTransparent(adjBlockType) && adjBlockType != bt) {
        return true;
    }

    if ((bt == CEDAR_LEAVES || bt == TEAK_LEAVES ||
        bt == CHERRY_BLOSSOMS_1 || bt == CHERRY_BLOSSOMS_2 || bt == CHERRY_BLOSSOMS_3 || bt == CHERRY_BLOSSOMS_4 ||
        bt == MAPLE_LEAVES_1 || bt == MAPLE_LEAVES_2 || bt == MAPLE_LEAVES_3 ||
        bt == PINE_LEAVES ||
        bt == WISTERIA_BLOSSOMS_1 || bt == WISTERIA_BLOSSOMS_2 || bt == WISTERIA_BLOSSOMS_3) && bt == adjBlockType) {
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
    if (((d == XPOS || d == XNEG) && (isPartialX(bt) || isPartialX(adjBlockType)
                                      || isPartialY(adjBlockType) || isPartialZ(adjBlockType))) ||
        ((d == YPOS) && (isPartialY(bt) || isPartialX(adjBlockType) || isPartialZ(adjBlockType))) ||
        ((d == YNEG) && (isPartialX(adjBlockType) || isPartialY(adjBlockType) || isPartialZ(adjBlockType)))
        ||
        ((d == ZPOS || d == ZNEG) && (isPartialZ(bt) || isPartialX(adjBlockType)
                                      || isPartialY(adjBlockType) || isPartialZ(adjBlockType)))) {
        return true;
    }

    if (d == YNEG && (bt == CEDAR_PLANKS_2 || bt == TEAK_PLANKS_2 || bt == CHERRY_PLANKS_2 ||
                      bt == MAPLE_PLANKS_2 || bt == PINE_PLANKS_2 || bt == WISTERIA_PLANKS_2 ||
                      bt == ROOF_TILES_2 || bt == STRAW_2)) {
        return true;
    }

    if (d == YPOS && (adjBlockType == CEDAR_PLANKS_2 || adjBlockType == TEAK_PLANKS_2 || adjBlockType == CHERRY_PLANKS_2 ||
                      adjBlockType == MAPLE_PLANKS_2 || adjBlockType == PINE_PLANKS_2 || adjBlockType == WISTERIA_PLANKS_2 ||
                      adjBlockType == ROOF_TILES_2 || adjBlockType == STRAW_2)) {
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

void Chunk::createFaceVBOData(std::vector<Vertex>& verts, float currX, float currY, float currZ,
                              DirectionVector dirVec, BlockType bt, glm::vec4 bWts) {
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

    switch (bt) {
        case LILY_PAD:
        case LOTUS_1:
        case LOTUS_2:
            offsetYPOS = 0.005f;
            break;

        case WHEAT_1:
        case WHEAT_2:
        case WHEAT_3:
        case WHEAT_4:
        case WHEAT_5:
        case WHEAT_6:
        case WHEAT_7:
        case WHEAT_8:
        case SEA_GRASS:
            offsetXPOS = 0.75f;
            offsetXNEG = 0.25f;
            offsetZPOS = 0.75f;
            offsetZNEG = 0.25f;
            keepEdges = true;
            break;
        case TATAMI_XL: case TATAMI_XR: case TATAMI_ZT: case TATAMI_ZB:
            offsetYPOS = 0.0625;
            break;
        case SNOW_1: case CLOTH_1:
            offsetYPOS = 0.125;
            break;
        case SNOW_2: case CLOTH_2:
            offsetYPOS = 0.25;
            break;
        case SNOW_3: case CLOTH_3:
            offsetYPOS = 0.375;
            break;
        case SNOW_4: case ROOF_TILES_1: case STRAW_1: case CLOTH_4:
        case CEDAR_PLANKS_1: case TEAK_PLANKS_1: case CHERRY_PLANKS_1: case MAPLE_PLANKS_1: case PINE_PLANKS_1: case WISTERIA_PLANKS_1:
            offsetYPOS = 0.5;
            break;
        case ROOF_TILES_2: case STRAW_2:
        case CEDAR_PLANKS_2: case TEAK_PLANKS_2: case CHERRY_PLANKS_2: case MAPLE_PLANKS_2: case PINE_PLANKS_2: case WISTERIA_PLANKS_2:
            offsetYNEG = 0.5;
            break;
        case SNOW_5: case CLOTH_5:
            offsetYPOS = 0.625;
            break;
        case SNOW_6: case CLOTH_6:
            offsetYPOS = 0.75;
            break;
        case SNOW_7: case CLOTH_7:
            offsetYPOS = 0.875;
            break;
        case CEDAR_WINDOW_X: case TEAK_WINDOW_X: case CHERRY_WINDOW_X: case MAPLE_WINDOW_X: case PINE_WINDOW_X: case WISTERIA_WINDOW_X:
            offsetXPOS = 0.5625; // may be offsetX or offsetZ
            offsetXNEG = 0.4375;
            break;
        case CEDAR_WINDOW_Z: case TEAK_WINDOW_Z: case CHERRY_WINDOW_Z: case MAPLE_WINDOW_Z: case PINE_WINDOW_Z: case WISTERIA_WINDOW_Z:
            offsetZPOS = 0.5625;
            offsetZNEG = 0.4375;
            break;
        case TILLED_DIRT: case PATH: case IRRIGATED_SOIL:
            offsetYPOS = 0.9375;
            break;
        case BAMBOO_1: case BAMBOO_2: case BAMBOO_3:
            offsetXPOS = 0.5625;
            offsetXNEG = 0.4375;
            offsetZPOS = 0.5625;
            offsetZNEG = 0.4375;
            break;
        case PAPER_LANTERN:
            offsetXPOS = 0.75;
            offsetXNEG = 0.25;
            offsetZPOS = 0.75;
            offsetZNEG = 0.25;
            offsetYPOS = 0.75;
            break;
        case WOOD_LANTERN:
            offsetXPOS = 0.9375;
            offsetXNEG = 0.0625;
            offsetZPOS = 0.9375;
            offsetZNEG = 0.0625;
            break;
        case PAINTING_1_XP: case PAINTING_2_XP: case PAINTING_3_XP: case PAINTING_4_XP: case PAINTING_5_XP:
        case PAINTING_7T_XP: case PAINTING_7B_XP: case PAINTING_6L_XP: case PAINTING_6R_XP:
            offsetXPOS = 0.0625;
            break;
        case PAINTING_1_XN: case PAINTING_2_XN: case PAINTING_3_XN: case PAINTING_4_XN: case PAINTING_5_XN:
        case PAINTING_7T_XN: case PAINTING_7B_XN: case PAINTING_6L_XN: case PAINTING_6R_XN:
            offsetXNEG = 0.9375;
            break;
        case PAINTING_1_ZP: case PAINTING_2_ZP: case PAINTING_3_ZP: case PAINTING_4_ZP: case PAINTING_5_ZP:
        case PAINTING_7T_ZP: case PAINTING_7B_ZP: case PAINTING_6L_ZP: case PAINTING_6R_ZP:
            offsetZPOS = 0.0625;
            break;
        case PAINTING_1_ZN: case PAINTING_2_ZN: case PAINTING_3_ZN: case PAINTING_4_ZN: case PAINTING_5_ZN:
        case PAINTING_7T_ZN: case PAINTING_7B_ZN: case PAINTING_6L_ZN: case PAINTING_6R_ZN:
            offsetZNEG = 0.9375;
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
        case CHERRY_BLOSSOM_IKEBANA: case BLUE_HYDRANGEA_IKEBANA:
        case TULIP_IKEBANA: case DAFFODIL_IKEBANA:
            offsetXPOS = 0.6875;
            offsetXNEG = 0.3125;
            offsetZPOS = 0.6875;
            offsetZNEG = 0.3125;
            offsetYPOS = 0.375;
            break;
        case PLUM_BLOSSOM_IKEBANA: case MAGNOLIA_BUD_IKEBANA:
        case POPPY_IKEBANA: case MAPLE_IKEBANA:
        case ONCIDIUM_IKEBANA:
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
        case XPOS:
        case XNEG:
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
                verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG,
                                                                                              offsetYNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG,
                                                                                              offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS,
                                                                                              offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS,
                                                                                              offsetYNEG), bWts));

                if (isCross4(bt)) {
                    verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG,
                                                                                                  offsetYNEG), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG,
                                                                                                  offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS,
                                                                                                  offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS,
                                                                                                  offsetYNEG), bWts));
                }
            } else {
                verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS,
                                                                                              offsetYNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS,
                                                                                              offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG,
                                                                                              offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG,
                                                                                              offsetYNEG), bWts));

                if (isCross4(bt)) {
                    verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS,
                                                                                                  offsetYNEG), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetZPOS,
                                                                                                  offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG,
                                                                                                  offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetZNEG,
                                                                                                  offsetYNEG), bWts));
                }
            }

            break;

        case YPOS:
        case YNEG:
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
                verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                              offsetZPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                              offsetZPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                              offsetZNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                              offsetZNEG), bWts));
            } else {
                verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                              offsetZNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                              offsetZNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                              offsetZPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                              offsetZPOS), bWts));
            }

            break;

        case ZPOS:
        case ZNEG:
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
                verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                              offsetYNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                              offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                              offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                              offsetYNEG), bWts));

                if (isCross4(bt)) {
                    verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                                  offsetYNEG), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                                  offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                                  offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                                  offsetYNEG), bWts));
                }
            } else {
                verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                              offsetYNEG), bWts));
                verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                              offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                              offsetYPOS), bWts));
                verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                              offsetYNEG), bWts));

                if (isCross4(bt)) {
                    verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                                  offsetYNEG), bWts));
                    verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS,
                                                                                                  offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                                  offsetYPOS), bWts));
                    verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG,
                                                                                                  offsetYNEG), bWts));
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

                                oIndices.push_back(oVertCount + 4);
                                oIndices.push_back(oVertCount + 5);
                                oIndices.push_back(oVertCount + 6);
                                oIndices.push_back(oVertCount + 4);
                                oIndices.push_back(oVertCount + 6);
                                oIndices.push_back(oVertCount + 7);

                                oVertCount += 8;
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

    hasVBOData = true;

    m_oCount = oIndices.size();

    generateOIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, oIndices.size() * sizeof(GLuint), oIndices.data(),
                             GL_STATIC_DRAW);

    generateOVertData();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufVertData);
    mp_context->glBufferData(GL_ARRAY_BUFFER, oVertData.size() * sizeof(glm::vec4), oVertData.data(),
                             GL_STATIC_DRAW);

    m_tCount = tIndices.size();

    generateTIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_tBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, tIndices.size() * sizeof(GLuint), tIndices.data(),
                             GL_STATIC_DRAW);

    generateTVertData();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_tBufVertData);
    mp_context->glBufferData(GL_ARRAY_BUFFER, tVertData.size() * sizeof(glm::vec4), tVertData.data(),
                             GL_STATIC_DRAW);
    hasBinded = true;
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

    for (int i = 0; i < (int)ovd.size(); i = i + 6) {
        o_positions.push_back(ovd[i]);
        o_normals.push_back(ovd[i + 1]);
        o_colors.push_back(ovd[i + 2]);
        o_uvs.push_back(ovd[i + 3]);
        o_bts.push_back(ovd[i + 4]);
        o_bWts.push_back(ovd[i + 5]);
    }

    for (int i = 0; i < (int)tvd.size(); i = i + 6) {
        t_positions.push_back(tvd[i]);
        t_normals.push_back(tvd[i + 1]);
        t_colors.push_back(tvd[i + 2]);
        t_uvs.push_back(tvd[i + 3]);
        t_bts.push_back(tvd[i + 4]);
        t_bWts.push_back(tvd[i + 5]);
    }

    m_oCount = oIndices.size();
    generateOIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, oIndices.size() * sizeof(GLuint), oIndices.data(),
                             GL_STATIC_DRAW);
    generateOPos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_positions.size() * sizeof(glm::vec4),
                             o_positions.data(), GL_STATIC_DRAW);
    generateONor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_normals.size() * sizeof(glm::vec4), o_normals.data(),
                             GL_STATIC_DRAW);
    generateOCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_colors.size() * sizeof(glm::vec4), o_colors.data(),
                             GL_STATIC_DRAW);
    generateOUVs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufUVs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_uvs.size() * sizeof(glm::vec4), o_uvs.data(),
                             GL_STATIC_DRAW);
    generateOBTs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufBTs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_bts.size() * sizeof(glm::vec4), o_bts.data(),
                             GL_STATIC_DRAW);
    generateOBWts();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufBWts);
    mp_context->glBufferData(GL_ARRAY_BUFFER, o_bWts.size() * sizeof(glm::vec4), o_bWts.data(),
                             GL_STATIC_DRAW);

    m_tCount = tIndices.size();
    generateTIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_tBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, tIndices.size() * sizeof(GLuint), tIndices.data(),
                             GL_STATIC_DRAW);
    generateTPos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_tBufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_positions.size() * sizeof(glm::vec4),
                             t_positions.data(), GL_STATIC_DRAW);
    generateTNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_tBufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_normals.size() * sizeof(glm::vec4), t_normals.data(),
                             GL_STATIC_DRAW);
    generateTCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_colors.size() * sizeof(glm::vec4), t_colors.data(),
                             GL_STATIC_DRAW);
    generateTUVs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufUVs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_uvs.size() * sizeof(glm::vec4), t_uvs.data(),
                             GL_STATIC_DRAW);
    generateTBTs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufBTs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_bts.size() * sizeof(glm::vec4), t_bts.data(),
                             GL_STATIC_DRAW);
    generateTBWts();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_tBufBWts);
    mp_context->glBufferData(GL_ARRAY_BUFFER, t_bWts.size() * sizeof(glm::vec4), t_bWts.data(),
                             GL_STATIC_DRAW);

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
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, oIndices.size() * sizeof(GLuint), oIndices.data(),
                             GL_STATIC_DRAW);

    generateOVertData();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufVertData);
    mp_context->glBufferData(GL_ARRAY_BUFFER, oVertData.size() * sizeof(glm::vec4), oVertData.data(),
                             GL_STATIC_DRAW);

    m_tCount = tIndices.size();

    generateTIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_tBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, tIndices.size() * sizeof(GLuint), tIndices.data(),
                             GL_STATIC_DRAW);

    generateTVertData();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_tBufVertData);
    mp_context->glBufferData(GL_ARRAY_BUFFER, tVertData.size() * sizeof(glm::vec4), tVertData.data(),
                             GL_STATIC_DRAW);
    //std::cout<<"I hath binded \n";
    hasBinded = true;
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

                                oIndices.push_back(oVertCount + 4);
                                oIndices.push_back(oVertCount + 5);
                                oIndices.push_back(oVertCount + 6);
                                oIndices.push_back(oVertCount + 4);
                                oIndices.push_back(oVertCount + 6);
                                oIndices.push_back(oVertCount + 7);

                                oVertCount += 8;
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

    hasVBOData = true;

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

void Chunk::helperCreate(int worldXOrigin, int worldZOrigin) {
    std::vector<glm::vec3> cedarPos;
    std::vector<glm::vec3> teakPos;
    std::vector<glm::vec3> cottagePos;
    std::vector<glm::vec3> cherryPos;
    std::vector<glm::vec3> teaHousePos;
    std::vector<glm::vec3> maplePos;
    std::vector<glm::vec3> pinePos;
    std::vector<glm::vec3> wisteriaPos;

    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            int worldX = worldXOrigin + x;
            int worldZ = worldZOrigin + z;

            float hM = Biome::mountains(glm::vec2(worldX, worldZ));
            float hH = Biome::hills(glm::vec2(worldX, worldZ));
            float hF = Biome::forest(glm::vec2(worldX, worldZ));
            float hI = Biome::islands(glm::vec2(worldX, worldZ));

            std::pair<float, BiomeEnum> hb = blendMultipleBiomes(glm::vec2(worldX, worldZ), glm::vec2(x, z), hM,
                                                                 hH, hF, hI);
            float h = hb.first;
            BiomeEnum b = hb.second;

            int numDirtBlocks = 10 * Biome::fbm(glm::vec2(worldX, worldZ));
            if (b == MOUNTAINS) {
                if (h < 120) {
                    for (int y = 0; y < h - numDirtBlocks; ++y) {
                        setBlockAt(x, y, z, STONE);
                    }
                    for (int y = h - numDirtBlocks; y < h; ++y) {
                        setBlockAt(x, y, z, DIRT);
                    }
                    for (int y = h; y < 120; ++y) {
                        setBlockAt(x, y, z, WATER);
                    }
                } else {
                    for (int y = 0; y < h - numDirtBlocks - 1; ++y) {
                        setBlockAt(x, y, z, STONE);
                    }
                    for (int y = h - numDirtBlocks - 1; y < h - 1; ++y) {
                        setBlockAt(x, y, z, DIRT);
                    }
                    setBlockAt(x, h - 1, z, GRASS);
                    float snowBar = Biome::noise1D(glm::vec3(x, h, z));
                    if (snowBar < 0.5) {
                        setBlockAt(x, h, z, SNOW_1);
                    } else if (snowBar < 0.8) {
                        setBlockAt(x, h, z, SNOW_2);
                    } else {
                        setBlockAt(x, h, z, SNOW_3);
                    }
                }
            } else if (b == HILLS) {
                for (int y = 0; y < h - 3 - numDirtBlocks; ++y) {
                    setBlockAt(x, y, z, STONE);
                }
                for (int currY = h - 3 - numDirtBlocks; currY < h - 1; ++currY) {
                    setBlockAt(x, currY, z, DIRT);
                }

                float p3 = Biome::noise1D(glm::vec2(h, h));
                float p4 = Biome::noise1D(glm::vec3(worldX, h, worldZ));

                if (h < 120) {
                    setBlockAt(x, h - 1, z, DIRT);

                    for (int y = h; y < 120; ++y) {
                        setBlockAt(x, y, z, WATER);
                    }
                } else if (h <= 130) {
                    setBlockAt(x, h - 1, z, GRASS);
                } else if (h > 130 && h <= 130 + 10 * p3) {
                    setBlockAt(x, h - 1, z, TILLED_DIRT);
                    if (p4 < 0.1) {
                        setBlockAt(x, h, z, WHEAT_1);
                    } else if (p4 < 0.2) {
                        setBlockAt(x, h, z, WHEAT_2);
                    } else if (p4 < 0.3) {
                        setBlockAt(x, h, z, WHEAT_3);
                    } else if (p4 < 0.4) {
                        setBlockAt(x, h, z, WHEAT_4);
                    } else if (p4 < 0.5) {
                        setBlockAt(x, h, z, WHEAT_5);
                    } else if (p4 < 0.6) {
                        setBlockAt(x, h, z, WHEAT_6);
                    } else if (p4 < 0.7) {
                        setBlockAt(x, h, z, WHEAT_7);
                    } else {
                        setBlockAt(x, h, z, WHEAT_8);
                    }
                } else {
                    setBlockAt(x, h - 1, z, IRRIGATED_SOIL);
                    if (p4 < 0.3) {
                        setBlockAt(x, h, z, RICE_1);
                    } else if (p4 < 0.5) {
                        setBlockAt(x, h, z, RICE_2);
                    } else if (p4 < 0.6) {
                        setBlockAt(x, h, z, RICE_3);
                    } else if (p4 < 0.7) {
                        setBlockAt(x, h, z, RICE_4);
                    } else if (p4 < 0.85) {
                        setBlockAt(x, h, z, RICE_5);
                    } else {
                        setBlockAt(x, h, z, RICE_6);
                    }
                }
            } else if (b == FOREST) {
                for (int y = 0; y < h - numDirtBlocks - 1; ++y) {
                    setBlockAt(x, y, z, STONE);
                }
                for (int y = h - numDirtBlocks - 1; y < h - 1; ++y) {
                    setBlockAt(x, y, z, DIRT);
                }

                if (h < 120) {
                    setBlockAt(x, h - 1, z, DIRT);

                    for (int y = h; y < 120; ++y) {
                        setBlockAt(x, y, z, WATER);
                    }
                } else {
                    setBlockAt(x, h - 1, z, GRASS);
                }
            } else if (b == ISLANDS) {
                for (int y = 0; y < 80; ++y) {
                    setBlockAt(x, y, z, STONE);
                }
                for (int y = 80; y < h; ++y) {
                    setBlockAt(x, y, z, SAND);
                }
                if (h < 120) {
                    for (int y = h; y < 120; ++y) {
                        setBlockAt(x, y, z, WATER);
                    }
                }
            }

            // assets
            float p1 = Biome::noise1D(glm::vec2(worldX, worldZ));
            float p2 = Biome::fbm(glm::vec2(worldX, worldZ));
            glm::vec2 wTree = Biome::voronoi(glm::vec2(worldX, worldZ), 5);
            glm::vec2 wSparseTree = Biome::voronoi(glm::vec2(worldX, worldZ), 12);
            glm::vec2 wHouse = Biome::voronoi(glm::vec2(worldX, worldZ), 107);



            if (getBlockAt(x, h, z) == EMPTY || getBlockAt(x, h, z) == SNOW_1) {

                // TALL_GRASS
                if ((b == MOUNTAINS && p1 < 0.15) ||
                    (b == HILLS && p1 < 0.35) ||
                    (b == FOREST && p1 < 0.05) ||
                    (b == ISLANDS && p1 < 0.1)) {

                    setBlockAt(x, h, z, TALL_GRASS);
                }

                // bamboo, trees
                if (b == MOUNTAINS && h > 130) {
                    if (worldX == wTree.x && worldZ == wTree.y) {
                        if (p1 < 0.175) {
                            cedarPos.push_back(glm::vec3(x, h, z));
                        } else if (p1 < 0.225) {
                            teakPos.push_back(glm::vec3(x, h, z));
                        }
                    }
                    if (worldX == wHouse.x && worldZ == wHouse.y) {
                        cottagePos.push_back(glm::vec3(x, h, z));
                    }
                } else if (b == FOREST) {
                    if (p1 < 0.04) {
                        // bamboo
                        int y = h;
                        int addHeight = 0;
                        while (addHeight == 0) {
                            setBlockAt(x, y, z, BAMBOO_1);
                            y++;
                            if (Biome::noise1D(glm::vec3(worldX, y, worldZ)) >= 0.75) {
                                addHeight = 1;
                            }
                        }
                        while (addHeight == 1) {
                            setBlockAt(x, y, z, BAMBOO_2);
                            y++;
                            if (Biome::noise1D(glm::vec3(worldX, y, worldZ)) >= 0.5) {
                                addHeight = 2;
                            }
                        }
                        while (addHeight == 2) {
                            setBlockAt(x, y, z, BAMBOO_3);
                            y++;
                            if (Biome::noise1D(glm::vec3(worldX, y, worldZ)) >= 0.25) {
                                addHeight = 3;
                            }
                        }
                    }

                    if (worldX == wSparseTree.x && worldZ == wSparseTree.y) {
                        cherryPos.push_back(glm::vec3(x, h, z));
                    }
                    if (worldX == wHouse.x && worldZ == wHouse.y && p1 > 0.75) {
                        teaHousePos.push_back(glm::vec3(x, h, z));
                    }
                } else if (b == HILLS) {
                    float p3 = Biome::noise1D(glm::vec3(worldX, h, worldZ));
                    if (worldX == wHouse.x && worldZ == wHouse.y && p3 < 0.2) {
                        createHut(x, h, z);
                    }
                } else if (b == ISLANDS) {
                    if (worldX == wTree.x && worldZ == wTree.y) {
                        if (p1 < 0.2) {
                            pinePos.push_back(glm::vec3(x, h, z));
                        } else {
                            maplePos.push_back(glm::vec3(x, h, z));
                        }
                    }
                }
            } else if (getBlockAt(x, h, z) == WATER) {
                // lotuses, coral, sea grass, kelp, lanterns
                if (b == MOUNTAINS) {
                    if (p1 < 0.0125) {
                        setBlockAt(x, 120 + (1000 * p1), z, PAPER_LANTERN);
                    } else if (p1 < 0.025) {
                        setBlockAt(x, 120, z, PAPER_LANTERN);
                    }
                } else if (b == FOREST) {
                    if (p1 < 0.05) {
                        setBlockAt(x, 120, z, LILY_PAD);
                    } else if (p1 < 0.075) {
                        setBlockAt(x, 120, z, LOTUS_1);
                    } else if (p1 < 0.1) {
                        setBlockAt(x, 120, z, LOTUS_2);
                    }
                } else if (b == ISLANDS && p2 < 0.75 && h < 119) {
                    if (p1 < 0.1) {
                        setBlockAt(x, h, z, SEA_GRASS);
                    } else if (p1 < 0.11) {
                        setBlockAt(x, h, z, CORAL_1);
                    } else if (p1 < 0.12) {
                        setBlockAt(x, h, z, CORAL_2);
                    } else if (p1 < 0.13) {
                        setBlockAt(x, h, z, CORAL_3);
                    } else if (p1 < 0.14) {
                        setBlockAt(x, h, z, CORAL_4);
                    } else if (p1 < 0.3) {
                        int y = h;
                        bool addHeight = true;
                        while (y < 118 && addHeight) {
                            setBlockAt(x, y, z, KELP_1);
                            y++;
                            addHeight = (Biome::noise1D(glm::vec3(worldX, y, worldZ)) < 0.75);
                        }
                        setBlockAt(x, y, z, KELP_2);
                    }
                }
            }

            bool prevNotGround = false;
            std::vector<float> treePos;
            for (int currY = 1; currY <= 106; currY++) {
                float cavePerlin3D = Biome::perlin3D(glm::vec3(worldX, currY, worldZ) * 0.06f);
                float cavePerlin3DTwo = Biome::perlin3D(glm::vec3(worldX, currY, glm::mix(worldX, worldZ, 0.35f)) * 0.06f);

                float p3 = Biome::noise1D(glm::vec3(worldX, currY, worldZ));

                if (cavePerlin3D + cavePerlin3DTwo < -0.15f) {
                    if (currY < 25) {
                        setBlockAt(x, currY, z, LAVA);
                    } else {
                        setBlockAt(x, currY, z, EMPTY);
                        if (!prevNotGround) {

                            setBlockAt(x, currY - 1, z, GRASS);

                            if (p3 < 0.02) {
                                setBlockAt(x, currY, z, GHOST_LILY);
                            } else if (p3 < 0.04) {
                                setBlockAt(x, currY, z, GHOST_WEED);
                            } else if (p3 < 0.1) {
                                setBlockAt(x, currY, z, TALL_GRASS);
                            }
                            else if (p3 < 0.1075) {
                                treePos.push_back(currY);
                            }
                        }
                        prevNotGround = true;
                    }
                } else {
                    prevNotGround = false;
                }
            }
            for (float y : treePos) {
                float p4 = Biome::noise1D(glm::vec2(worldX, worldZ));
                if (worldX == wTree.x && worldZ == wTree.y) {
                    if (p4 < 0.2) {
                        createDeciduous3(x, y, z, WISTERIA_BLOSSOMS_1, WISTERIA_WOOD_Y);
                    } else if (p4 < 0.4) {
                        createDeciduous2(x, y, z, WISTERIA_BLOSSOMS_2, WISTERIA_WOOD_Y);
                    } else {
                        createDeciduous1(x, y, z, WISTERIA_BLOSSOMS_3, WISTERIA_WOOD_Y);
                    }
                }
            }

            if (getBlockAt(x, h, z) == WATER) {
                int y = h - 1;
                while (getBlockAt(x, y, z) == EMPTY && y > 0) {
                    setBlockAt(x, y, z, WATER);
                    y--;
                }
            }
            setBlockAt(x, 0, z, BEDROCK);
        }
    }

    for (glm::vec3 p : cedarPos) {
        createConifer1(p.x, p.y, p.z, CEDAR_LEAVES, CEDAR_WOOD_Y);
    }
    for (glm::vec3 p : teakPos) {
        createConifer2(p.x, p.y, p.z, TEAK_LEAVES, TEAK_WOOD_Y);
    }
    for (glm::vec3 p : cottagePos) {
        float p3 = Biome::noise1D(glm::vec3(p.x, p.y, p.z));
        if (p3 < 0.2) {
            createCottage1(p.x, p.y, p.z);
        } else if (p3 < 0.4) {
            createCottage2(p.x, p.y, p.z);
        }
    }
    for (glm::vec3 p : cherryPos) {
        float p3 = Biome::noise1D(glm::vec3(p.x, p.y, p.z));
        if (p3 < 0.05) {
            createDeciduous2(p.x, p.y, p.z, CHERRY_BLOSSOMS_1, CHERRY_WOOD_Y);
        } else if (p3 < 0.055) {
            createDeciduous3(p.x, p.y, p.z, CHERRY_BLOSSOMS_2, CHERRY_WOOD_Y);
        } else if (p3 < 0.06) {
            createDeciduous3(p.x, p.y, p.z, CHERRY_BLOSSOMS_3, CHERRY_WOOD_Y);
        } else if (p3 < 0.065) {
            createDeciduous1(p.x, p.y, p.z, CHERRY_BLOSSOMS_4, CHERRY_WOOD_Y);
        } else if (p3 < 0.066) {
            createToriiGate(p.x, p.y, p.z, 0);
        } else if (p3 < 0.067) {
            createToriiGate(p.x, p.y, p.z, 1);
        }
    }
    for (glm::vec3 p : teaHousePos) {
        createTeaHouse(p.x, p.y, p.z);
    }
    for (glm::vec3 p : pinePos) {
        createConifer3(p.x, p.y, p.z, PINE_LEAVES, PINE_WOOD_Y);
    }
    for (glm::vec3 p : maplePos) {
        float p3 = Biome::noise1D(glm::vec3(p.x, p.y, p.z));
        if (p3 < 0.2) {
            createDeciduous2(p.x, p.y, p.z, MAPLE_LEAVES_1, MAPLE_WOOD_Y);
        } else if (p3 < 0.4) {
            createDeciduous3(p.x, p.y, p.z, MAPLE_LEAVES_2, MAPLE_WOOD_Y);
        } else if (p3 < 0.6) {
            createDeciduous1(p.x, p.y, p.z, MAPLE_LEAVES_3, MAPLE_WOOD_Y);
        }
    }
}

std::pair<float, BiomeEnum> Chunk::blendMultipleBiomes(glm::vec2 worldXZ, glm::vec2 localXZ,
                                                       float mountH, float hillH,
                                                       float forestH, float islandH) {

    // perform bilinear interpolation

    BiomeEnum b;
    glm::vec4 biomeWts;

    double elev = std::clamp((Biome::perlin1(worldXZ / 197.f) + 1.f) / 2.f, 0.f, 1.f); // remap perlin noise from (-1, 1) to (0, 1)
    double temp = std::clamp((Biome::perlin2(worldXZ / 308.f) + 1.f) / 2.f, 0.f, 1.f);


//    std::cout<<elev<<","<<temp<<std::endl;

    if (elev >= 0.5 && temp < 0.5) {
        b = MOUNTAINS;
    } else if (elev >= 0.5 && temp >= 0.5) {
        b = HILLS;
    } else if (elev < 0.5 && temp < 0.5) {
        b = FOREST;
    } else {
        b = ISLANDS;
    }

    // set biome weights in m_biomes for each xz coord in this chunk
    biomeWts.x = elev * (1.f - temp);
    biomeWts.y = elev * temp;
    biomeWts.z = (1.f - elev) * (1.f - temp);
    biomeWts.w = (1.f - elev) * temp;
    setBiomeAt(localXZ[0], localXZ[1], biomeWts);

//    std::cout<<"("<<biomeWts.x<<","<<biomeWts.y<<","<<biomeWts.z<<","<<biomeWts.w<<")"<<std::endl;
//    if (biomeWts.x + biomeWts.y + biomeWts.z + biomeWts.w > 1.00001 || biomeWts.x + biomeWts.y + biomeWts.z + biomeWts.w < 0.99999) {
//        std::cout << "something is wrong" << std::endl;
//    }

    float h = (biomeWts.x * mountH) +
              (biomeWts.y * hillH) +
              (biomeWts.z * forestH) +
              (biomeWts.w * islandH);
    return std::pair(h, b);
}

void Chunk::createToriiGate(int x, int y, int z, int rot) {
    if (rot == 0) {
        setBlockAt(x, y, z, BLACK_PAINTED_WOOD);
        setBlockAt(x + 6, y, z, BLACK_PAINTED_WOOD);

        for (int y1 = y + 1; y1 <= y + 5; y1++) {
            setBlockAt(x, y1, z, RED_PAINTED_WOOD);
            setBlockAt(x + 6, y1, z, RED_PAINTED_WOOD);
        }

        for (int x1 = x - 1; x1 <= x + 7; x1++) {
            setBlockAt(x1, y + 6, z, RED_PAINTED_WOOD);
            setBlockAt(x1, y + 8, z, RED_PAINTED_WOOD);
        }
        setBlockAt(x, y + 7, z, RED_PAINTED_WOOD);
        setBlockAt(x + 3, y + 7, z, RED_PAINTED_WOOD);
        setBlockAt(x + 6, y + 7, z, RED_PAINTED_WOOD);

        for (int x2 = x - 2; x2 <= x + 8; x2++) {
            setBlockAt(x2, y + 9, z, ROOF_TILES_1);
        }
    } else {
        setBlockAt(x, y, z, BLACK_PAINTED_WOOD);
        setBlockAt(x, y, z + 6, BLACK_PAINTED_WOOD);

        for (int y1 = y + 1; y1 <= y + 5; y1++) {
            setBlockAt(x, y1, z, RED_PAINTED_WOOD);
            setBlockAt(x, y1, z + 6, RED_PAINTED_WOOD);
        }

        for (int z1 = z - 1; z1 <= z + 7; z1++) {
            setBlockAt(x, y + 6, z1, RED_PAINTED_WOOD);
            setBlockAt(x, y + 8, z1, RED_PAINTED_WOOD);
        }
        setBlockAt(x, y + 7, z, RED_PAINTED_WOOD);
        setBlockAt(x, y + 7, z + 3, RED_PAINTED_WOOD);
        setBlockAt(x, y + 7, z + 6, RED_PAINTED_WOOD);

        for (int z2 = z - 2; z2 <= z + 8; z2++) {
            setBlockAt(x, y + 9, z2, ROOF_TILES_1);
        }
    }
}

void Chunk::createHut(int x, int y, int z) {
    for (int y1 = y; y1 <= y + 3; y1++) {
        setBlockAt(x, y1, z, MAPLE_WOOD_Y);
        setBlockAt(x + 8, y1, z, MAPLE_WOOD_Y);
        setBlockAt(x, y1, z + 8, MAPLE_WOOD_Y);
        setBlockAt(x + 8, y1, z + 8, MAPLE_WOOD_Y);
    }
    for (int x1 = x - 1; x1 <= x + 9; x1++) {
        for (int z1 = z - 1; z1 <= z + 9; z1++) {
            setBlockAt(x1, y + 4, z1, MAPLE_WOOD_Z);
        }
        setBlockAt(x1, y + 4, z - 2, MAPLE_WOOD_X);
        setBlockAt(x1, y + 4, z + 10, MAPLE_WOOD_X);
    }
    setBlockAt(x - 3, y + 4, z - 2, MAPLE_WOOD_X);
    setBlockAt(x - 3, y + 4, z + 10, MAPLE_WOOD_X);
    setBlockAt(x - 2, y + 4, z - 2, MAPLE_WOOD_X);
    setBlockAt(x - 2, y + 4, z + 10, MAPLE_WOOD_X);
    setBlockAt(x + 10, y + 4, z - 2, MAPLE_WOOD_X);
    setBlockAt(x + 10, y + 4, z + 10, MAPLE_WOOD_X);
    setBlockAt(x + 11, y + 4, z - 2, MAPLE_WOOD_X);
    setBlockAt(x + 11, y + 4, z + 10, MAPLE_WOOD_X);


    for (int y2 = y + 5; y2 <= y + 10; y2++) {
        for (int x2 = x; x2 <= x + 8; x2++) {
            setBlockAt(x2, y2, z, CHERRY_PLANKS);
            setBlockAt(x2, y2, z + 8, CHERRY_PLANKS);
        }
        for (int z2 = z + 1; z2 <= z + 7; z2++) {
            setBlockAt(x, y2, z2, CHERRY_PLANKS);
            setBlockAt(x + 8, y2, z2, CHERRY_PLANKS);
        }
    }
    for (int x2 = x + 2; x2 <= x + 6; x2++) {
        setBlockAt(x2, y + 11, z, CHERRY_PLANKS);
        setBlockAt(x2, y + 11, z + 8, CHERRY_PLANKS);
    }
    setBlockAt(x + 4, y + 12, z, CHERRY_PLANKS);
    setBlockAt(x + 4, y + 12, z + 8, CHERRY_PLANKS);

    // roof
    for (int z3 = z - 1; z3 <= z + 9; z3++) {
        int dx = -2;
        for (int y3 = y + 10; y3 <= y + 12; y3++) {
            setBlockAt(x + dx, y3, z3, STRAW_1);
            setBlockAt(x + 8 - dx, y3, z3, STRAW_1);
            dx++;
            setBlockAt(x + dx, y3, z3, STRAW);
            setBlockAt(x + 8 - dx, y3, z3, STRAW);
            dx++;
        }
        setBlockAt(x + 4, y + 13, z3, STRAW_1);
    }

    setBlockAt(x + 4, y + 5, z, EMPTY);
    setBlockAt(x + 4, y + 6, z, EMPTY);

//    setBlockAt(x + 4, y + 5, z + 8, EMPTY);
//    setBlockAt(x + 4, y + 6, z + 8, EMPTY);

//    setBlockAt(x, y + 5, z + 4, EMPTY);
//    setBlockAt(x, y + 6, z + 4, EMPTY);

    setBlockAt(x + 8, y + 5, z + 4, EMPTY);
    setBlockAt(x + 8, y + 6, z + 4, EMPTY);
}

void Chunk::createCottage1(int x, int y, int z) {
    for (int x1 = x; x1 <= x + 10; x1++) {
        for (int z1 = z; z1 <= z + 7; z1++) {
            for (int y1 = y; y1 <= y + 10; y1++) {
                setBlockAt(x1, y1, z1, EMPTY);
            }
        }
    }

    // floor 1 walls
    for (int y1 = y; y1 <= y + 3; y1++) {
        for (int x1 = x; x1 <= x + 10; x1++) {
            setBlockAt(x1, y1, z, CHERRY_PLANKS);
            setBlockAt(x1, y1, z + 7, CHERRY_PLANKS);
        }
        for (int z1 = z; z1 <= z + 7; z1++) {
            setBlockAt(x, y1, z1, CHERRY_PLANKS);
            setBlockAt(x + 10, y1, z1, CHERRY_PLANKS);
        }
    }
    for (int x1 = x; x1 <= x + 4; x1++) {
        for (int y1 = y; y1 <= y + 2; y1++) {
            setBlockAt(x1, y1, z + 7, EMPTY);
            setBlockAt(x1, y1, z + 6, CHERRY_PLANKS);
        }
    }

    // floor 1 decor
    for (int x1 = x + 1; x1 < x + 10; x1 += 2) {
        setBlockAt(x1, y + 6, z - 1, PAPER_LANTERN);
        setBlockAt(x1, y + 6, z + 8, PAPER_LANTERN);
    }
    setBlockAt(x + 2, y, z + 6, CHERRY_WINDOW_Z);
    setBlockAt(x + 2, y + 1, z + 6, CHERRY_WINDOW_Z);
    setBlockAt(x + 3, y, z + 6, EMPTY);
    setBlockAt(x + 3, y + 1, z + 6, EMPTY);
    setBlockAt(x + 4, y + 2, z + 7, PAPER_LANTERN);
    setBlockAt(x + 2, y, z + 5, WOOD_LANTERN);
    setBlockAt(x + 2, y + 1, z + 5, MAPLE_IKEBANA);
    setBlockAt(x + 3, y + 1, z + 1, PAINTING_4_ZP);

    setBlockAt(x + 10, y, z + 3, EMPTY);
    setBlockAt(x + 10, y + 1, z + 3, EMPTY);
    setBlockAt(x + 10, y, z + 4, CHERRY_WINDOW_X);
    setBlockAt(x + 10, y + 1, z + 4, CHERRY_WINDOW_X);

    // floor 2 walls
    for (int y2 = y + 4; y2 <= y + 7; y2++) {
        for (int x1 = x; x1 <= x + 10; x1++) {
            setBlockAt(x1, y2, z, PINE_PLANKS);
            setBlockAt(x1, y2, z + 7, PINE_PLANKS);
        }
        for (int z2 = z; z2 <= z + 7; z2++) {
            setBlockAt(x, y2, z2, PINE_PLANKS);
            setBlockAt(x + 10, y2, z2, PINE_PLANKS);
        }
    }
    for (int z2 = z + 2; z2 <= z + 5; z2++) {
        setBlockAt(x, y + 8, z2, PINE_PLANKS);
        setBlockAt(x + 10, y + 8, z2, PINE_PLANKS);
    }

    // floor 2 decor
    setBlockAt(x + 9, y + 4, z + 6, WOOD_LANTERN);
    setBlockAt(x + 9, y + 5, z + 6, DAFFODIL_IKEBANA);
    setBlockAt(x + 1, y + 4, z + 1, WOOD_LANTERN);
    setBlockAt(x + 1, y + 5, z + 1, ONCIDIUM_IKEBANA);

    setBlockAt(x + 2, y + 4, z + 3, WISTERIA_WOOD_Z);
    setBlockAt(x + 3, y + 4, z + 3, WISTERIA_WOOD_Z);
    setBlockAt(x + 2, y + 4, z + 4, WISTERIA_WOOD_Z);
    setBlockAt(x + 3, y + 4, z + 4, WISTERIA_WOOD_Z);
    setBlockAt(x + 2, y + 4, z + 5, WISTERIA_WOOD_Z);
    setBlockAt(x + 3, y + 4, z + 5, WISTERIA_WOOD_Z);

    setBlockAt(x + 2, y + 5, z + 4, CHRYSANTHEMUM_IKEBANA);
    setBlockAt(x + 2, y + 5, z + 5, PAPER_LANTERN);


    // floor
    for (int x2 = x; x2 <= x + 10; x2++) {
        for (int z2 = z; z2 <= z + 7; z2++) {
            setBlockAt(x2, y - 1, z2, PINE_PLANKS);
            setBlockAt(x2, y + 3, z2, CHERRY_PLANKS);
        }
    }

    // balcony
    for (int x3 = x - 1; x3 <= x + 11; x3++) {
        setBlockAt(x3, y + 3, z + 8, ROOF_TILES);
        setBlockAt(x3, y + 3, z + 9, ROOF_TILES_1);
        setBlockAt(x3, y + 3, z - 1, ROOF_TILES_2);
        setBlockAt(x3, y + 3, z - 2, ROOF_TILES_2);
    }
    for (int z3 = z; z3 <= z + 7; z3++) {
        setBlockAt(x - 1, y + 3, z3, ROOF_TILES_2);
        setBlockAt(x + 11, y + 3, z3, ROOF_TILES_2);
    }

    // roof
    for (int x3 = x - 1; x3 <= z + 12; x3++) {
        int dx = -2;
        for (int y3 = y + 7; y3 <= y + 9; y3++) {
            setBlockAt(x3, y3, z + dx, ROOF_TILES_1);
            setBlockAt(x3, y3, z + 7 - dx, ROOF_TILES_1);
            dx++;
            setBlockAt(x3, y3, z + dx, ROOF_TILES);
            setBlockAt(x3, y3, z + 7 - dx, ROOF_TILES);
            dx++;
        }
    }

    // stairs
    setBlockAt(x + 5, y, z + 1, CHERRY_PLANKS_2);
    setBlockAt(x + 5, y, z + 2, CHERRY_PLANKS_2);
    setBlockAt(x + 6, y + 1, z + 1, CHERRY_PLANKS_2);
    setBlockAt(x + 6, y + 1, z + 2, CHERRY_PLANKS_2);
    setBlockAt(x + 7, y + 2, z + 1, CHERRY_PLANKS_2);
    setBlockAt(x + 7, y + 2, z + 2, CHERRY_PLANKS_2);

    setBlockAt(x + 5, y + 3, z + 1, EMPTY);
    setBlockAt(x + 5, y + 3, z + 2, EMPTY);
    setBlockAt(x + 6, y + 3, z + 1, EMPTY);
    setBlockAt(x + 6, y + 3, z + 2, EMPTY);
    setBlockAt(x + 7, y + 3, z + 1, EMPTY);
    setBlockAt(x + 7, y + 3, z + 2, EMPTY);
}

void Chunk::createCottage2(int x, int y, int z) {
    for (int x1 = x - 2; x1 <= x + 12; x1++) {
        for (int z1 = z - 2; z1 <= z + 11; z1++) {
            for (int y1 = y; y1 <= y + 15; y1++) {
                setBlockAt(x1, y1, z1, EMPTY);
            }
        }
    }

    // platform
    for (int x1 = x - 2; x1 <= x + 12; x1++) {
        for (int z1 = z - 2; z1 <= z + 11; z1++) {
            setBlockAt(x1, y, z1, PINE_PLANKS_1);
        }
    }
    for (int y1 = y - 1; y1 >= y - 4; y1--) {
        setBlockAt(x - 2, y1, z - 2, MAPLE_WOOD_Y);
        setBlockAt(x + 12, y1, z - 2, MAPLE_WOOD_Y);
        setBlockAt(x - 2, y1, z + 11, MAPLE_WOOD_Y);
        setBlockAt(x + 12, y1, z + 11, MAPLE_WOOD_Y);
    }

    // floor
    for (int x1 = x; x1 <= x + 10; x1++) {
        for (int z1 = z; z1 <= z + 9; z1++) {
            setBlockAt(x1, y, z1, PINE_WOOD_X);
        }
    }
    for (int z1 = z; z1 <= z + 9; z1++) {
        setBlockAt(x, y, z1, PINE_WOOD_Z);
        setBlockAt(x + 10, y, z1, PINE_WOOD_Z);
    }


    // walls
    for (int y1 = y + 1; y1 <= y + 7; y1++) {
        for (int x1 = x; x1 <= x + 10; x1++) {
            setBlockAt(x1, y1, z, PLASTER);
            setBlockAt(x1, y1, z + 9, PLASTER);
        }
        for (int z1 = z; z1 <= z + 9; z1++) {
            setBlockAt(x, y1, z1, PLASTER);
            setBlockAt(x + 10, y1, z1, PLASTER);
        }
    }

    setBlockAt(x, y + 8, z + 4, PLASTER);
    setBlockAt(x + 10, y + 8, z + 4, PLASTER);
    setBlockAt(x, y + 8, z + 5, PLASTER);
    setBlockAt(x + 10, y + 8, z + 5, PLASTER);

    for (int y1 = y + 1; y1 <= y + 2; y1++) {
        for (int z1 = z + 1; z1 <= z + 8; z1++) {
            setBlockAt(x, y1, z1, MAPLE_PLANKS);
            setBlockAt(x + 10, y1, z1, MAPLE_PLANKS);
        }
    }
    setBlockAt(x + 1, y + 1, z, MAPLE_PLANKS);
    setBlockAt(x + 1, y + 1, z + 9, MAPLE_PLANKS);
    setBlockAt(x + 2, y + 1, z, MAPLE_PLANKS);
    setBlockAt(x + 2, y + 1, z + 9, MAPLE_PLANKS);
    setBlockAt(x + 8, y + 1, z, MAPLE_PLANKS);
    setBlockAt(x + 8, y + 1, z + 9, MAPLE_PLANKS);
    setBlockAt(x + 9, y + 1, z, MAPLE_PLANKS);
    setBlockAt(x + 9, y + 1, z + 9, MAPLE_PLANKS);

    setBlockAt(x + 1, y + 2, z, MAPLE_WINDOW_Z);
    setBlockAt(x + 1, y + 3, z, MAPLE_WINDOW_Z);
    setBlockAt(x + 2, y + 2, z, MAPLE_WINDOW_Z);
    setBlockAt(x + 2, y + 3, z, MAPLE_WINDOW_Z);
    setBlockAt(x + 8, y + 2, z, MAPLE_WINDOW_Z);
    setBlockAt(x + 8, y + 3, z, MAPLE_WINDOW_Z);
    setBlockAt(x + 9, y + 2, z, MAPLE_WINDOW_Z);
    setBlockAt(x + 9, y + 3, z, MAPLE_WINDOW_Z);

    setBlockAt(x + 1, y + 2, z + 9, MAPLE_WINDOW_Z);
    setBlockAt(x + 1, y + 3, z + 9, MAPLE_WINDOW_Z);
    setBlockAt(x + 2, y + 2, z + 9, MAPLE_WINDOW_Z);
    setBlockAt(x + 2, y + 3, z + 9, MAPLE_WINDOW_Z);
    setBlockAt(x + 8, y + 2, z + 9, MAPLE_WINDOW_Z);
    setBlockAt(x + 8, y + 3, z + 9, MAPLE_WINDOW_Z);
    setBlockAt(x + 9, y + 2, z + 9, MAPLE_WINDOW_Z);
    setBlockAt(x + 9, y + 3, z + 9, MAPLE_WINDOW_Z);

    for (int y1 = y; y1 <= y + 7; y1++) {
        setBlockAt(x, y1, z, WISTERIA_WOOD_Y);
        setBlockAt(x + 10, y1, z, WISTERIA_WOOD_Y);
        setBlockAt(x, y1, z + 9, WISTERIA_WOOD_Y);
        setBlockAt(x + 10, y1, z + 9, WISTERIA_WOOD_Y);

        setBlockAt(x, y1, z + 3, WISTERIA_WOOD_Y);
        setBlockAt(x, y1, z + 6, WISTERIA_WOOD_Y);
        setBlockAt(x + 10, y1, z + 3, WISTERIA_WOOD_Y);
        setBlockAt(x + 10, y1, z + 6, WISTERIA_WOOD_Y);

        setBlockAt(x + 3, y1, z, WISTERIA_WOOD_Y);
        setBlockAt(x + 7, y1, z, WISTERIA_WOOD_Y);
        setBlockAt(x + 3, y1, z + 9, WISTERIA_WOOD_Y);
        setBlockAt(x + 7, y1, z + 9, WISTERIA_WOOD_Y);
    }
    for (int x1 = x + 1; x1 <= x + 9; x1++) {
        setBlockAt(x1, y + 4, z, WISTERIA_WOOD_X);
        setBlockAt(x1, y + 4, z + 9, WISTERIA_WOOD_X);
        setBlockAt(x1, y + 6, z, WISTERIA_WOOD_X);
        setBlockAt(x1, y + 6, z + 9, WISTERIA_WOOD_X);
    }
    for (int z1 = z + 1; z1 <= z + 8; z1++) {
        setBlockAt(x, y + 4, z1, WISTERIA_WOOD_Z);
        setBlockAt(x + 10, y + 4, z1, WISTERIA_WOOD_Z);
        setBlockAt(x, y + 6, z1, WISTERIA_WOOD_Z);
        setBlockAt(x + 10, y + 6, z1, WISTERIA_WOOD_Z);

        setBlockAt(x + 2, y + 6, z1, WISTERIA_WOOD_Z);
        setBlockAt(x + 4, y + 6, z1, WISTERIA_WOOD_Z);
        setBlockAt(x + 6, y + 6, z1, WISTERIA_WOOD_Z);
        setBlockAt(x + 8, y + 6, z1, WISTERIA_WOOD_Z);
    }
    setBlockAt(x + 10, y + 1, z + 1, EMPTY);
    setBlockAt(x + 10, y + 2, z + 1, EMPTY);
    setBlockAt(x + 10, y + 1, z + 2, WISTERIA_WINDOW_X);
    setBlockAt(x + 10, y + 2, z + 2, WISTERIA_WINDOW_X);

    // roof
    for (int x3 = x - 1; x3 <= z + 11; x3++) {
        int dx = -2;
        for (int y3 = y + 6; y3 <= y + 9; y3++) {
            setBlockAt(x3, y3, z + dx, ROOF_TILES_1);
            setBlockAt(x3, y3, z + 9 - dx, ROOF_TILES_1);
            dx++;
            setBlockAt(x3, y3, z + dx, ROOF_TILES);
            setBlockAt(x3, y3, z + 9 - dx, ROOF_TILES);
            dx++;
        }
    }

    // bed
    for (int x1 = x + 1; x1 <= x + 5; x1++) {
        for (int z1 = z + 1; z1 <= z + 6; z1++) {
            setBlockAt(x1, y + 1, z1, WISTERIA_PLANKS_1);
        }
    }
    setBlockAt(x + 1, y + 1, z + 1, WISTERIA_PLANKS);
    setBlockAt(x + 1, y + 1, z + 2, WISTERIA_PLANKS);
    setBlockAt(x + 1, y + 1, z + 3, WISTERIA_PLANKS);
    setBlockAt(x + 1, y + 1, z + 4, WISTERIA_PLANKS);
    setBlockAt(x + 1, y + 1, z + 5, WISTERIA_PLANKS);
    setBlockAt(x + 1, y + 1, z + 6, WISTERIA_PLANKS);

    setBlockAt(x + 1, y + 2, z + 4, WISTERIA_PLANKS_1);
    setBlockAt(x + 1, y + 2, z + 5, WISTERIA_PLANKS_1);
    setBlockAt(x + 1, y + 2, z + 6, WISTERIA_PLANKS_1);
    setBlockAt(x + 1, y + 2, z + 7, WISTERIA_PLANKS_1);
    setBlockAt(x + 1, y + 2, z + 8, WISTERIA_PLANKS_1);

    setBlockAt(x + 2, y + 1, z + 4, CLOTH_7);
    setBlockAt(x + 2, y + 1, z + 5, CLOTH_7);
    setBlockAt(x + 3, y + 1, z + 4, CLOTH_6);
    setBlockAt(x + 3, y + 1, z + 5, CLOTH_6);
    setBlockAt(x + 4, y + 1, z + 4, CLOTH_6);
    setBlockAt(x + 4, y + 1, z + 5, CLOTH_6);

    setBlockAt(x + 1, y + 2, z + 1, PLUM_BLOSSOM_IKEBANA);
    setBlockAt(x + 1, y + 3, z + 4, PAINTING_6R_XP);
    setBlockAt(x + 1, y + 3, z + 5, PAINTING_6L_XP);
    setBlockAt(x + 1, y + 2, z + 3, PAPER_LANTERN);

    setBlockAt(x + 9, y + 1, z + 8, WOOD_LANTERN);
    setBlockAt(x + 9, y + 2, z + 8, LOTUS_IKEBANA);
}

void Chunk::createTeaHouse(int x, int y, int z) {
    for (int x1 = x - 1; x1 <= x + 16; x1++) {
        for (int z1 = z - 1; z1 <= z + 11; z1++) {
            for (int y1 = y; y1 <= y + 15; y1++) {
                setBlockAt(x1, y1, z1, EMPTY);
            }
        }
    }


    // platform
    for (int x1 = x - 2; x1 <= x + 16; x1++) {
        for (int z1 = z - 2; z1 <= z + 11; z1++) {
            setBlockAt(x1, y, z1, PINE_PLANKS);
        }
    }

    // floor
    for (int x2 = x + 1; x2 <= x + 13; x2++) {
        for (int z2 = z + 1; z2 <= z + 8; z2++) {
            setBlockAt(x2, y + 1, z2, CEDAR_PLANKS);
        }
    }

    // walls
    for (int y3 = y + 2; y3 <= y + 7; y3++) {
        for (int x3 = x + 1; x3 <= x + 13; x3++) {
            setBlockAt(x3, y3, z, PLASTER);
            setBlockAt(x3, y3, z + 9, CEDAR_WINDOW_Z);
        }
        for (int z3 = z + 1; z3 <= z + 8; z3++) {
            setBlockAt(x, y3, z3, PLASTER);
            setBlockAt(x + 14, y3, z3, CEDAR_WINDOW_X);
        }
    }
    setBlockAt(x + 10, y + 2, z + 9, PLASTER);
    setBlockAt(x + 11, y + 2, z + 9, PLASTER);
    setBlockAt(x + 12, y + 2, z + 9, PLASTER);
    setBlockAt(x + 13, y + 2, z + 9, PLASTER);
    setBlockAt(x + 10, y + 3, z + 9, PLASTER);
    setBlockAt(x + 10, y + 4, z + 9, PLASTER);
    setBlockAt(x + 13, y + 3, z + 9, PLASTER);
    setBlockAt(x + 13, y + 4, z + 9, PLASTER);
    setBlockAt(x + 10, y + 5, z + 9, PLASTER);
    setBlockAt(x + 11, y + 5, z + 9, PLASTER);
    setBlockAt(x + 12, y + 5, z + 9, PLASTER);
    setBlockAt(x + 13, y + 5, z + 9, PLASTER);

    for (int x3 = x + 1; x3 <= x + 13; x3++) {
        setBlockAt(x3, y + 1, z, CEDAR_PLANKS);
        setBlockAt(x3, y + 1, z + 9, CEDAR_PLANKS);
        setBlockAt(x3, y + 6, z, CEDAR_PLANKS);
        setBlockAt(x3, y + 6, z + 9, CEDAR_PLANKS);
        setBlockAt(x3, y + 8, z, CEDAR_PLANKS);
        setBlockAt(x3, y + 8, z + 9, CEDAR_PLANKS);
    }
    for (int z3 = z + 1; z3 <= z + 8; z3++) {
        setBlockAt(x, y + 1, z3, CEDAR_PLANKS);
        setBlockAt(x + 14, y + 1, z3, CEDAR_PLANKS);
        setBlockAt(x, y + 6, z3, CEDAR_PLANKS);
        setBlockAt(x + 14, y + 6, z3, CEDAR_PLANKS);
        setBlockAt(x, y + 8, z3, CEDAR_PLANKS);
        setBlockAt(x + 14, y + 8, z3, CEDAR_PLANKS);
    }
    for (int y3 = y + 1; y3 <= y + 8; y3++) {
        setBlockAt(x, y3, z, CEDAR_PLANKS);
        setBlockAt(x + 14, y3, z, CEDAR_PLANKS);
        setBlockAt(x, y3, z + 9, CEDAR_PLANKS);
        setBlockAt(x + 14, y3, z + 9, CEDAR_PLANKS);
        setBlockAt(x + 9, y3, z, CEDAR_PLANKS);
        setBlockAt(x + 9, y3, z + 9, CEDAR_PLANKS);
    }

    for (int z3 = z + 2; z3 <= z + 7; z3++) {
        setBlockAt(x, y + 9, z3, PLASTER);
        setBlockAt(x + 14, y + 9, z3, PLASTER);
    }
    setBlockAt(x, y + 10, z + 4, PLASTER);
    setBlockAt(x, y + 10, z + 5, PLASTER);
    setBlockAt(x + 14, y + 10, z + 4, PLASTER);
    setBlockAt(x + 14, y + 10, z + 5, PLASTER);

    // door
    for (int y3 = y + 2; y3 <= y + 5; y3++) {
        for (int z3 = z + 5; z3 <= z + 6; z3++) {
            setBlockAt(x + 14, y3, z3, TEAK_WINDOW_X);
        }
        for (int z3 = z + 3; z3 <= z + 4; z3++) {
            setBlockAt(x + 14, y3, z3, EMPTY);
        }
    }

    // decor
    setBlockAt(x + 3, y + 4, z + 1, PAINTING_1_ZP);
    setBlockAt(x + 4, y + 4, z + 1, PAINTING_2_ZP);
    setBlockAt(x + 5, y + 4, z + 1, PAINTING_3_ZP);
    setBlockAt(x + 8, y + 2, z + 1, PAPER_LANTERN);

    setBlockAt(x + 7, y + 4, z + 1, PAINTING_5_ZP);

    setBlockAt(x + 10, y + 2, z + 1, TEAK_PLANKS);
    setBlockAt(x + 11, y + 2, z + 1, TEAK_PLANKS);
    setBlockAt(x + 12, y + 2, z + 1, WOOD_LANTERN);

    float p1 = Biome::noise1D(glm::vec3(x + 10, y + 3, z + 1));
    if (p1 < 0.1) {
        setBlockAt(x + 10, y + 3, z + 1, CHERRY_BLOSSOM_IKEBANA);
    } else if (p1 < 0.2) {
        setBlockAt(x + 10, y + 3, z + 1, MAGNOLIA_BUD_IKEBANA);
    } else if (p1 < 0.3) {
        setBlockAt(x + 10, y + 3, z + 1, TULIP_IKEBANA);
    } else if (p1 < 0.4) {
        setBlockAt(x + 10, y + 3, z + 1, MAPLE_IKEBANA);
    } else if (p1 < 0.5) {
        setBlockAt(x + 10, y + 3, z + 1, ONCIDIUM_IKEBANA);
    } else if (p1 < 0.6) {
        setBlockAt(x + 10, y + 3, z + 1, DAFFODIL_IKEBANA);
    } else if (p1 < 0.7) {
        setBlockAt(x + 10, y + 3, z + 1, POPPY_IKEBANA);
    } else if (p1 < 0.8) {
        setBlockAt(x + 10, y + 3, z + 1, BLUE_HYDRANGEA_IKEBANA);
    } else if (p1 < 0.9) {
        setBlockAt(x + 10, y + 3, z + 1, GREEN_HYDRANGEA_IKEBANA);
    } else {
        setBlockAt(x + 10, y + 3, z + 1, LOTUS_IKEBANA);
    }

    setBlockAt(x + 9, y + 2, z + 8, WOOD_LANTERN);
    setBlockAt(x + 9, y + 3, z + 8, BONSAI_TREE);

    setBlockAt(x + 11, y + 3, z + 1, PAINTING_7B_ZP);
    setBlockAt(x + 11, y + 4, z + 1, PAINTING_7T_ZP);
    setBlockAt(x + 9, y + 2, z + 1, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 2, z + 2, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 2, z + 3, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 3, z + 1, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 3, z + 2, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 3, z + 3, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 4, z + 1, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 4, z + 2, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 4, z + 3, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 5, z + 1, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 5, z + 2, TEAK_WINDOW_X);
    setBlockAt(x + 9, y + 5, z + 3, TEAK_WINDOW_X);

    setBlockAt(x + 1, y + 2, z + 1, TATAMI_ZT);
    setBlockAt(x + 2, y + 2, z + 1, TATAMI_ZB);
    setBlockAt(x + 3, y + 2, z + 1, TATAMI_ZT);
    setBlockAt(x + 4, y + 2, z + 1, TATAMI_ZB);
    setBlockAt(x + 5, y + 2, z + 1, TATAMI_ZT);
    setBlockAt(x + 6, y + 2, z + 1, TATAMI_ZB);

    setBlockAt(x + 1, y + 2, z + 8, TATAMI_ZT);
    setBlockAt(x + 2, y + 2, z + 8, TATAMI_ZB);
    setBlockAt(x + 3, y + 2, z + 8, TATAMI_ZT);
    setBlockAt(x + 4, y + 2, z + 8, TATAMI_ZB);
    setBlockAt(x + 5, y + 2, z + 8, TATAMI_ZT);
    setBlockAt(x + 6, y + 2, z + 8, TATAMI_ZB);

    setBlockAt(x + 1, y + 2, z + 2, TATAMI_XR);
    setBlockAt(x + 1, y + 2, z + 3, TATAMI_XL);
    setBlockAt(x + 1, y + 2, z + 4, TATAMI_XR);
    setBlockAt(x + 1, y + 2, z + 5, TATAMI_XL);
    setBlockAt(x + 1, y + 2, z + 6, TATAMI_XR);
    setBlockAt(x + 1, y + 2, z + 7, TATAMI_XL);

    setBlockAt(x + 6, y + 2, z + 2, TATAMI_XR);
    setBlockAt(x + 6, y + 2, z + 3, TATAMI_XL);
    setBlockAt(x + 6, y + 2, z + 4, TATAMI_XR);
    setBlockAt(x + 6, y + 2, z + 5, TATAMI_XL);
    setBlockAt(x + 6, y + 2, z + 6, TATAMI_XR);
    setBlockAt(x + 6, y + 2, z + 7, TATAMI_XL);

    setBlockAt(x + 2, y + 2, z + 2, TATAMI_ZT);
    setBlockAt(x + 3, y + 2, z + 2, TATAMI_ZB);
    setBlockAt(x + 4, y + 2, z + 2, TATAMI_ZT);
    setBlockAt(x + 5, y + 2, z + 2, TATAMI_ZB);

    setBlockAt(x + 2, y + 2, z + 7, TATAMI_ZT);
    setBlockAt(x + 3, y + 2, z + 7, TATAMI_ZB);
    setBlockAt(x + 4, y + 2, z + 7, TATAMI_ZT);
    setBlockAt(x + 5, y + 2, z + 7, TATAMI_ZB);

    setBlockAt(x + 2, y + 2, z + 3, TATAMI_XR);
    setBlockAt(x + 2, y + 2, z + 4, TATAMI_XL);
    setBlockAt(x + 2, y + 2, z + 5, TATAMI_XR);
    setBlockAt(x + 2, y + 2, z + 6, TATAMI_XL);

    setBlockAt(x + 5, y + 2, z + 3, TATAMI_XR);
    setBlockAt(x + 5, y + 2, z + 4, TATAMI_XL);
    setBlockAt(x + 5, y + 2, z + 5, TATAMI_XR);
    setBlockAt(x + 5, y + 2, z + 6, TATAMI_XL);

    setBlockAt(x + 3, y + 2, z + 3, TATAMI_ZT);
    setBlockAt(x + 4, y + 2, z + 3, TATAMI_ZB);

    setBlockAt(x + 3, y + 2, z + 6, TATAMI_ZT);
    setBlockAt(x + 4, y + 2, z + 6, TATAMI_ZB);

    setBlockAt(x + 3, y + 2, z + 4, TATAMI_XR);
    setBlockAt(x + 3, y + 2, z + 5, TATAMI_XL);

    setBlockAt(x + 4, y + 2, z + 4, TATAMI_XR);
    setBlockAt(x + 4, y + 2, z + 5, TATAMI_XL);

    // roof
    for (int x3 = x - 1; x3 <= x + 15; x3++) {
        int dx = -2;
        for (int y3 = y + 8; y3 <= y + 11; y3++) {
            setBlockAt(x3, y3, z + dx, ROOF_TILES_1);
            setBlockAt(x3, y3, z + 9 - dx, ROOF_TILES_1);
            dx++;
            setBlockAt(x3, y3, z + dx, ROOF_TILES);
            setBlockAt(x3, y3, z + 9 - dx, ROOF_TILES);
            dx++;
        }
    }
}

void Chunk::createConifer1(int x, int y, int z, BlockType leaf, BlockType wood) {
    // leaves
    for (int x3 = x - 1; x3 <= x + 1; x3++) {
        for (int z3 = z - 1; z3 <= z + 1; z3++) {
            setBlockAt(x3, y + 3, z3, leaf);
            setBlockAt(x3, y + 5, z3, leaf);
            setBlockAt(x3, y + 7, z3, leaf);
        }
    }
    setBlockAt(x - 1, y + 3, z - 1, EMPTY);
    setBlockAt(x - 1, y + 3, z + 1, EMPTY);
    setBlockAt(x + 1, y + 3, z - 1, EMPTY);
    setBlockAt(x + 1, y + 3, z + 1, EMPTY);
    setBlockAt(x - 1, y + 5, z - 1, EMPTY);
    setBlockAt(x - 1, y + 5, z + 1, EMPTY);
    setBlockAt(x + 1, y + 5, z - 1, EMPTY);
    setBlockAt(x + 1, y + 5, z + 1, EMPTY);
    setBlockAt(x - 1, y + 7, z - 1, EMPTY);
    setBlockAt(x - 1, y + 7, z + 1, EMPTY);
    setBlockAt(x + 1, y + 7, z - 1, EMPTY);
    setBlockAt(x + 1, y + 7, z + 1, EMPTY);

    for (int x2 = x - 2; x2 <= x + 2; x2++) {
        for (int z2 = z - 2; z2 <= z + 2; z2++) {
            setBlockAt(x2, y + 2, z2, leaf);
            setBlockAt(x2, y + 4, z2, leaf);
        }
    }
    setBlockAt(x - 2, y + 2, z - 2, EMPTY);
    setBlockAt(x - 2, y + 2, z + 2, EMPTY);
    setBlockAt(x + 2, y + 2, z - 2, EMPTY);
    setBlockAt(x + 2, y + 2, z + 2, EMPTY);
    setBlockAt(x - 2, y + 4, z - 2, EMPTY);
    setBlockAt(x - 2, y + 4, z + 2, EMPTY);
    setBlockAt(x + 2, y + 4, z - 2, EMPTY);
    setBlockAt(x + 2, y + 4, z + 2, EMPTY);

    for (int x1 = x - 3; x1 <= x + 3; x1++) {
        for (int z1 = z - 3; z1 <= z + 3; z1++) {
            setBlockAt(x1, y + 1, z1, leaf);
        }
    }
    setBlockAt(x - 3, y + 1, z - 3, EMPTY);
    setBlockAt(x - 3, y + 1, z + 3, EMPTY);
    setBlockAt(x + 3, y + 1, z - 3, EMPTY);
    setBlockAt(x + 3, y + 1, z + 3, EMPTY);

    // trunk
    for (int y1 = y; y1 <= y + 6; y1++) {
        setBlockAt(x, y1, z, wood);
    }
}
void Chunk::createConifer2(int x, int y, int z, BlockType leaf, BlockType wood) {
    for (int x3 = x - 1; x3 <= x + 1; x3++) {
        for (int z3 = z - 1; z3 <= z + 1; z3++) {
            setBlockAt(x3, y + 1, z3, leaf);
            setBlockAt(x3, y + 3, z3, leaf);
            setBlockAt(x3, y + 5, z3, leaf);
            setBlockAt(x3, y + 7, z3, leaf);
            setBlockAt(x3, y + 9, z3, leaf);
        }
    }
    setBlockAt(x - 1, y + 1, z - 1, EMPTY);
    setBlockAt(x - 1, y + 1, z + 1, EMPTY);
    setBlockAt(x + 1, y + 1, z - 1, EMPTY);
    setBlockAt(x + 1, y + 1, z + 1, EMPTY);
    setBlockAt(x - 1, y + 3, z - 1, EMPTY);
    setBlockAt(x - 1, y + 3, z + 1, EMPTY);
    setBlockAt(x + 1, y + 3, z - 1, EMPTY);
    setBlockAt(x + 1, y + 3, z + 1, EMPTY);
    setBlockAt(x - 1, y + 5, z - 1, EMPTY);
    setBlockAt(x - 1, y + 5, z + 1, EMPTY);
    setBlockAt(x + 1, y + 5, z - 1, EMPTY);
    setBlockAt(x + 1, y + 5, z + 1, EMPTY);
    setBlockAt(x - 1, y + 7, z - 1, EMPTY);
    setBlockAt(x - 1, y + 7, z + 1, EMPTY);
    setBlockAt(x + 1, y + 7, z - 1, EMPTY);
    setBlockAt(x + 1, y + 7, z + 1, EMPTY);
    setBlockAt(x - 1, y + 9, z - 1, EMPTY);
    setBlockAt(x - 1, y + 9, z + 1, EMPTY);
    setBlockAt(x + 1, y + 9, z - 1, EMPTY);
    setBlockAt(x + 1, y + 9, z + 1, EMPTY);

    for (int x2 = x - 2; x2 <= x + 2; x2++) {
        for (int z2 = z - 2; z2 <= z + 2; z2++) {
            setBlockAt(x2, y + 2, z2, leaf);
            setBlockAt(x2, y + 4, z2, leaf);
            setBlockAt(x2, y + 6, z2, leaf);
        }
    }
    setBlockAt(x - 2, y + 2, z - 2, EMPTY);
    setBlockAt(x - 2, y + 2, z + 2, EMPTY);
    setBlockAt(x + 2, y + 2, z - 2, EMPTY);
    setBlockAt(x + 2, y + 2, z + 2, EMPTY);
    setBlockAt(x - 2, y + 4, z - 2, EMPTY);
    setBlockAt(x - 2, y + 4, z + 2, EMPTY);
    setBlockAt(x + 2, y + 4, z - 2, EMPTY);
    setBlockAt(x + 2, y + 4, z + 2, EMPTY);
    setBlockAt(x - 2, y + 6, z - 2, EMPTY);
    setBlockAt(x - 2, y + 6, z + 2, EMPTY);
    setBlockAt(x + 2, y + 6, z - 2, EMPTY);
    setBlockAt(x + 2, y + 6, z + 2, EMPTY);

    // trunk
    for (int y1 = y; y1 <= y + 8; y1++) {
        setBlockAt(x, y1, z, wood);
    }
}
void Chunk::createConifer3(int x, int y, int z, BlockType leaf, BlockType wood) {

    for (int y3 = y + 1; y3 <= y + 5; y3++) {
        for (int x3 = x - 1; x3 <= x + 1; x3++) {
            for (int z3 = z - 1; z3 <= z + 1; z3++) {
                setBlockAt(x3, y3, z3, leaf);
                setBlockAt(x3, y3 + 1, z3, SNOW_1);
            }
        }
    }
    setBlockAt(x + 2, y + 2, z, leaf);
    setBlockAt(x, y + 2, z - 2, leaf);
    setBlockAt(x - 2, y + 2, z, leaf);
    setBlockAt(x, y + 2, z - 2, leaf);
    setBlockAt(x + 2, y + 4, z, leaf);
    setBlockAt(x, y + 4, z - 2, leaf);
    setBlockAt(x - 2, y + 4, z, leaf);
    setBlockAt(x, y + 4, z - 2, leaf);

    setBlockAt(x - 1, y + 1, z - 1, EMPTY);
    setBlockAt(x - 1, y + 1, z + 1, EMPTY);
    setBlockAt(x + 1, y + 1, z - 1, EMPTY);
    setBlockAt(x + 1, y + 1, z + 1, EMPTY);
    setBlockAt(x - 1, y + 3, z - 1, EMPTY);
    setBlockAt(x - 1, y + 3, z + 1, EMPTY);
    setBlockAt(x + 1, y + 3, z - 1, EMPTY);
    setBlockAt(x + 1, y + 3, z + 1, EMPTY);
    setBlockAt(x - 1, y + 5, z - 1, EMPTY);
    setBlockAt(x - 1, y + 5, z + 1, EMPTY);
    setBlockAt(x + 1, y + 5, z - 1, EMPTY);
    setBlockAt(x + 1, y + 5, z + 1, EMPTY);
    setBlockAt(x - 1, y + 2, z - 1, EMPTY);
    setBlockAt(x - 1, y + 2, z + 1, EMPTY);
    setBlockAt(x + 1, y + 2, z - 1, EMPTY);
    setBlockAt(x + 1, y + 2, z + 1, EMPTY);
    setBlockAt(x - 1, y + 4, z - 1, EMPTY);
    setBlockAt(x - 1, y + 4, z + 1, EMPTY);
    setBlockAt(x + 1, y + 4, z - 1, EMPTY);
    setBlockAt(x + 1, y + 4, z + 1, EMPTY);
    setBlockAt(x - 1, y + 6, z - 1, EMPTY);
    setBlockAt(x - 1, y + 6, z + 1, EMPTY);
    setBlockAt(x + 1, y + 6, z - 1, EMPTY);
    setBlockAt(x + 1, y + 6, z + 1, EMPTY);

    setBlockAt(x, y + 6, z, leaf);

    // trunk
    for (int y1 = y; y1 <= y + 5; y1++) {
        setBlockAt(x, y1, z, wood);
    }
}
void Chunk::createDeciduous1(int x, int y, int z, BlockType leaf, BlockType wood) {
    for (int y2 = y + 3; y2 <= y + 4; y2++) {
        for (int x2 = x - 2; x2 <= x + 2; x2++) {
            for (int z2 = z - 2; z2 <= z + 2; z2++) {
                setBlockAt(x2, y2, z2, leaf);
            }
        }
        setBlockAt(x - 2, y2, z - 2, EMPTY);
        setBlockAt(x + 2, y2, z - 2, EMPTY);
        setBlockAt(x - 2, y2, z + 2, EMPTY);
        setBlockAt(x + 2, y2, z + 2, EMPTY);
    }

    for (int y3 = y + 5; y3 <= y + 6; y3++) {
        for (int x3 = x - 1; x3 <= x + 1; x3++) {
            for (int z3 = z - 1; z3 <= z + 1; z3++) {
                setBlockAt(x3, y3, z3, leaf);
            }
        }
    }
    setBlockAt(x - 1, y + 6, z - 1, EMPTY);
    setBlockAt(x - 1, y + 6, z + 1, EMPTY);
    setBlockAt(x + 1, y + 6, z - 1, EMPTY);
    setBlockAt(x + 1, y + 6, z + 1, EMPTY);

    for (int y1 = y; y1 <= y + 5; y1++) {
        setBlockAt(x, y1, z, wood);
    }
}
void Chunk::createDeciduous2(int x, int y, int z, BlockType leaf, BlockType wood) {
    for (int y2 = y + 2; y2 <= y + 3; y2++) {
        for (int x2 = x - 3; x2 <= x + 3; x2++) {
            for (int z2 = z - 3; z2 <= z + 3; z2++) {
                setBlockAt(x2, y2, z2, leaf);
            }
        }
        setBlockAt(x - 3, y2, z - 3, EMPTY);
        setBlockAt(x + 3, y2, z - 3, EMPTY);
        setBlockAt(x - 3, y2, z + 3, EMPTY);
        setBlockAt(x + 3, y2, z + 3, EMPTY);
    }

    for (int y3 = y + 4; y3 <= y + 5; y3++) {
        for (int x3 = x - 2; x3 <= x + 2; x3++) {
            for (int z3 = z - 2; z3 <= z + 2; z3++) {
                setBlockAt(x3, y3, z3, leaf);
            }
        }
    }
    setBlockAt(x - 2, y + 5, z - 2, EMPTY);
    setBlockAt(x - 2, y + 5, z + 2, EMPTY);
    setBlockAt(x + 2, y + 5, z - 2, EMPTY);
    setBlockAt(x + 2, y + 5, z + 2, EMPTY);

    for (int y1 = y; y1 <= y + 4; y1++) {
        setBlockAt(x, y1, z, wood);
    }
}
void Chunk::createDeciduous3(int x, int y, int z, BlockType leaf, BlockType wood) {
    for (int x1 = x - 2; x1 <= x + 2; x1++) {
        for (int z1 = z - 2; z1 <= z + 2; z1++) {
            setBlockAt(x1, y + 1, z1, leaf);
        }
    }
    setBlockAt(x - 2, y + 1, z + 2, EMPTY);
    setBlockAt(x + 2, y + 1, z + 2, EMPTY);
    setBlockAt(x + 2, y + 1, z - 2, EMPTY);

    for (int x2 = x - 1; x2 <= x + 1; x2++) {
        for (int z2 = z - 1; z2 <= z + 1; z2++) {
            setBlockAt(x2, y + 2, z2, leaf);
        }
    }
    setBlockAt(x + 1, y + 2, z - 1, EMPTY);
    setBlockAt(x, y + 3, z, leaf);
    setBlockAt(x, y, z, wood);
    setBlockAt(x, y + 1, z, wood);
}

