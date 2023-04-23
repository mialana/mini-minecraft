#include "terrain.h"
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <QThreadPool>
#include "workers.h"

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

bool Terrain::hasNewChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));

    return newChunks.find(toKey(16 * xFloor, 16 * zFloor)) != newChunks.end();
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

uPtr<Chunk>& Terrain::getNewChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));

    return newChunks[toKey(16 * xFloor, 16 * zFloor)];
}

const uPtr<Chunk>& Terrain::getNewChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));

    return newChunks.at(toKey(16 * xFloor, 16 * zFloor));
}

bool Terrain::hasTerrainGenerationZoneAt(glm::ivec2 zone) {
    return m_generatedTerrain.find(toKey(zone.x, zone.y)) != m_generatedTerrain.end();
}

void Terrain::multithreadedWork(glm::vec3 currPlayerPos, glm::vec3 prevPlayerPos, float dt) {
    m_chunkTimer += dt;
    if (m_chunkTimer >= 0.5f) {
        tryNewChunk(currPlayerPos, prevPlayerPos);
        m_chunkTimer = 0.0f;
    }
    checkthreadResults();
}

std::vector<glm::ivec2> getNearTerrainGenZones(glm::vec2 pos, int n) {
    std::vector<glm::ivec2> tgzs = {};
    int halfWidth = glm::floor(n / 2.f) * 64;

    for (int i = pos.x - halfWidth; i <= pos.x + halfWidth; i += 64) {
        for (int j = pos.y - halfWidth; j <= pos.y + halfWidth; j+= 64) {
            tgzs.push_back(glm::ivec2(i, j));
        }
    }

    return tgzs;
}

std::vector<glm::ivec2> isSameVector(std::vector<glm::ivec2> a, std::vector<glm::ivec2> b) {
    std::vector<glm::ivec2> diff;

    for (glm::ivec2 vecB : b) {
        bool match = false;
        for (glm::ivec2 vecA : a) {
            if (vecA.x == vecB.x && vecA.y == vecB.y) {
                match = true;
                break;
            }
        }

        if (!match) {
            diff.push_back(vecB);
        }
    }

    return diff;
}

void Terrain::tryNewChunk(glm::vec3 pos, glm::vec3 prevPos) {
    // Find the 64 x 64 zone the player is on
    glm::ivec2 curr(64.f * floor(pos.x/64.f), 64.f * floor(pos.z/64.f));
    glm::ivec2 prev(64.f * floor(prevPos.x/64.f), 64.f * floor(prevPos.z/64.f));
    // Figure out which zones border this zone and the previous zone
    QSet<long long> borderingCurr = borderingZone(curr, 3, false);
    QSet<long long> borderingPrev = borderingZone(prev, 3, false);
    // If previous zones are no longer there, remove their vbo data
    for(long long zone : borderingPrev) {
        if (!borderingCurr.contains(zone)) {
            std::cout<<"hello world";
            glm::ivec2 coord = toCoords(zone);
            for (int x = coord.x; x < coord.x + 64; x += 16) {
                for(int z = coord.y; z < coord.y + 64; z += 16) {
                    auto& c = getChunkAt(x, z);
                    c->destroyVBOdata();
                    c->hasVBOData = false;
                    c->hasBinded = false;
                    //c->isBuffered = false;
                }
            }
        }
    }
    // Figure out if the current zones need VBO data or Block data
    for (long long zone: borderingCurr) {
        if (m_chunks.find(zone) != m_chunks.end()) {
            if (!borderingPrev.contains(zone)) {
                glm::ivec2 coord = toCoords(zone);
                for (int x = coord.x; x < coord.x + 64; x += 16) {
                    for(int z = coord.y; z < coord.y + 64; z += 16) {
                        auto& c = getChunkAt(x, z);
                        createVBOWorker(c.get());
                    }
                }
            }
        } else {
            //std::cout<<"I am bd worker \n";
            createBDWorker(zone);
        }
    }
}

QSet<long long> Terrain::borderingZone(glm::ivec2 coords, int radius, bool atEdge) {
    int radiusScale = radius * 64;
    QSet<long long> result;
    // Adds Zones only at the radius
    if (atEdge) {
        for (int i = -radiusScale; i <= radiusScale; i += 64) {
            result.insert(toKey(coords.x + radiusScale, coords.y + i));
            result.insert(toKey(coords.x - radiusScale, coords.y + i));
            result.insert(toKey(coords.x + i, coords.y + radiusScale));
            result.insert(toKey(coords.x + i, coords.y + radiusScale));
        }
    // Adds Zones upto and including the radius
    } else {
        for (int i = -radiusScale; i <= radiusScale; i += 64) {
            for (int j = -radiusScale; j <= radiusScale; j += 64) {
                result.insert(toKey(coords.x + i, coords.y + j));
            }
        }
    }
    return result;
}

