#include "chunk.h"


Chunk::Chunk(OpenGLContext* context) : Drawable(context), m_blocks(), m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);

//    btToUV[GRASS] = glm::vec2(0, 11);
//    btToBlockIdx[GRASS] = glm::vec3(0, 0, 1);
//    btToUV[DIRT] = glm::vec2(0, 13);
//    btToBlockIdx[DIRT] = glm::vec3(1, 0, 0);
//    btToUV[STONE] = glm::vec2(1, 15);
//    btToBlockIdx[STONE] = glm::vec3(2, 0, 0);
//    btToUV[COBBLESTONE] = glm::vec2(1, 15);
//    btToBlockIdx[COBBLESTONE] = glm::vec3(2, 0, 0);
//    btToUV[MOSS_STONE] = glm::vec2(3, 15);
//    btToBlockIdx[MOSS_STONE] = glm::vec3(4, 0, 0);
//    btToUV[SAND] = glm::vec2(0, 14);
//    btToBlockIdx[SAND] = glm::vec3(5, 0, 0);
//    btToUV[TALL_GRASS] = glm::vec2(0, 10);
//    btToBlockIdx[TALL_GRASS] = glm::vec3(6, 3, 0);
//    btToUV[WATER] = glm::vec2(13, 3);
//    btToBlockIdx[WATER] = glm::vec3(7, 6, 1);
//    btToUV[LAVA] = glm::vec2(13, 1);
//    btToBlockIdx[LAVA] = glm::vec3(8, 6, 2);
//    btToUV[BEDROCK] = glm::vec2(2, 15);
//    btToBlockIdx[BEDROCK] = glm::vec3(9, 0, 0);
//    btToUV[SNOW_1] = glm::vec2(1, 14);
//    btToBlockIdx[SNOW_1] = glm::vec3(10, 1, 1);
//    btToUV[SNOW_2] = glm::vec2(1, 14);
//    btToBlockIdx[SNOW_2] = glm::vec3(11, 1, 2);
//    btToUV[SNOW_3] = glm::vec2(1, 14);
//    btToBlockIdx[SNOW_3] = glm::vec3(12, 1, 3);
//    btToUV[SNOW_4] = glm::vec2(1, 14);
//    btToBlockIdx[SNOW_4] = glm::vec3(13, 1, 4);
//    btToUV[SNOW_5] = glm::vec2(1, 14);
//    btToBlockIdx[SNOW_5] = glm::vec3(14, 1, 5);
//    btToUV[SNOW_6] = glm::vec2(1, 14);
//    btToBlockIdx[SNOW_6] = glm::vec3(15, 1, 6);
//    btToUV[SNOW_7] = glm::vec2(1, 14);
//    btToBlockIdx[SNOW_7] = glm::vec3(16, 1, 7);
//    btToUV[SNOW_8] = glm::vec2(1, 14);
//    btToBlockIdx[SNOW_8] = glm::vec3(17, 0, 0);
//    btToUV[ICE] = glm::vec2(2, 14);
//    btToBlockIdx[ICE] = glm::vec3(18, 0, 0);

//    btToUV[RED_PAINTED_WOOD] = glm::vec2(4, 15);
//    btToBlockIdx[RED_PAINTED_WOOD] = glm::vec3(19, 0, 0);
//    btToUV[BLACK_PAINTED_WOOD] = glm::vec2(4, 15);
//    btToBlockIdx[BLACK_PAINTED_WOOD] = glm::vec3(20, 0, 0);
//    btToUV[PLASTER] = glm::vec2(11, 12);
//    btToBlockIdx[PLASTER] = glm::vec3(21, 0, 0);
//    btToUV[ROOF_TILES] = glm::vec2(11, 15);
//    btToBlockIdx[ROOF_TILES] = glm::vec3(22, 1, 2);
//    btToUV[STRAW] = glm::vec2(11, 13);
//    btToBlockIdx[STRAW] = glm::vec3(23, 1, 2);

//    btToUV[CEDAR_WOOD] = glm::vec2(5, 12);
//    btToBlockIdx[CEDAR_WOOD] = glm::vec3(24, 0, 0);
//    btToUV[TEAK_WOOD] = glm::vec2(6, 12);
//    btToBlockIdx[TEAK_WOOD] = glm::vec3(25, 0, 0);
//    btToUV[CHERRY_WOOD] = glm::vec2(7, 12);
//    btToBlockIdx[CHERRY_WOOD] = glm::vec3(26, 0, 0);
//    btToUV[MAPLE_WOOD] = glm::vec2(8, 12);
//    btToBlockIdx[MAPLE_WOOD] = glm::vec3(27, 0, 0);
//    btToUV[PINE_WOOD] = glm::vec2(9, 12);
//    btToBlockIdx[PINE_WOOD] = glm::vec3(28, 0, 0);
//    btToUV[WISTERIA_WOOD] = glm::vec2(10, 12);
//    btToBlockIdx[WISTERIA_WOOD] = glm::vec3(29, 0, 0);

