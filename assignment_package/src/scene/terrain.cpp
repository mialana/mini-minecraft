#include "terrain.h"
#include <stdexcept>
#include <iostream>

Terrain::Terrain(OpenGLContext *context)
    : m_chunks(), m_generatedTerrain(), mp_context(context)
{}

Terrain::~Terrain() {
}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

glm::vec4 Terrain::getBiomeAt(glm::vec2 p) const {
    return getBiomeAt(p.x, p.y);
}

glm::vec4 Terrain::getBiomeAt(int x, int z) const
{
    if(hasChunkAt(x, z)) {
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBiomeAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}


uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

void Terrain::setBiomeAt(int x, int z, glm::vec4 b) {
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBiomeAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      b);
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                std::to_string(z) + " have no Chunk!");
    }
}

Chunk* Terrain::instantiateChunkAt(int x, int z) {
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context);
    Chunk *cPtr = chunk.get();
    m_chunks[toKey(x, z)] = std::move(chunk);
    // Set the neighbor pointers of itself and its neighbors
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
    }
    return cPtr;
    return cPtr;
}

void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            const uPtr<Chunk>& currChunk = getChunkAt(x, z);
            currChunk->createVBOdata();

            shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
            shaderProgram->drawInterleavedO(*currChunk);
        }
    }

    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            const uPtr<Chunk>& currChunk = getChunkAt(x, z);
            currChunk->createVBOdata();

            shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
            shaderProgram->drawInterleavedT(*currChunk);
        }
    }
}