void Terrain::tryExpansion(glm::vec3 currPlayerPos, glm::vec3 prevPlayerPos) {

    //std::cout<<"itry  \n";

    glm::ivec2 currZone = glm::ivec2(glm::floor(currPlayerPos.x / 64.f) * 64.f,
                                     glm::floor(currPlayerPos.z / 64.f) * 64.f);

    glm::ivec2 prevZone = glm::ivec2(glm::floor(prevPlayerPos.x / 64.f) * 64.f,
                                     glm::floor(prevPlayerPos.z / 64.f) * 64.f);

    std::vector<glm::ivec2> currTGZs = getNearTerrainGenZones(currZone, 1);
    std::vector<glm::ivec2> prevTGZs = getNearTerrainGenZones(prevZone, 1);

    std::vector<glm::ivec2> newZones = firstTick ? currTGZs : isSameVector(currTGZs, prevTGZs);
    std::vector<glm::ivec2> oldZones = isSameVector(currTGZs, prevTGZs);

    for (glm::ivec2 newZone : newZones) {
        if (!hasTerrainGenerationZoneAt(newZone)) {
            m_generatedTerrain.insert(toKey(newZone.x, newZone.y));

            for (int x = 0; x < 64; x += 16) {
                for (int z = 0; z < 64; z += 16) {
                    std::cout<<"i am add new chunk \n";
                    newChunks[toKey(newZone.x + x, newZone.y + z)] = instantiateNewChunkAt(newZone.x + x, newZone.y + z);
                }
            }
        }
    }

    for (auto & [ key, chunk ]: newChunks) {
        int x = chunk->getWorldPos().x;
        int z = chunk->getWorldPos().y;

        if (hasChunkAt(x, z + 16)) {
            auto &chunkNorth = getChunkAt(x, z + 16);
            chunk->linkNeighbor(chunkNorth, ZPOS);
        }
        if (hasChunkAt(x, z - 16)) {
            auto &chunkSouth = getChunkAt(x, z - 16);
            chunk->linkNeighbor(chunkSouth, ZNEG);
        }
        if (hasChunkAt(x + 16, z)) {
            auto &chunkEast = getChunkAt(x + 16, z);
            chunk->linkNeighbor(chunkEast, XPOS);
        }
        if (hasChunkAt(x - 16, z)) {
            auto &chunkWest = getChunkAt(x - 16, z);
            chunk->linkNeighbor(chunkWest, XNEG);
        }
    }

    for (auto & [ key, chunk ]: newChunks) {
        blockWorkerThreads.push_back(std::thread(&Terrain::blockWorker, this, move(chunk)));
    }

    newChunks.clear();
    firstTick = false;
}

uPtr<Chunk> Terrain::instantiateNewChunkAt(int x, int z) {
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context);
    chunk.get()->setWorldPos(x, z);

    return chunk;
}

BlockType Terrain::generateBlockTypeByHeight(int height, bool isTop) {

    if (height < 120) {
        return STONE;
    }

    if (height < 140) {
        return STONE;
    }

    if (height < 160) {
        return isTop ? GRASS : STONE;
    }

    if (height > 200) {
        return isTop ? GRASS : STONE;
    }

    return STONE;
}

void Terrain::blockWorker(uPtr<Chunk> chunk) {
    chunksWithBlockDataMutex.lock();
    glm::ivec2 chunkWorldPos = chunk->getWorldPos();

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            int height = std::rand() % 250;
            for (int k = 0; k <= height; k++) {
                chunk->setBlockAt(x, k, z, generateBlockTypeByHeight(height, k == height));
            }
        }
    }
    chunksWithBlockData[toKey(chunk->getWorldPos().x, chunk->getWorldPos().y)] = move(chunk);
    chunksWithBlockDataMutex.unlock();
}

void Terrain::checkThreadResults() {
   /* //std::cout<<"i am cjecl tjread load \n";
    chunksWithBlockDataMutex.lock();
    for (auto & [ key, chunk ] : chunksWithBlockData) {
        std::cout<<"i am here at vbo worker \n";
        vboWorkerThreads.push_back(std::thread(&Terrain::VBOWorker, this, std::move(chunk)));
    }
    chunksWithBlockData.clear();
    chunksWithBlockDataMutex.unlock();

    chunksWithVBODataMutex.lock();
    for (auto & [ key, chunk ] : chunksWithVBOData) {
        chunk->loadVBO();
        std::cout<<"i am here at load \n";
        m_chunks[key] = move(chunk);
    }
    chunksWithVBOData.clear();
    chunksWithVBODataMutex.unlock();*/
}

