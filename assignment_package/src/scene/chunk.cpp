#include "chunk.h"


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

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
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

boolean Chunk::isHPlane(BlockType bt) {
    return hPlane.find(bt) != hPlane.end();
}
boolean Chunk::isCross2(BlockType bt) {
    return cross2.find(bt) != cross2.end();
}
boolean Chunk::isCross4(BlockType bt) {
    return cross4.find(bt) != cross4.end();
}
boolean Chunk::isPartialX(BlockType bt) {
    return partialX.find(bt) != partialX.end();
}
boolean Chunk::isPartialY(BlockType bt) {
    return partialY.find(bt) != partialY.end();
}
boolean Chunk::isPartialZ(BlockType bt) {
    return partialZ.find(bt) != partialZ.end();
}
boolean Chunk::isFullCube(BlockType bt) {
    return fullCube.find(bt) != fullCube.end();
}
boolean Chunk::isTransparent(BlockType bt) {
    return transparent.find(bt) != transparent.end();
}
boolean Chunk::isVisible(int x, int y, int z, BlockType bt) {
    for (const DirectionVector& dv : directionIter) {
        glm::ivec3 adjBlockPos = glm::ivec3(x, y, z) + dv.vec;
        bool inSameChunk = Chunk::isInBounds(adjBlockPos);

        BlockType adjBlockType;

        if (inSameChunk) {
            adjBlockType = this->getBlockAt(adjBlockPos.x, adjBlockPos.y, adjBlockPos.z);
        } else {
            adjBlockType = this->getAdjBlockType(dv.dir, adjBlockPos);
        }
        if (isTransparent(adjBlockType) && adjBlockType != bt) {
            return true;
        }
    }
    return false;
}
boolean Chunk::isVisible(int x, int y, int z, DirectionVector dv, BlockType bt) {
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

    // if the block adjacent to this face is EMPTY or transparent and is of a different type
    if (isTransparent(adjBlockType) && adjBlockType != bt) {
        return true;
    }

    // if not a cube or partial block
    if (isHPlane(bt) || isCross2(bt) || isCross4(bt)) {
        return true;
    }

    // if this face doesn't reach the bounds of a full block
    if (((d == XPOS || d == XNEG) && isPartialX(bt)) ||
            ((d == YPOS) && isPartialY(bt)) ||
            ((d == ZPOS || d == ZNEG) && isPartialZ(bt))) {
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

void Chunk::createFaceVBOData(std::vector<Vertex>& verts, float currX, float currY, float currZ, DirectionVector dirVec, BlockType bt) {
    Direction d = dirVec.dir;

    // offsets move the face inwardsfloat offsetXPOS = 0.f;
    float offsetXPOS = 1.f;
    float offsetXNEG = 0.f;
    float offsetYPOS = 1.f;
    float offsetYNEG = 0.f;
    float offsetZPOS = 1.f;
    float offsetZNEG = 0.f;
    float offsetDiag = std::sqrt(0.5);
    boolean keepEdges = false; // keep portions of face outside intersection ex: wheat vs lantern

    float x1 = currX;
    float x2 = currX;
    float y1 = currY;
    float y2 = currY;
    float z1 = currZ;
    float z2 = currZ;

    switch(bt) {
        case LILY_PAD: case LOTUS_1: case LOTUS_2:
            offsetYPOS = 0.f;
            break;
        case WHEAT_1: case WHEAT_2: case WHEAT_3: case WHEAT_4: case WHEAT_5: case WHEAT_6: case WHEAT_7: case WHEAT_8: case SEA_GRASS:
            offsetXPOS = 0.25f;
            offsetXNEG = 0.75f;
            offsetZPOS = 0.25f;
            offsetZNEG = 0.75f;
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
            offsetXPOS = 0.4375; // may be offsetX or offsetZ
            offsetXNEG = 0.5625;
            break;
        case TILLED_DIRT: case PATH:
            offsetYPOS = 0.9375;
            break;
        case BAMBOO_1: case BAMBOO_2: case BAMBOO_3:
            offsetXPOS = 0.4375;
            offsetXNEG = 0.5625;
            offsetZPOS = 0.4375;
            offsetZNEG = 0.5625;
            break;
        case PAPER_LANTERN:
            offsetXPOS = 0.1875;
            offsetXNEG = 0.8125;
            offsetZPOS = 0.1875;
            offsetZNEG = 0.8125;
            offsetYPOS = 0.125;
            break;
        case WOOD_LANTERN:
            offsetXPOS = 0.0625;
            offsetXNEG = 0.9375;
            offsetZPOS = 0.0625;
            offsetZNEG = 0.9375;
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
            offsetXPOS = 0.0625;
            offsetXNEG = 0.9375;
            offsetZPOS = 0.0625;
            offsetZNEG = 0.9375;
            offsetYPOS = 0.125;
            break;
        case GREEN_HYDRANGEA_IKEBANA: case CHRYSANTHEMUM_IKEBANA:
            offsetXPOS = 0.25;
            offsetXNEG = 0.75;
            offsetZPOS = 0.25;
            offsetZNEG = 0.75;
            offsetYPOS = 0.25;
            break;
        case CHERRY_BLOSSOM_IKEBANA: case BLUE_HYDRANGEA_IKEBANA: case TULIP_IKEBANA: case DAFFODIL_IKEBANA:
            offsetXPOS = 0.3125;
            offsetXNEG = 0.6875;
            offsetZPOS = 0.3125;
            offsetZNEG = 0.6875;
            offsetYPOS = 0.375;
            break;
        case PLUM_BLOSSOM_IKEBANA: case MAGNOLIA_BUD_IKEBANA: case POPPY_IKEBANA: case MAPLE_IKEBANA: case ONCIDIUM_IKEBANA:
            offsetXPOS = 0.40625;
            offsetXNEG = 0.59375;
            offsetZPOS = 0.40625;
            offsetZNEG = 0.59375;
            offsetYPOS = 0.5;
            break;
        default:
            break;
    }

    switch (d) {
        case XPOS: case XNEG:
            if (d == XNEG) {
                x1 = currX + offsetXNEG;
            } else {
                x1 = currX + offsetXPOS;
            }
            if (!keepEdges) {
                y1 = currY + offsetYNEG;
                y2 = currY + offsetYPOS;
                z1 = currZ + offsetZNEG;
                z2 = currZ + offsetZPOS;
            }
            verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetYNEG, offsetZNEG)));
            verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetYPOS, offsetZNEG)));
            verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetYPOS, offsetZPOS)));
            verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetYNEG, offsetZPOS)));
            break;

        case YPOS: case YNEG:
            if (d == YNEG) {
                y1 = currY + offsetYNEG;
            } else {
                y1 = currY + offsetYPOS;
            }
            if (!keepEdges) {
                x1 = currX + offsetXNEG;
                x2 = currX + offsetXPOS;
                z1 = currZ + offsetZNEG;
                z2 = currZ + offsetZPOS;
            }
            verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetZNEG)));
            verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetZNEG)));
            verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetZPOS)));
            verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetZPOS)));
            break;

        case ZPOS: case ZNEG:
            if (d == ZNEG) {
                z1 = currZ + offsetZNEG;
            } else {
                z1 = currZ + offsetZPOS;
            }
            if (!keepEdges) {
                x1 = currX + offsetXNEG;
                x2 = currX + offsetXPOS;
                y1 = currY + offsetYNEG;
                y2 = currY + offsetYPOS;
            }
            verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetYNEG)));
            verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetYNEG)));
            verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXPOS, offsetYPOS)));
            verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(offsetXNEG, offsetYPOS)));
            break;
        case XPOS_ZPOS:
            x2 += offsetDiag;
            y2 += offsetDiag;
            z2 += offsetDiag;
            verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(0, 0)));
            verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(1, 0)));
            verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(1, 1)));
            verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(0, 1)));
            break;
        case XNEG_ZNEG:
            x2 += offsetDiag;
            y2 += offsetDiag;
            z2 += offsetDiag;
            verts.push_back(Vertex(glm::vec4(x1, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(0, 1)));
            verts.push_back(Vertex(glm::vec4(x2, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(1, 1)));
            verts.push_back(Vertex(glm::vec4(x2, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(1, 0)));
            verts.push_back(Vertex(glm::vec4(x1, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(0, 0)));
            break;
        case XPOS_ZNEG:
            x2 += offsetDiag;
            y2 += offsetDiag;
            z2 += offsetDiag;
            verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(0, 1)));
            verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(1, 1)));
            verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(1, 0)));
            verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(0, 0)));
            break;
        case XNEG_ZPOS:
            x2 += offsetDiag;
            y2 += offsetDiag;
            z2 += offsetDiag;
            verts.push_back(Vertex(glm::vec4(x1, y2, z2, 1), dirVec.vec, bt, d, glm::vec2(0, 0)));
            verts.push_back(Vertex(glm::vec4(x2, y2, z1, 1), dirVec.vec, bt, d, glm::vec2(1, 0)));
            verts.push_back(Vertex(glm::vec4(x2, y1, z1, 1), dirVec.vec, bt, d, glm::vec2(1, 1)));
            verts.push_back(Vertex(glm::vec4(x1, y1, z2, 1), dirVec.vec, bt, d, glm::vec2(0, 1)));
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

                if (currType != EMPTY) {
                    if (isHPlane(currType)) {
                        if (isVisible(x, y, z, currType)) {
                            for (const DirectionVector& dv : planeDirIter) {
                                std::vector<Vertex> faceVerts;
                                Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType);

                                for (const Vertex& v : faceVerts) {
                                    tVertData.push_back(v.position);
                                    tVertData.push_back(v.normal);
                                    tVertData.push_back(v.color);
                                    tVertData.push_back(v.uvCoords);
                                    tVertData.push_back(v.blockType);
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
                                Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType);

                                for (const Vertex& v : faceVerts) {
                                    tVertData.push_back(v.position);
                                    tVertData.push_back(v.normal);
                                    tVertData.push_back(v.color);
                                    tVertData.push_back(v.uvCoords);
                                    tVertData.push_back(v.blockType);
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
                                Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType);

                                for (const Vertex& v : faceVerts) {
                                    tVertData.push_back(v.position);
                                    tVertData.push_back(v.normal);
                                    tVertData.push_back(v.color);
                                    tVertData.push_back(v.uvCoords);
                                    tVertData.push_back(v.blockType);
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
                    if (isPartialX(currType) || isPartialY(currType) || isPartialZ(currType)) {
                        for (const DirectionVector& dv : directionIter) {
                            if (isVisible(x, y, z, dv, currType)) {
                                std::vector<Vertex> faceVerts;
                                Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType);

                                for (const Vertex& v : faceVerts) {
                                    tVertData.push_back(v.position);
                                    tVertData.push_back(v.normal);
                                    tVertData.push_back(v.color);
                                    tVertData.push_back(v.uvCoords);
                                    tVertData.push_back(v.blockType);
                                }
                                tIndices.push_back(oVertCount);
                                tIndices.push_back(oVertCount + 1);
                                tIndices.push_back(oVertCount + 2);
                                tIndices.push_back(oVertCount);
                                tIndices.push_back(oVertCount + 2);
                                tIndices.push_back(oVertCount + 3);

                                tVertCount += 4;
                            }
                        }
                    }
                    if (isFullCube(currType)) {
                        for (const DirectionVector& dv : directionIter) {
                            glm::ivec3 adjBlockPos = glm::ivec3(x, y, z) + dv.vec;
                            bool inSameChunk = Chunk::isInBounds(adjBlockPos);

                            BlockType adjBlockType;
                            if (inSameChunk) {
                                adjBlockType = this->getBlockAt(adjBlockPos.x, adjBlockPos.y, adjBlockPos.z);
                            } else {
                                adjBlockType = this->getAdjBlockType(dv.dir, adjBlockPos);
                            }

                            if (isTransparent(adjBlockType)) {
                                if (!isTransparent(currType)) {
                                    std::vector<Vertex> faceVerts;
                                    Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType);

                                    for (const Vertex& v : faceVerts) {
                                        oVertData.push_back(v.position);
                                        oVertData.push_back(v.normal);
                                        oVertData.push_back(v.color);
                                        oVertData.push_back(v.uvCoords);
                                        oVertData.push_back(v.blockType);
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
                                    Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType);
                                    for (const Vertex& v : faceVerts) {
                                        tVertData.push_back(v.position);
                                        tVertData.push_back(v.normal);
                                        tVertData.push_back(v.color);
                                        tVertData.push_back(v.uvCoords);
                                        tVertData.push_back(v.blockType);
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

    std::vector<glm::vec4> t_positions;
    std::vector<glm::vec4> t_normals;
    std::vector<glm::vec4> t_colors;
    std::vector<glm::vec4> t_uvs;
    std::vector<glm::vec4> t_bts;

    for (int i = 0; i < (int)ovd.size(); i = i+5) {
        o_positions.push_back(ovd[i]);
        o_normals.push_back(ovd[i+1]);
        o_colors.push_back(ovd[i+2]);
        o_uvs.push_back(ovd[i+3]);
        o_bts.push_back(ovd[i+4]);
    }
    for (int i = 0; i < (int)tvd.size(); i = i+5) {
        t_positions.push_back(tvd[i]);
        t_normals.push_back(tvd[i+1]);
        t_colors.push_back(tvd[i+2]);
        t_uvs.push_back(tvd[i+3]);
        t_bts.push_back(tvd[i+4]);
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
}