//    btToUV[CEDAR_LEAVES] = glm::vec2(5, 11);
//    btToBlockIdx[CEDAR_LEAVES] = glm::vec3(30, 0, 2);
//    btToUV[TEAK_LEAVES] = glm::vec2(6, 11);
//    btToBlockIdx[CEDAR_LEAVES] = glm::vec3(31, 0, 2);
//    btToUV[CHERRY_BLOSSOMS_1] = glm::vec2(7, 8);
//    btToBlockIdx[CHERRY_BLOSSOMS_1] = glm::vec3(32, 0, 2);
//    btToUV[CHERRY_BLOSSOMS_2] = glm::vec2(7, 9);
//    btToBlockIdx[CHERRY_BLOSSOMS_2] = glm::vec3(33, 0, 2);
//    btToUV[CHERRY_BLOSSOMS_3] = glm::vec2(7, 10);
//    btToBlockIdx[CHERRY_BLOSSOMS_3] = glm::vec3(34, 0, 2);
//    btToUV[CHERRY_BLOSSOMS_4] = glm::vec2(7, 11);
//    btToBlockIdx[CHERRY_BLOSSOMS_4] = glm::vec3(35, 0, 2);
//    btToUV[MAPLE_LEAVES_1] = glm::vec2(8, 9);
//    btToBlockIdx[MAPLE_LEAVES_1] = glm::vec3(36, 0, 2);
//    btToUV[MAPLE_LEAVES_2] = glm::vec2(8, 10);
//    btToBlockIdx[MAPLE_LEAVES_2] = glm::vec3(37, 0, 2);
//    btToUV[MAPLE_LEAVES_3] = glm::vec2(8, 11);
//    btToBlockIdx[MAPLE_LEAVES_3] = glm::vec3(38, 0, 2);
//    btToUV[PINE_LEAVES] = glm::vec2(9, 11);
//    btToBlockIdx[PINE_LEAVES] = glm::vec3(39, 0, 2);
//    btToUV[WISTERIA_BLOSSOMS_1] = glm::vec2(10, 9);
//    btToBlockIdx[WISTERIA_BLOSSOMS_1] = glm::vec3(40, 0, 2);
//    btToUV[WISTERIA_BLOSSOMS_2] = glm::vec2(10, 10);
//    btToBlockIdx[WISTERIA_BLOSSOMS_2] = glm::vec3(41, 0, 2);
//    btToUV[WISTERIA_BLOSSOMS_3] = glm::vec2(10, 11);
//    btToBlockIdx[WISTERIA_BLOSSOMS_3] = glm::vec3(42, 0, 2);

//    btToUV[CEDAR_PLANKS] = glm::vec2(5, 15);
//    btToBlockIdx[CEDAR_PLANKS] = glm::vec3(43, 0, 0);
//    btToUV[TEAK_PLANKS] = glm::vec2(6, 15);
//    btToBlockIdx[CEDAR_PLANKS] = glm::vec3(44, 0, 0);
//    btToUV[CHERRY_PLANKS] = glm::vec2(7, 15);
//    btToBlockIdx[CHERRY_PLANKS] = glm::vec3(45, 0, 0);
//    btToUV[MAPLE_PLANKS] = glm::vec2(8, 15);
//    btToBlockIdx[MAPLE_PLANKS] = glm::vec3(46, 0, 0);
//    btToUV[PINE_PLANKS] = glm::vec2(9, 15);
//    btToBlockIdx[PINE_PLANKS] = glm::vec3(47, 0, 0);
//    btToUV[WISTERIA_PLANKS] = glm::vec2(10, 15);
//    btToBlockIdx[WISTERIA_PLANKS] = glm::vec3(48, 0, 0);

//    btToUV[CEDAR_WINDOW] = glm::vec2(5, 14);
//    btToBlockIdx[CEDAR_WINDOW] = glm::vec3(49, 1, 9);
//    btToUV[TEAK_WINDOW] = glm::vec2(6, 14);
//    btToBlockIdx[CEDAR_WINDOW] = glm::vec3(50, 1, 9);
//    btToUV[CHERRY_WINDOW] = glm::vec2(7, 14);
//    btToBlockIdx[CHERRY_WINDOW] = glm::vec3(51, 1, 9);
//    btToUV[MAPLE_WINDOW] = glm::vec2(8, 14);
//    btToBlockIdx[MAPLE_WINDOW] = glm::vec3(52, 1, 9);
//    btToUV[PINE_WINDOW] = glm::vec2(9, 14);
//    btToBlockIdx[PINE_WINDOW] = glm::vec3(53, 1, 9);
//    btToUV[WISTERIA_WINDOW] = glm::vec2(10, 14);
//    btToBlockIdx[WISTERIA_WINDOW] = glm::vec3(54, 1, 9);