/*void Terrain::VBOWorker(uPtr<Chunk> chunk) {
    chunksWithVBODataMutex.lock();
    chunk->generateVBOData();
    chunksWithVBOData[toKey(chunk->getWorldPos().x, chunk->getWorldPos().y)] = move(chunk);
    chunksWithVBODataMutex.unlock();
}*/

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

Chunk* Terrain::instantiateChunkAt(int xcoord, int zcoord) {
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context);
    chunk->setWorldPos(xcoord, zcoord);
    m_chunks[toKey(xcoord, zcoord)] = std::move(chunk);
    Chunk *cPtr = m_chunks[toKey(xcoord, zcoord)].get();
    // Set the neighbor pointers of itself and its neighbors
    if(hasChunkAt(xcoord, zcoord + 16)) {
        auto &chunkNorth = m_chunks[toKey(xcoord, zcoord + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(xcoord, zcoord - 16)) {
        auto &chunkSouth = m_chunks[toKey(xcoord, zcoord - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(xcoord + 16, zcoord)) {
        auto &chunkEast = m_chunks[toKey(xcoord + 16, zcoord)];
        cPtr->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(xcoord - 16, zcoord)) {
        auto &chunkWest = m_chunks[toKey(xcoord - 16, zcoord)];
        cPtr->linkNeighbor(chunkWest, XNEG);
    }
    return cPtr;
}

void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    chunksWithBlockDataMutex.lock();
    chunksWithVBODataMutex.lock();
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {


            /*const uPtr<Chunk>& currChunk = getChunkAt(x, z);
            currChunk->generateVBOData();
            currChunk->loadVBO();
            shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
            shaderProgram->drawInterleavedO(*currChunk);*/


            if (hasChunkAt(x, z)) {
                //std::cout << x << ", " << z << std::endl;
                const uPtr<Chunk> &currChunk = getChunkAt(x, z);
                if (currChunk->hasVBOData && currChunk->hasBinded) {
                    shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
                    shaderProgram->drawInterleavedO(*currChunk);
                }
                //currChunk->loadVBO();
                //std::cout << currChunk->m_oCount << std::endl;

            }
        }
    }

    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {


            /*const uPtr<Chunk>& currChunk = getChunkAt(x, z);
            currChunk->generateVBOData();
            currChunk->loadVBO();
            shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
            shaderProgram->drawInterleavedO(*currChunk);*/


            if(hasChunkAt(x, z)) {
                const uPtr<Chunk> &currChunk = getChunkAt(x, z);
                //currChunk->loadVBO();
                if (currChunk->hasVBOData && currChunk->hasBinded) {
                    shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
                    shaderProgram->drawInterleavedO(*currChunk);
                }
            }
        }
    }
    chunksWithBlockDataMutex.unlock();
    chunksWithVBODataMutex.unlock();
}


