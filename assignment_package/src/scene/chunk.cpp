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

void Chunk::createFaceVBOData(std::vector<Vertex>& verts, int currX, int currY, int currZ, DirectionVector dirVec, BlockType bt) {
    int biome = 0;
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
                            Chunk::createFaceVBOData(faceVerts, x, y, z, dv, currType);

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
