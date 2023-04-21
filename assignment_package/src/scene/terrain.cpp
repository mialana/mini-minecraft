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
//            currChunk->createVBOdata();

            shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
            shaderProgram->drawInterleavedO(*currChunk);
        }
    }

    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            const uPtr<Chunk>& currChunk = getChunkAt(x, z);
//            currChunk->createVBOdata();

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

    for (int x = 0; x < 48; ++x) {
        for (int z = 0; z < 48; ++z) {

            float hM = Biome::mountains(glm::vec2(x, z));
            float hH = Biome::hills(glm::vec2(x, z));
            float hF = Biome::forest(glm::vec2(x, z));
            float hI = Biome::islands(glm::vec2(x, z));

            std::pair<float, BiomeEnum> hb = blendMultipleBiomes(glm::vec2(x, z), hM, hF, hH, hI);
            float h = hb.first;
            BiomeEnum b = hb.second;

            int numDirtBlocks = 10 * Biome::fbm(glm::vec2(x, z));
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
                    } else if (snowBar < 0.75) {
                        setBlockAt(x, h, z, SNOW_2);
                    } else if (snowBar < 0.85) {
                        setBlockAt(x, h, z, SNOW_3);
                    } else if (snowBar < 0.9) {
                        setBlockAt(x, h, z, SNOW_4);
                    } else if (snowBar < 0.925) {
                        setBlockAt(x, h, z, SNOW_5);
                    } else if (snowBar < 0.9375) {
                        setBlockAt(x, h, z, SNOW_6);
                    } else if (snowBar < 0.94375) {
                        setBlockAt(x, h, z, SNOW_7);
                    } else if (snowBar < 0.946875) {
                        setBlockAt(x, h, z, SNOW_8);
                    }
                }
            } else if (b == HILLS) {
                for (int y = 0; y < h - 3 - numDirtBlocks; ++y) {
                    setBlockAt(x, y, z, STONE);
                }
                for (int currY = h - 3 - numDirtBlocks; currY < h - 1; ++currY) {
                    setBlockAt(x, currY, z, DIRT);
                }

                if (h < 120) {
                    setBlockAt(x, h - 1, z, DIRT);

                    for (int y = h; y < 120; ++y) {
                        setBlockAt(x, y, z, WATER);
                    }
                } else if (h > 130) {
                    float p3 = Biome::noise1D(glm::vec2(h, h));
                    float p4 = Biome::noise1D(glm::vec3(x, h, z));
                    if (p3 < 0.4) {
                        setBlockAt(x, h - 1, z, TILLED_DIRT);
                        if (p4 < 0.05) {
                            setBlockAt(x, h, z, WHEAT_1);
                        } else if (p4 < 0.1) {
                            setBlockAt(x, h, z, WHEAT_2);
                        } else if (p4 < 0.15) {
                            setBlockAt(x, h, z, WHEAT_3);
                        } else if (p4 < 0.2) {
                            setBlockAt(x, h, z, WHEAT_4);
                        } else if (p4 < 0.25) {
                            setBlockAt(x, h, z, WHEAT_5);
                        } else if (p4 < 0.3) {
                            setBlockAt(x, h, z, WHEAT_6);
                        } else if (p4 < 0.35) {
                            setBlockAt(x, h, z, WHEAT_7);
                        } else {
                            setBlockAt(x, h, z, WHEAT_8);
                        }
                    } else if (p3 < 0.7) {
                        setBlockAt(x, h - 1, z, IRRIGATED_SOIL);
                        if (p4 < 0.45) {
                            setBlockAt(x, h, z, RICE_1);
                        } else if (p4 < 0.5) {
                            setBlockAt(x, h, z, RICE_2);
                        } else if (p4 < 0.55) {
                            setBlockAt(x, h, z, RICE_3);
                        } else if (p4 < 0.6) {
                            setBlockAt(x, h, z, RICE_4);
                        } else if (p4 < 0.65) {
                            setBlockAt(x, h, z, RICE_5);
                        } else {
                            setBlockAt(x, h, z, RICE_6);
                        }
                    } else {
                        setBlockAt(x, h - 1, z, GRASS);
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
            float p1 = Biome::noise1D(glm::vec2(x, z));
            float p2 = Biome::fbm(glm::vec2(x, z));

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
                    if (p1 < 0.175) {
//                        createConifer1(x, h, z, CEDAR_LEAVES, CEDAR_WOOD_Y);
                    } else if (p1 < 0.225) {
//                        createConifer2(x, h, z, TEAK_LEAVES, TEAK_WOOD_Y);
                    } else if (p1 > 0.995) {
                        float p3 = Biome::noise1D(glm::vec3(x, h, z));
                        if (p3 < 0.5) {
//                            createCottage1(x, h, z);
                        } else {
//                            createCottage2(x, h, z);
                        }
                    }
                } else if (b == FOREST) {
                    if (p1 < 0.04) {
                        // bamboo
                        int y = h;
                        int addHeight = 0;
                        while (addHeight == 0) {
                            setBlockAt(x, y, z, BAMBOO_1);
                            y++;
                            if (Biome::noise1D(glm::vec3(x, y, z)) >= 0.75) {
                                addHeight = 1;
                            }
                        }
                        while (addHeight == 1) {
                            setBlockAt(x, y, z, BAMBOO_2);
                            y++;
                            if (Biome::noise1D(glm::vec3(x, y, z)) >= 0.5) {
                                addHeight = 2;
                            }
                        }
                        while (addHeight == 2) {
                            setBlockAt(x, y, z, BAMBOO_3);
                            y++;
                            if (Biome::noise1D(glm::vec3(x, y, z)) >= 0.25) {
                                addHeight = 3;
                            }
                        }
                    } else if (p1 < 0.05) {
//                        createDeciduous2(x, h, z, CHERRY_BLOSSOMS_1, CHERRY_WOOD_Y);
                    } else if (p1 < 0.055) {
//                        createDeciduous3(x, h, z, CHERRY_BLOSSOMS_2, CHERRY_WOOD_Y);
                    } else if (p1 < 0.06) {
//                        createDeciduous3(x, h, z, CHERRY_BLOSSOMS_3, CHERRY_WOOD_Y);
                    } else if (p1 < 0.065) {
//                        createDeciduous1(x, h, z, CHERRY_BLOSSOMS_4, CHERRY_WOOD_Y);
                    } else if (p1 > 0.997) {
//                        createTeaHouse(x, h, z);
                    }
                } else if (b == HILLS) {
//                    if (p1 > 0.998) {
//                        createHut(x, h, z);
//                    }
                } else if (b == ISLANDS) {
                    if (p1 < 0.005) {
//                    createConifer3(x, h, z, PINE_LEAVES, PINE_WOOD_Y);
                    } else if (p1 < 0.00525) {
//                        createDeciduous2(x, h, z, MAPLE_LEAVES_1, MAPLE_WOOD_Y);
                    } else if (p1 < 0.0055) {
//                        createDeciduous3(x, h, z, MAPLE_LEAVES_2, MAPLE_WOOD_Y);
                    } else if (p1 < 0.00575) {
//                        createDeciduous1(x, h, z, MAPLE_LEAVES_3, MAPLE_WOOD_Y);
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
                        while (y < 119 && addHeight) {
                            setBlockAt(x, y, z, KELP_1);
                            y++;
                            addHeight = (Biome::noise1D(glm::vec3(x, y, z)) < 0.75);
                        }
                        setBlockAt(x, y, z, KELP_2);
                    }
                }
            }

            bool prevNotGround = false;
            std::vector<float> treePos;
            for (int currY = 1; currY <= 106; currY++) {
                float cavePerlin3D = Biome::perlin3D(glm::vec3(x, currY, z) * 0.06f);
                float cavePerlin3DTwo = Biome::perlin3D(glm::vec3(x, currY, glm::mix(x, z, 0.35f)) * 0.06f);

                float p3 = Biome::noise1D(glm::vec3(x, currY, z));

                if (cavePerlin3D + cavePerlin3DTwo < -0.15f) {
                    if (currY < 25) {
                        setBlockAt(x, currY, z, LAVA);
                    } else {
                        setBlockAt(x, currY, z, EMPTY);
                        if (!prevNotGround) {
//                            BlockType xn = getBlockAt(x - 1, currY - 1, z);
//                            BlockType xp = getBlockAt(x + 1, currY - 1, z);
//                            BlockType zn = getBlockAt(x, currY - 1, z - 1);
//                            BlockType zp = getBlockAt(x, currY - 1, z + 1);

                            if (p3 < 0.6) {
                                setBlockAt(x, currY - 1, z, GRASS);
                            } else if (p3 < 0.8 /*&&
                                (Chunk::isFullCube(xn) || xn == WATER) &&
                                (Chunk::isFullCube(xp) || xp == WATER) &&
                                (Chunk::isFullCube(zn) || zn == WATER) &&
                                (Chunk::isFullCube(zp) || zp == WATER)*/) {
                                    setBlockAt(x, currY - 1, z, WATER);
                            }

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
                float p4 = Biome::noise1D(glm::vec2(x, z));
                if (p4 < 0.1025) {
//                    createDeciduous3(x, y, z, WISTERIA_BLOSSOMS_1, WISTERIA_WOOD_Y);
                } else if (p4 < 0.105) {
//                    createDeciduous2(x, y, z, WISTERIA_BLOSSOMS_2, WISTERIA_WOOD_Y);
                } else {
//                    createDeciduous1(x, y, z, WISTERIA_BLOSSOMS_3, WISTERIA_WOOD_Y);
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

    for (const auto& c: m_chunks) {
        c.second->createVBOdata();
    }
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

    float LimE = 0.39;
    float LimT = 0.69;


    if (elev >= LimE && temp < LimT) {
        b = MOUNTAINS;
    } else if (elev >= LimE && temp >= LimT) {
        b = HILLS;
    } else if (elev < LimE && temp < LimT) {
        b = FOREST;
    } else if (elev < LimE && temp >= LimT) {
        b = ISLANDS;
    }
    // set biome weights in m_biomes for each xz coord in this chunk
    biomeWts.x = elev * (1.f - temp);
    biomeWts.y = elev * temp;
    biomeWts.z = (1.f - elev) * (1.f - temp);
    biomeWts.w = (1.f - elev) * temp;

    setBiomeAt(xz.x, xz.y, biomeWts);

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

void Terrain::createCottage1(int x, int y, int z) {
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

void Terrain::createCottage2(int x, int y, int z) {
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

void Terrain::createTeaHouse(int x, int y, int z) {
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
    for (int x3 = x - 1; x3 <= z + 15; x3++) {
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

void Terrain::createConifer1(int x, int y, int z, BlockType leaf, BlockType wood) {
    // leaves
    for (int x1 = x - 3; x1 <= x + 3; x1++) {
        for (int z1 = z - 3; z1 <= z + 3; z1++) {
            setBlockAt(x1, y + 1, z1, leaf);
        }
    }
    setBlockAt(x - 3, y + 1, z - 3, EMPTY);
    setBlockAt(x - 3, y + 1, z + 3, EMPTY);
    setBlockAt(x + 3, y + 1, z - 3, EMPTY);
    setBlockAt(x + 3, y + 1, z + 3, EMPTY);

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

    // trunk
    for (int y1 = y; y1 <= y + 6; y1++) {
        setBlockAt(x, y1, z, wood);
    }
}
void Terrain::createConifer2(int x, int y, int z, BlockType leaf, BlockType wood) {
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

    // trunk
    for (int y1 = y; y1 <= y + 8; y1++) {
        setBlockAt(x, y1, z, wood);
    }
}
void Terrain::createConifer3(int x, int y, int z, BlockType leaf, BlockType wood) {

    for (int y3 = y + 1; y3 <= y + 5; y3++) {
        for (int x3 = x - 1; x3 <= x + 1; x3++) {
            for (int z3 = z - 1; z3 <= z + 1; z3++) {
                setBlockAt(x3, y3, z3, leaf);
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

    setBlockAt(x, y + 6, z, leaf);

    // trunk
    for (int y1 = y; y1 <= y + 5; y1++) {
        setBlockAt(x, y1, z, wood);
    }
}
void Terrain::createDeciduous1(int x, int y, int z, BlockType leaf, BlockType wood) {
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
void Terrain::createDeciduous2(int x, int y, int z, BlockType leaf, BlockType wood) {
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
void Terrain::createDeciduous3(int x, int y, int z, BlockType leaf, BlockType wood) {
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