//    btToUV[LILY_PAD] = glm::vec2(0, 9);
//    btToBlockIdx[LILY_PAD] = glm::vec3(55, 2, 0);
//    btToUV[LOTUS] = glm::vec2(1, 9);
//    btToBlockIdx[LOTUS] = glm::vec3(56, 3, 0);
//    btToUV[TILLED_DIRT] = glm::vec2(0, 8);
//    btToBlockIdx[TILLED_DIRT] = glm::vec3(57, 1, 8);
//    btToUV[WHEAT] = glm::vec2(0, 7);
//    btToBlockIdx[WHEAT] = glm::vec3(58, 4, 0);
//    btToUV[RICE] = glm::vec2(0, 6);
//    btToBlockIdx[RICE] = glm::vec3(59, 3, 0);
//    btToUV[BAMBOO] = glm::vec2(12, 12);
//    btToBlockIdx[BAMBOO] = glm::vec3(60, 5, 1);

//    btToUV[TATAMI] = glm::vec2(11, 11);
//    btToBlockIdx[TATAMI] = glm::vec3(61, 1, 1);
//    btToUV[PAPER_LANTERN] = glm::vec2(13, 14);
//    btToBlockIdx[PAPER_LANTERN] = glm::vec3(62, 1, 12);
//    btToUV[WOOD_LANTERN] = glm::vec2(13, 12);
//    btToBlockIdx[WOOD_LANTERN] = glm::vec3(63, 1, 11);
//    btToUV[PAINTING] = glm::vec2(13, 5);
//    btToBlockIdx[PAINTING] = glm::vec3(64, 1, 10);
//    btToUV[PAINTING_T] = glm::vec2(12, 11);
//    btToBlockIdx[PAINTING_T] = glm::vec3(65, 1, 10);
//    btToUV[PAINTING_B] = glm::vec2(12, 10);
//    btToBlockIdx[PAINTING_B] = glm::vec3(66, 1, 10);
//    btToUV[PAINTING_L] = glm::vec2(14, 5);
//    btToBlockIdx[PAINTING_L] = glm::vec3(67, 1, 10);
//    btToUV[PAINTING_R] = glm::vec2(15, 5);
//    btToBlockIdx[PAINTING_R] = glm::vec3(68, 1, 10);

//    btToUV[BONSAI_TREE] = glm::vec2(1, 8);
//    btToBlockIdx[BONSAI_TREE] = glm::vec3(69, 5, 4);
//    btToUV[IKEBANA_1] = glm::vec2(0, 5);
//    btToBlockIdx[IKEBANA_1] = glm::vec3(70, 5, 2);
//    btToUV[IKEBANA_2] = glm::vec2(0, 4);
//    btToBlockIdx[IKEBANA_2] = glm::vec3(71, 5, 3);
//    btToUV[IKEBANA_3] = glm::vec2(0, 3);
//    btToBlockIdx[IKEBANA_3] = glm::vec3(72, 5, 4);

//    btToUV[TEA_KETTLE] = glm::vec2(15, 15);
//    btToBlockIdx[TEA_KETTLE] = glm::vec3(73, 7, 1);
//    btToUV[GHOST_LILY] = glm::vec2(2, 9);
//    btToBlockIdx[GHOST_LILY] = glm::vec3(74, 3, 0);
//    btToUV[CORAL] = glm::vec2(3, 9);
//    btToBlockIdx[CORAL] = glm::vec3(75, 3, 0);
//    btToUV[KELP] = glm::vec2(2, 8);
//    btToBlockIdx[KELP] = glm::vec3(76, 3, 0);
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
int Chunk::getBiomeAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_biomes.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
int Chunk::getBiomeAt(int x, int y, int z) const {
    return getBiomeAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
}