void Terrain::CreateTestScene()
{
    // Create the Chunks that will
    // store the blocks for our
    // initial world space


    for(int x = 0; x < 64; x += 16) {
        for(int z = 0; z < 64; z += 16) {
            Chunk* c = instantiateChunkAt(x, z);
            c->helperCreate(0.0f, 0.0f);
            c->createVBOdata();
        }
    }

    // Tell our existing terrain set that
    // the "generated terrain zone" at (0,0)
    // now exists.
    m_generatedTerrain.insert(toKey(0, 0));

/*
    for (int x = 0; x < 64; ++x) {
        for (int z = 0; z < 64; ++z) {
            int h = Biome::mountains(glm::vec2(x, z));
            int numDirtBlocks = 10 * Biome::fbm(glm::vec2(x, z));

            if (h <= 100) {
                for (int y = 0; y < h - numDirtBlocks; ++y) {
                    setBlockAt(x, y, z, STONE);
                }
                for (int y = h - numDirtBlocks; y < h; ++y) {
                    setBlockAt(x, y, z, DIRT);
                }
                for (int y = h; y < 100; ++y) {
                    setBlockAt(x, y, z, WATER);
                }
            } else {
                for (int y = 0; y < h - numDirtBlocks - 1; ++y) {
                    setBlockAt(x, y, z, STONE);
                }
                for (int y = h - numDirtBlocks - 1; y < h - 2; ++y) {
                    setBlockAt(x, y, z, DIRT);
                }
                setBlockAt(x, h - 2, z, GRASS);
                setBlockAt(x, h - 1, z, SNOW_1);
            }

            std::cout<< h << std::endl;
        }
    }*/
/*
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
                    setBlockAt(x, h, z, SNOW_1);
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
                } else {
                    setBlockAt(x, h - 1, z, GRASS);
                }
            } else if (b == FOREST) {
                for (int y = 0; y < h - numDirtBlocks - 1; ++y) {
                    setBlockAt(x, y, z, STONE);
                }
                for (int y = h - numDirtBlocks - 1; y < h - 1; ++y) {
                    setBlockAt(x, y, z, DIRT);
                }

                if (h < 125) {
                    setBlockAt(x, h - 1, z, DIRT);

                    for (int y = h; y < 125; ++y) {
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
                if (h < 115) {
                    for (int y = h; y < 115; ++y) {
                        setBlockAt(x, y, z, WATER);
                    }
                }
            }

            // assets
            float plant = Biome::noise1D(glm::vec2(x, z));
            if (getBlockAt(x, h, z) == EMPTY) {

                // TALL_GRASS
                if ((b == MOUNTAINS && plant < 0.35) ||
                    (b == HILLS && plant < 0.75) ||
                    (b == FOREST && plant < 0.25) ||
                    (b == ISLANDS && plant < 0.1)) {

                    setBlockAt(x, h, z, TALL_GRASS);
                }

                // bamboo, trees
            } else if (getBlockAt(x, h, z) == WATER) {
                // lotuses, coral, sea grass, kelp, lanterns
            }

            for (int currY = 1; currY <= 106; currY++) {
                float cavePerlin3D = Biome::perlin3D(glm::vec3(x, currY, z) * 0.06f);
                float cavePerlin3DTwo = Biome::perlin3D(glm::vec3(x, currY, glm::mix(x, z, 0.35f)) * 0.06f);

                if (cavePerlin3D + cavePerlin3DTwo < -0.15f) {
                    if (currY < 25) {
                        setBlockAt(x, currY, z, LAVA);
                    } else {
                        setBlockAt(x, currY, z, EMPTY);
                    }

                }
            }
            setBlockAt(x, 0, z, BEDROCK);
        }
    }*/
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

    double elev = (Biome::perlin1(xz) + 1.f) / 2.f; // remap perlin noise from (-1, 1) to (0, 1)
    double temp = (Biome::perlin2(xz) + 1.f) / 2.f;

//    BiomeEnum bFM;
//    BiomeEnum bHI;
//    float heightMix1 = glm::smoothstep(0.25, 0.75, p1);
//    float hFM = ((1 - heightMix1) * forestH) + (heightMix1 * mountH);
//    float hHI = ((1 - heightMix1) * hillH) + (heightMix1 * islandH);

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
    setBiomeAt(xz.x, xz.y, biomeWts);

//    std::cout<<"("<<biomeWts.x<<","<<biomeWts.y<<","<<biomeWts.z<<","<<biomeWts.w<<")"<<std::endl;
    if (biomeWts.x + biomeWts.y + biomeWts.z + biomeWts.w != 1) {
        std::cout<< "something is wrong" <<std::endl;
    }

    float h = (biomeWts.x * mountH) +
                (biomeWts.y * hillH) +
                (biomeWts.z * forestH) +
                (biomeWts.w * islandH);
    return std::pair(h, b);
}

void Terrain::checkthreadResults() {
    // First, send chunks processed by BlockWorkers to VBOWorkers
    if (!m_blockDataChunks.empty()) {
        m_blockDataChunksLock.lock();
        for (auto &c : m_blockDataChunks) {
            for (auto& n : c->m_neighbors) {
                if (n.second) {
                m_blockDataChunks.insert(n.second);
                }
            }
        }
        createVBOWorkers(m_blockDataChunks);
        m_blockDataChunks.clear();
        m_blockDataChunksLock.unlock();
    }
    // Second, take the chunks that have VBO data and send data to GPU
    m_VBODataChunksLock.lock();
    for (auto& data: m_vboDataChunks) {
        data->loadVBO();
    }
    m_vboDataChunks.clear();
    m_VBODataChunksLock.unlock();
}

void Terrain::createBDWorkers(const QSet<long long> &zones) {
    for (long long zone: zones) {
        createBDWorker(zone);
    }
}

void Terrain::createBDWorker(long long zone) {
    std::vector<Chunk*> toDo;
    int x = toCoords(zone).x;
    int z = toCoords(zone).y;
    for (int i = x; i < x + 64; i += 16) {
        for (int j = z; j < z + 64; j += 16) {
            toDo.push_back(instantiateChunkAt(i, j));
        }
    }
    BDWorker *worker = new BDWorker(x, z, toDo,
                                    &m_blockDataChunks, &m_blockDataChunksLock);
    QThreadPool::globalInstance()->start(worker);
}

void Terrain::createVBOWorkers(const std::unordered_set<Chunk*> &chunks) {
    for (Chunk* chunk: chunks) {
        createVBOWorker(chunk);
    }
}

void Terrain::createVBOWorker(Chunk* chunk) {
    VBOWorker *werk = new VBOWorker(chunk, &m_vboDataChunks, &m_VBODataChunksLock);
    QThreadPool::globalInstance()->start(werk);
}