void Terrain::CreateTestScene()
{
    // Create the Chunks that will
    // store the blocks for our
    // initial world space
    for(int x = 0; x < 256; x += 16) {
        for(int z = 0; z < 256; z += 16) {
            instantiateChunkAt(x, z);
        }
    }

    // Tell our existing terrain set that
    // the "generated terrain zone" at (0,0)
    // now exists.
    m_generatedTerrain.insert(toKey(0, 0));

//    for (int x = 0; x < 48; ++x) {
//        for (int z = 0; z < 48; ++z) {

//            float hM = Biome::mountains(glm::vec2(x, z));
//            float hH = Biome::hills(glm::vec2(x, z));
//            float hF = Biome::forest(glm::vec2(x, z));
//            float hI = Biome::islands(glm::vec2(x, z));

//            std::pair<float, BiomeEnum> hb = blendMultipleBiomes(glm::vec2(x, z), hM, hF, hH, hI);
//            float h = hb.first;
//            BiomeEnum b = hb.second;

//            int numDirtBlocks = 10 * Biome::fbm(glm::vec2(x, z));
//            if (b == MOUNTAINS) {
//                if (h < 120) {
//                    for (int y = 0; y < h - numDirtBlocks; ++y) {
//                        setBlockAt(x, y, z, STONE);
//                    }
//                    for (int y = h - numDirtBlocks; y < h; ++y) {
//                        setBlockAt(x, y, z, DIRT);
//                    }
//                    for (int y = h; y < 120; ++y) {
//                        setBlockAt(x, y, z, WATER);
//                    }
//                } else {
//                    for (int y = 0; y < h - numDirtBlocks - 1; ++y) {
//                        setBlockAt(x, y, z, STONE);
//                    }
//                    for (int y = h - numDirtBlocks - 1; y < h - 1; ++y) {
//                        setBlockAt(x, y, z, DIRT);
//                    }
//                    setBlockAt(x, h - 1, z, GRASS);
//                    setBlockAt(x, h, z, SNOW_1);
//                }
//            } else if (b == HILLS) {
//                for (int y = 0; y < h - 3 - numDirtBlocks; ++y) {
//                    setBlockAt(x, y, z, STONE);
//                }
//                for (int currY = h - 3 - numDirtBlocks; currY < h - 1; ++currY) {
//                    setBlockAt(x, currY, z, DIRT);
//                }

//                if (h < 120) {
//                    setBlockAt(x, h - 1, z, DIRT);

//                    for (int y = h; y < 120; ++y) {
//                        setBlockAt(x, y, z, WATER);
//                    }
//                } else {
//                    setBlockAt(x, h - 1, z, GRASS);
//                }
//            } else if (b == FOREST) {
//                for (int y = 0; y < h - numDirtBlocks - 1; ++y) {
//                    setBlockAt(x, y, z, STONE);
//                }
//                for (int y = h - numDirtBlocks - 1; y < h - 1; ++y) {
//                    setBlockAt(x, y, z, DIRT);
//                }

//                if (h < 125) {
//                    setBlockAt(x, h - 1, z, DIRT);

//                    for (int y = h; y < 125; ++y) {
//                        setBlockAt(x, y, z, WATER);
//                    }
//                } else {
//                    setBlockAt(x, h - 1, z, GRASS);
//                }
//            } else if (b == ISLANDS) {
//                for (int y = 0; y < 80; ++y) {
//                    setBlockAt(x, y, z, STONE);
//                }
//                for (int y = 80; y < h; ++y) {
//                    setBlockAt(x, y, z, SAND);
//                }
//                if (h < 125) {
//                    for (int y = h; y < 125; ++y) {
//                        setBlockAt(x, y, z, WATER);
//                    }
//                }
//            }



//            // assets
//            float p1 = Biome::noise1D(glm::vec2(x, z));
//            float p2 = Biome::worley(glm::vec2(x, z));

//            if (getBlockAt(x, h, z) == EMPTY) {

//                // TALL_GRASS
//                if ((b == MOUNTAINS && p1 < 0.15) ||
//                    (b == HILLS && p1 < 0.35) ||
//                    (b == FOREST && p1 < 0.15) ||
//                    (b == ISLANDS && p1 < 0.1)) {

//                    setBlockAt(x, h, z, TALL_GRASS);
//                }

//                // bamboo, trees
//            } else if (getBlockAt(x, h, z) == WATER) {
//                // lotuses, coral, sea grass, kelp, lanterns

//                if (b == MOUNTAINS) {

//                } else if (b == HILLS) {

//                } if (b == ISLANDS && p2 < 0.3 && h < 124) {
//                    if (p1 < 0.1) {
//                        setBlockAt(x, h, z, SEA_GRASS);
//                    } else if (p1 < 0.11) {
//                        setBlockAt(x, h, z, CORAL_1);
//                    } else if (p1 < 0.12) {
//                        setBlockAt(x, h, z, CORAL_2);
//                    } else if (p1 < 0.13) {
//                        setBlockAt(x, h, z, CORAL_3);
//                    } else if (p1 < 0.14) {
//                        setBlockAt(x, h, z, CORAL_4);
//                    } else if (p1 < 0.4) {
//                        int y = h;
//                        bool addHeight = true;
//                        while (y < 123 && addHeight) {
//                            setBlockAt(x, y, z, KELP_1);
//                            y++;
//                            addHeight = (Biome::noise1D(glm::vec3(x, y, z)) < 0.75);
//                        }
//                        setBlockAt(x, y, z, KELP_2);
//                    }
//                }
//            }

//            for (int currY = 1; currY <= 106; currY++) {
//                float cavePerlin3D = Biome::perlin3D(glm::vec3(x, currY, z) * 0.06f);
//                float cavePerlin3DTwo = Biome::perlin3D(glm::vec3(x, currY, glm::mix(x, z, 0.35f)) * 0.06f);

//                if (cavePerlin3D + cavePerlin3DTwo < -0.15f) {
//                    if (currY < 25) {
//                        setBlockAt(x, currY, z, LAVA);
//                    } else {
//                        setBlockAt(x, currY, z, EMPTY);
//                    }
//                }
//            }

//            if (getBlockAt(x, h, z) == WATER) {
//                int y = h - 1;
//                while (getBlockAt(x, y, z) == EMPTY && y > 0) {
//                    setBlockAt(x, y, z, WATER);
//                    y--;
//                }
//            }
//            setBlockAt(x, 0, z, BEDROCK);
//        }
//    }
    createHut(10, 120, 10);
//    createToriiGate(10, 108, 10, 0);
}

void Terrain::loadNewChunks(glm::vec3 currPos) {
    for (const glm::ivec2& p : directionHelper) {
        if (!hasChunkAt(currPos.x + p[0], currPos.z + p[1])) {
            glm::ivec2 currChunkOrigin = 16 * glm::ivec2(glm::floor(glm::vec2(currPos.x, currPos.z) / 16.f));

            glm::ivec2 newChunkOrigin = currChunkOrigin + p;

            Chunk* newChunk = instantiateChunkAt(newChunkOrigin[0], newChunkOrigin[1]);
            newChunk->createVBOdata();
        }
    }
}