void Chunk::setBiomeAt(unsigned int x, unsigned int y, unsigned int z, int b) {
    m_biomes.at(x + 16 * y + 16 * 256 * z) = b;
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

void Chunk::createFaceVBOData(std::vector<Vertex>& verts, int currX, int currY, int currZ, DirectionVector dirVec, BlockType bt, int biome) {
    Direction d = dirVec.dir;
    switch (d) {
        case XPOS: case XNEG:
            if (d == XPOS) {
                currX++;
            }
            verts.push_back(Vertex(glm::vec4(currX, currY, currZ, 1), dirVec.vec, bt, glm::vec2(0, 0), biome));
            verts.push_back(Vertex(glm::vec4(currX, currY, currZ+1, 1), dirVec.vec, bt, glm::vec2(1, 0), biome));
            verts.push_back(Vertex(glm::vec4(currX, currY+1, currZ+1, 1), dirVec.vec, bt, glm::vec2(1, 1), biome));
            verts.push_back(Vertex(glm::vec4(currX, currY+1, currZ, 1), dirVec.vec, bt, glm::vec2(0, 1), biome));
            break;

        case YPOS: case YNEG:
            if (d == YPOS) {
                currY++;
            }
            verts.push_back(Vertex(glm::vec4(currX, currY, currZ, 1), dirVec.vec, bt, glm::vec2(0, 0), biome));
            verts.push_back(Vertex(glm::vec4(currX+1, currY, currZ, 1), dirVec.vec, bt, glm::vec2(1, 0), biome));
            verts.push_back(Vertex(glm::vec4(currX+1, currY, currZ+1, 1), dirVec.vec, bt, glm::vec2(1, 1), biome));
            verts.push_back(Vertex(glm::vec4(currX, currY, currZ+1, 1), dirVec.vec, bt, glm::vec2(0, 1), biome));
            break;

        case ZPOS: case ZNEG:
            if (d == ZPOS) {
                currZ++;
            }
            verts.push_back(Vertex(glm::vec4(currX, currY, currZ, 1), dirVec.vec, bt, glm::vec2(0, 0), biome));
            verts.push_back(Vertex(glm::vec4(currX, currY+1, currZ, 1), dirVec.vec, bt, glm::vec2(1, 0), biome));
            verts.push_back(Vertex(glm::vec4(currX+1, currY+1, currZ, 1), dirVec.vec, bt, glm::vec2(1, 1), biome));
            verts.push_back(Vertex(glm::vec4(currX+1, currY, currZ, 1), dirVec.vec, bt, glm::vec2(0, 1), biome));
            break;
    }
}

void Chunk::createVBOdata() {
    std::vector<GLuint> indices = std::vector<GLuint>();
    std::vector<glm::vec4> vertData = std::vector<glm::vec4>();

    int vertCount = 0;

    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 256; y++) {
            for (int z = 0; z < 16; z++) {
                BlockType currType = this->getBlockAt(x, y, z);
                int currBiome = this->getBiomeAt(x, y, z);

                if (currType != EMPTY) {
                    for (const DirectionVector& dv : directionIter) {
                        glm::ivec3 adjBlockPos = glm::ivec3(x, y, z) + dv.vec;
                        bool inSameChunk = Chunk::isInBounds(adjBlockPos);

                        BlockType adjBlockType;
                        if (inSameChunk) {
                            adjBlockType = this->getBlockAt(adjBlockPos.x, adjBlockPos.y, adjBlockPos.z);
                        } else {
                            adjBlockType = this->getAdjBlockType(dv.dir, adjBlockPos);
                        }

                        if (adjBlockType == EMPTY) {
                            std::vector<Vertex> faceVerts;
                            Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType, currBiome);

                            for (const Vertex& v : faceVerts) {
                                vertData.push_back(v.position);
                                vertData.push_back(v.normal);
                                vertData.push_back(v.color);
                                vertData.push_back(v.uvCoords);
                                vertData.push_back(v.blockType);
                            }

                            indices.push_back(vertCount);
                            indices.push_back(vertCount + 1);
                            indices.push_back(vertCount + 2);
                            indices.push_back(vertCount);
                            indices.push_back(vertCount + 2);
                            indices.push_back(vertCount + 3);

                            vertCount += 4;
                        }
                    }
                }
            }
        }
    }

    m_count = indices.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    generateVertData();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufVertData);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vertData.size() * sizeof(glm::vec4), vertData.data(), GL_STATIC_DRAW);
}

void Chunk::redistributeVertexData(std::vector<glm::vec4> vd, std::vector<GLuint> indices) {
    std::vector<glm::vec4> positions;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec4> uvs;
    std::vector<glm::vec4> bts;

    for (int i = 0; i < vd.size(); i = i+5) {
        positions.push_back(vd[i]);
        normals.push_back(vd[i+1]);
        colors.push_back(vd[i+2]);
        uvs.push_back(vd[i+3]);
        bts.push_back(vd[i+4]);
    }

    m_count = indices.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec4), normals.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);

    generateUVs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufUVs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec4), uvs.data(), GL_STATIC_DRAW);

    generateBTs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufBTs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, bts.size() * sizeof(glm::vec4), bts.data(), GL_STATIC_DRAW);


}
