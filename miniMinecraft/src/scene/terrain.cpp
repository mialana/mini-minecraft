#include "terrain.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

Terrain::Terrain(OpenGLContext* context)
    : m_chunks()
    , m_generatedTerrain()
    , mp_context(context)
{}

Terrain::~Terrain() {}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z)
{
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

glm::ivec2 toCoords(int64_t k)
{
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;

    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if (z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }

    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if (hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if (y < 0 || y >= 256) {
            return EMPTY;
        }

        const uPtr<Chunk>& c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    } else {
        throw std::out_of_range("Coordinates " + std::to_string(x) + " " + std::to_string(y) + " "
                                + std::to_string(z) + " have no Chunk!");
    }
}

glm::vec4 Terrain::getBiomeAt(glm::vec2 p) const
{
    return getBiomeAt(p.x, p.y);
}

glm::vec4 Terrain::getBiomeAt(int x, int z) const
{
    if (hasChunkAt(x, z)) {
        const uPtr<Chunk>& c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBiomeAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    } else {
        throw std::out_of_range("Coordinates " + std::to_string(x) + std::to_string(z)
                                + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const
{
    return getBlockAt(p.x, p.y, p.z);
}

bool Terrain::hasChunkAt(int x, int z) const
{
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

uPtr<Chunk>& Terrain::getChunkAt(int x, int z)
{
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}

const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const
{
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::multithreadedWork(glm::vec3 currPlayerPos, glm::vec3 prevPlayerPos, float dt)
{
    m_chunkTimer += dt;
    if (m_chunkTimer >= 0.5f) {
        tryExpansion(currPlayerPos, prevPlayerPos);
        m_chunkTimer = 0.0f;
    }
    checkThreadResults();
}

QSet<long long> Terrain::borderingZone(glm::ivec2 coords, int radius, bool atEdge)
{
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

void Terrain::tryExpansion(glm::vec3 pos, glm::vec3 prevPos)
{
    // Find the 64 x 64 zone the player is on
    glm::ivec2 curr(64.f * floor(pos.x / 64.f), 64.f * floor(pos.z / 64.f));
    glm::ivec2 prev(64.f * floor(prevPos.x / 64.f), 64.f * floor(prevPos.z / 64.f));
    // Figure out which zones border this zone and the previous zone
    QSet<long long> borderingCurr = borderingZone(curr, 3, false);
    QSet<long long> borderingPrev = borderingZone(prev, 3, false);

    // Figure out if the current zones need VBO data or Block data
    for (long long zone : borderingCurr) {
        if (m_chunks.find(zone) != m_chunks.end()) {
            if (!borderingPrev.contains(zone)) {
                glm::ivec2 coord = toCoords(zone);

                for (int x = coord.x; x < coord.x + 64; x += 16) {
                    for (int z = coord.y; z < coord.y + 64; z += 16) {
                        auto& c = getChunkAt(x, z);
                        createVBOWorker(c.get());
                    }
                }
            }
        } else {
            createBDWorker(zone);
        }
    }
}

void Terrain::checkThreadResults()
{
    // First, send chunks processed by BlockWorkers to VBOWorkers
    if (!m_blockDataChunks.empty()) {
        m_blockDataChunksLock.lock();

        for (auto& c : m_blockDataChunks) {
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

    for (auto& data : m_vboDataChunks) {
        data->loadVBO();
    }

    m_vboDataChunks.clear();
    m_VBODataChunksLock.unlock();
}

void Terrain::createBDWorker(long long zone)
{
    std::vector<Chunk*> toDo;
    int x = toCoords(zone).x;
    int z = toCoords(zone).y;

    for (int i = x; i < x + 64; i += 16) {
        for (int j = z; j < z + 64; j += 16) {
            toDo.push_back(instantiateChunkAt(i, j));
        }
    }

    BDWorker* worker = new BDWorker(x, z, toDo, &m_blockDataChunks, &m_blockDataChunksLock);
    QThreadPool::globalInstance()->start(worker);
}

void Terrain::createVBOWorkers(const std::unordered_set<Chunk*>& chunks)
{
    for (Chunk* chunk : chunks) {
        createVBOWorker(chunk);
    }
}

void Terrain::createVBOWorker(Chunk* chunk)
{
    VBOWorker* werk = new VBOWorker(chunk, &m_vboDataChunks, &m_VBODataChunksLock);
    QThreadPool::globalInstance()->start(werk);
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    if (hasChunkAt(x, z)) {
        uPtr<Chunk>& c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
    } else {
        throw std::out_of_range("Coordinates " + std::to_string(x) + " " + std::to_string(y) + " "
                                + std::to_string(z) + " have no Chunk!");
    }
}

void Terrain::changeBlockAt(int x, int y, int z, BlockType bt)
{
    BlockType currBt = this->getBlockAt(glm::vec3(x, y, z));
    if (currBt != bt) {
        this->setBlockAt(x, y, z, bt);
        Chunk* c = this->getChunkAt(x, z).get();
        c->destroyVBOdata();
        c->generateVBOData();
        c->loadVBO();
    }
}

void Terrain::setBiomeAt(int x, int z, glm::vec4 b)
{
    if (hasChunkAt(x, z)) {
        uPtr<Chunk>& c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBiomeAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      b);
    } else {
        throw std::out_of_range("Coordinates " + std::to_string(x) + std::to_string(z)
                                + " have no Chunk!");
    }
}

Chunk* Terrain::instantiateChunkAt(int xcoord, int zcoord)
{
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context);
    chunk->setWorldPos(xcoord, zcoord);
    m_chunks[toKey(xcoord, zcoord)] = std::move(chunk);
    Chunk* cPtr = m_chunks[toKey(xcoord, zcoord)].get();

    // Set the neighbor pointers of itself and its neighbors
    if (hasChunkAt(xcoord, zcoord + 16)) {
        auto& chunkNorth = m_chunks[toKey(xcoord, zcoord + 16)];
        cPtr->linkNeighbor(chunkNorth.get(), ZPOS);
    }

    if (hasChunkAt(xcoord, zcoord - 16)) {
        auto& chunkSouth = m_chunks[toKey(xcoord, zcoord - 16)];
        cPtr->linkNeighbor(chunkSouth.get(), ZNEG);
    }

    if (hasChunkAt(xcoord + 16, zcoord)) {
        auto& chunkEast = m_chunks[toKey(xcoord + 16, zcoord)];
        cPtr->linkNeighbor(chunkEast.get(), XPOS);
    }

    if (hasChunkAt(xcoord - 16, zcoord)) {
        auto& chunkWest = m_chunks[toKey(xcoord - 16, zcoord)];
        cPtr->linkNeighbor(chunkWest.get(), XNEG);
    }

    return cPtr;
}

void Terrain::draw(int minX,
                   int maxX,
                   int minZ,
                   int maxZ,
                   ShaderProgram* shaderProgram,
                   std::vector<uPtr<Mob>>& currMobs)
{
    m_blockDataChunksLock.lock();
    m_VBODataChunksLock.lock();
    for (int x = minX; x < maxX; x += 16) {
        for (int z = minZ; z < maxZ; z += 16) {
            if (hasChunkAt(x, z)) {
                const uPtr<Chunk>& currChunk = getChunkAt(x, z);

                if (currChunk->hasVBOData && currChunk->hasBinded) {
                    shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
                    shaderProgram->drawInterleavedO(*currChunk);
                }
            }
        }
    }
    for (int x = minX; x < maxX; x += 16) {
        for (int z = minZ; z < maxZ; z += 16) {
            if (hasChunkAt(x, z)) {
                const uPtr<Chunk>& currChunk = getChunkAt(x, z);

                if (currChunk->hasVBOData && currChunk->hasBinded) {
                    shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
                    shaderProgram->drawInterleavedT(*currChunk);
                }
            }
        }
    }

    // handle mob respawning

    std::vector<Mob*> mobsToRespawn;

    for (auto& mob : currMobs) {
        if (mob->needsRespawn) {
            mobsToRespawn.push_back(mob.get());
        }
    }

    if (mobsToRespawn.size() > 0) {
        std::vector<Chunk*> availableChunks;

        for (int x = minX; x < maxX; x += 16) {
            for (int z = minZ; z < maxZ; z += 16) {
                if (hasChunkAt(x, z)) {
                    const uPtr<Chunk>& currChunk = getChunkAt(x, z);

                    if (currChunk->hasVBOData && currChunk->hasBinded
                        && currChunk->viableSpawnBlocks.size() > 0) {
                        availableChunks.push_back(currChunk.get());
                    }
                }
            }
        }
        if (availableChunks.size() > 0) {
            for (Mob* mob : mobsToRespawn) {
                int randomChunk = Biome::getRandomIntInRange(0, availableChunks.size() - 1);

                mob->respawn(availableChunks[randomChunk]);
            }
        }
    }

    m_blockDataChunksLock.unlock();
    m_VBODataChunksLock.unlock();
}

void Terrain::CreateTestScene()
{
    // Create the Chunks that will
    // store the blocks for our
    // initial world space
    for (int x = 0; x < 64; x += 16) {
        for (int z = 0; z < 64; z += 16) {
            Chunk* c = instantiateChunkAt(x, z);
            c->helperCreate(x, z);
        }
    }

    for (int x = 0; x < 64; x += 16) {
        for (int z = 0; z < 64; z += 16) {
            Chunk* c = getChunkAt(x, z).get();
            c->createVBOdata();
        }
    }
    // Tell our existing terrain set that
    // the "generated terrain zone" at (0,0)
    // now exists.
    m_generatedTerrain.insert(toKey(0, 0));
}

void Terrain::loadNewChunks(glm::vec3 currPos)
{
    for (const glm::ivec2& p : directionHelper) {
        if (!hasChunkAt(currPos.x + p[0], currPos.z + p[1])) {
            glm::ivec2 currChunkOrigin = 16
                                         * glm::ivec2(
                                             glm::floor(glm::vec2(currPos.x, currPos.z) / 16.f));

            glm::ivec2 newChunkOrigin = currChunkOrigin + p;

            Chunk* newChunk = instantiateChunkAt(newChunkOrigin[0], newChunkOrigin[1]);
            newChunk->createVBOdata();
        }
    }
}

std::pair<float, BiomeEnum> Terrain::blendMultipleBiomes(glm::vec2 xz,
                                                         float forestH,
                                                         float mountH,
                                                         float hillH,
                                                         float islandH)
{
    // perform bilinear interpolation

    BiomeEnum b;
    glm::vec4 biomeWts;

    double elev = (Biome::perlin1(xz / 297.f) + 1.f)
                  / 2.f;  // remap perlin noise from (-1, 1) to (0, 1)
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

    float h = (biomeWts.x * mountH) + (biomeWts.y * hillH) + (biomeWts.z * forestH)
              + (biomeWts.w * islandH);
    return std::pair(h, b);
}