std::pair<float, BiomeEnum> Terrain::blendMultipleBiomes(glm::vec2 xz, float forestH, float mountH, float hillH, float islandH) {

    // perform bilinear interpolation

    BiomeEnum b;
    glm::vec4 biomeWts;

    double elev = (Biome::perlin1(xz / 297.f) + 1.f) / 2.f; // remap perlin noise from (-1, 1) to (0, 1)
    double temp = (Biome::perlin2(xz / 308.f) + 1.f) / 2.f;
//    std::cout<<elev<<","<<temp<<std::endl;

    float uLim = 0.5;
    float lLim = 0.5;

    if (elev >= uLim && temp < lLim) {
        b = MOUNTAINS;
//        biomeWts = glm::vec4(1, 0, 0, 0);
    } else if (elev >= uLim && temp >= uLim) {
        b = HILLS;
//        biomeWts = glm::vec4(0, 1, 0, 0);
    } else if (elev < lLim && temp < lLim) {
        b = FOREST;
//        biomeWts = glm::vec4(0, 0, 1, 0);
    } else if (elev < lLim && temp >= uLim) {
        b = ISLANDS;
//        biomeWts = glm::vec4(0, 0, 0, 1);
    } /*else {*/
        // set biome weights in m_biomes for each xz coord in this chunk
        biomeWts.x = elev * (1.f - temp);
        biomeWts.y = elev * temp;
        biomeWts.z = (1.f - elev) * (1.f - temp);
        biomeWts.w = (1.f - elev) * temp;
//    }
    setBiomeAt(xz.x, xz.y, biomeWts);

    if (biomeWts.x + biomeWts.y + biomeWts.z + biomeWts.w != 1) {
        std::cout<< "something is wrong" <<std::endl;
    }

    float h = (biomeWts.x * mountH) +
                (biomeWts.y * hillH) +
                (biomeWts.z * forestH) +
                (biomeWts.w * islandH);
    return std::pair(h, b);
}

void Terrain::createToriiGate(int x, int y, int z, int rot) {
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

void Terrain::createHut(int x, int y, int z) {
    for (int y1 = y; y1 <= y + 3; y1++) {
        setBlockAt(x, y1, z, CHERRY_WOOD_Y);
        setBlockAt(x + 8, y1, z, CHERRY_WOOD_Y);
        setBlockAt(x, y1, z + 8, CHERRY_WOOD_Y);
        setBlockAt(x + 8, y1, z + 8, CHERRY_WOOD_Y);
    }
    for (int x1 = x - 1; x1 <= x + 9; x1++) {
        for (int z1 = z - 1; z1 <= z + 9; z1++) {
            setBlockAt(x1, y + 4, z1, CHERRY_WOOD_Z);
        }
        setBlockAt(x1, y + 4, z - 2, CHERRY_WOOD_X);
        setBlockAt(x1, y + 4, z + 10, CHERRY_WOOD_X);
    }
    setBlockAt(x - 3, y + 4, z - 2, CHERRY_WOOD_X);
    setBlockAt(x - 3, y + 4, z + 10, CHERRY_WOOD_X);
    setBlockAt(x - 2, y + 4, z - 2, CHERRY_WOOD_X);
    setBlockAt(x - 2, y + 4, z + 10, CHERRY_WOOD_X);
    setBlockAt(x + 10, y + 4, z - 2, CHERRY_WOOD_X);
    setBlockAt(x + 10, y + 4, z + 10, CHERRY_WOOD_X);
    setBlockAt(x + 11, y + 4, z - 2, CHERRY_WOOD_X);
    setBlockAt(x + 11, y + 4, z + 10, CHERRY_WOOD_X);


    for (int y2 = y + 5; y2 <= y + 10; y2++) {
        for (int x2 = x; x2 <= x + 8; x2++) {
            setBlockAt(x2, y2, z, CHERRY_WOOD_Y);
            setBlockAt(x2, y2, z + 8, CHERRY_WOOD_Y);
        }
        for (int z2 = z + 1; z2 <= z + 7; z2++) {
            setBlockAt(x, y2, z2, CHERRY_WOOD_Y);
            setBlockAt(x + 8, y2, z2, CHERRY_WOOD_Y);
        }
    }
    for (int x2 = x + 2; x2 <= x + 6; x2++) {
        setBlockAt(x2, y + 11, z, CHERRY_WOOD_Y);
        setBlockAt(x2, y + 11, z + 8, CHERRY_WOOD_Y);
    }
    setBlockAt(x + 4, y + 12, z, CHERRY_WOOD_Y);
    setBlockAt(x + 4, y + 12, z + 8, CHERRY_WOOD_Y);


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

void Terrain::createCottage(int x, int y, int z, int rot) {

}

void Terrain::createTeaHouse(int x, int y, int z, int rot) {

}
