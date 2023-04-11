#include "terrain.h"
#include <stdexcept>
#include <iostream>
#include <cstdlib>

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

void Terrain::multithreadedWork(glm::vec3 currPlayerPos, glm::vec3 prevPlayerPos) {
    tryExpansion(currPlayerPos, prevPlayerPos);
    checkThreadResults();
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

void Terrain::tryExpansion(glm::vec3 currPlayerPos, glm::vec3 prevPlayerPos) {
    glm::ivec2 currZone = glm::ivec2(glm::floor(currPlayerPos.x / 64.f) * 64.f,
                                     glm::floor(currPlayerPos.z / 64.f) * 64.f);
    glm::ivec2 prevZone = glm::ivec2(glm::floor(prevPlayerPos.x / 64.f) * 64.f,
                                     glm::floor(prevPlayerPos.z / 64.f) * 64.f);

    std::vector<glm::ivec2> currTGZs = getNearTerrainGenZones(currZone, 5);
    std::vector<glm::ivec2> prevTGZs = getNearTerrainGenZones(prevZone, 5);

    std::vector<glm::ivec2> newZones = firstTick ? currTGZs : isSameVector(prevTGZs, currTGZs);
    std::vector<glm::ivec2> oldZones = isSameVector(currTGZs, prevTGZs);

    for (glm::ivec2 newZone : newZones) {
        if (!hasTerrainGenerationZoneAt(newZone)) {
            m_generatedTerrain.insert(toKey(newZone.x, newZone.y));

            for (int x = 0; x < 64; x += 16) {
                for (int z = 0; z < 64; z += 16) {
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
    glm::ivec2 chunkWorldPos = chunk->getWorldPos();

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            //int height = ProcGen::getHeight(chunkWorldPos.x + x, chunkWorldPos.y + z);
            int height = std::rand() % 250;
            for (int k = 0; k <= height; k++) {
                chunk->setBlockAt(x, k, z, generateBlockTypeByHeight(height, k == height));
            }
        }
    }

    chunksWithBlockDataMutex.lock();
    chunksWithBlockData[toKey(chunk->getWorldPos().x, chunk->getWorldPos().y)] = move(chunk);
    chunksWithBlockDataMutex.unlock();
}

void Terrain::checkThreadResults() {
    chunksWithBlockDataMutex.lock();
    for (auto & [ key, chunk ] : chunksWithBlockData) {
        vboWorkerThreads.push_back(std::thread(&Terrain::VBOWorker, this, move(chunk)));
    }
    chunksWithBlockData.clear();
    chunksWithBlockDataMutex.unlock();

    chunksWithVBODataMutex.lock();
    for (auto & [ key, chunk ] : chunksWithVBOData) {
        chunk->create();
        m_chunks[key] = move(chunk);
    }
    chunksWithVBOData.clear();
    chunksWithVBODataMutex.unlock();
}

void Terrain::VBOWorker(uPtr<Chunk> chunk) {
    chunk->createVBOdata();

    chunksWithVBODataMutex.lock();
    chunksWithVBOData[toKey(chunk->getWorldPos().x, chunk->getWorldPos().y)] = move(chunk);
    chunksWithVBODataMutex.unlock();
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
    chunksWithBlockDataMutex.lock();
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            const uPtr<Chunk>& currChunk = getChunkAt(x, z);
            currChunk->createVBOdata();

            shaderProgram->setModelMatrix(glm::translate(glm::mat4(1), glm::vec3(x, 0, z)));
            shaderProgram->drawInterleaved(*currChunk);
        }
    }
    chunksWithBlockDataMutex.unlock();
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

            float h_h = hills(glm::vec2(x, z));
            float h_m = mountains(glm::vec2(x, z));
            float h_f = forest(glm::vec2(x, z));
            float h_i = islands(glm::vec2(x, z));

            // TODO: add forest and beach biomes

            float h = blendTerrain(glm::vec2(x, z), h_h, h_m);
            float biomeType = blendTerrain(glm::vec2(x, z), 0.f, 1.f);
            int numDirtBlocks = 10 * fbm(glm::vec2(x, z));
            if (biomeType >= 0.5) {
                // hills
                for (int y = 0; y < h - 3 - numDirtBlocks; ++y) {
                    setBlockAt(x, y, z, STONE);
                }
                for (int y = h - 3 - numDirtBlocks; y < h - 1; ++y) {
                    setBlockAt(x, y, z, DIRT);
                }
                setBlockAt(x, h - 1, z, GRASS);
            } else {
                // mountains
                if (h <= 100) {
                    for (int y = 0; y < h - 1; ++y) {
                        setBlockAt(x, y, z, STONE);
                    }
                } else {
                    for (int y = 0; y < h - numDirtBlocks - 1; ++y) {
                        setBlockAt(x, y, z, STONE);
                    }
                    for (int y = h - numDirtBlocks - 1; y < h - 2; ++y) {
                        setBlockAt(x, y, z, DIRT);
                    }
                    setBlockAt(x, h - 2, z, GRASS);
                    setBlockAt(x, h - 1, z, SNOW);
                }
            }
        }
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


// Terrain Generation Functions
glm::vec2 Terrain::noise2D(glm::vec2 p) {
    float x = glm::fract(43758.5453 * glm::sin(glm::dot(p, glm::vec2(127.1, 311.7))));
    float y = glm::fract(43758.5453 * glm::sin(glm::dot(p, glm::vec2(269.5, 183.3))));
    return glm::vec2(x, y);
}

float Terrain::noise1D(glm::vec2 p) {
    return glm::fract(sin(glm::dot(p, glm::vec2(127.1, 311.7))) * 43758.5453);
}

float Terrain::interpNoise(float x, float y) {
    int intX = int(floor(x));
    float fractX = glm::fract(x);
    int intY = int(floor(y));
    float fractY = glm::fract(y);

    float v1 = noise1D(glm::vec2(intX, intY));
    float v2 = noise1D(glm::vec2(intX + 1, intY));
    float v3 = noise1D(glm::vec2(intX, intY + 1));
    float v4 = noise1D(glm::vec2(intX + 1, intY + 1));

    // mix is a glsl fn that returns a lin interp btwn 2 vals based on some t s.t. 0<t<1
    float i1 = glm::mix(v1, v2, fractX);
    float i2 = glm::mix(v3, v4, fractX);
    return glm::mix(i1, i2, fractY);
}

float Terrain::fbm(const glm::vec2 uv) {
    float total = 0;
    float persistence = 0.5f;
    int octaves = 8;
    float freq = 2.f;
    float amp = 0.5f;
    for(int i = 1; i <= octaves; i++) {
        total += interpNoise(uv.x * freq,
                               uv.y * freq) * amp;
        freq *= 2.f;
        amp *= persistence;
    }
    return total;
}

float Terrain::surflet(glm::vec2 P, glm::vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial
    float distX = abs(P.x - gridPoint.x);
    float distY = abs(P.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.f) + 15 * pow(distX, 4.f) - 10 * pow(distX, 3.f);
    float tY = 1 - 6 * pow(distY, 5.f) + 15 * pow(distY, 4.f) - 10 * pow(distY, 3.f);
    // Get the random vector for the grid point
    glm::vec2 gradient = 2.f * noise2D(gridPoint) - glm::vec2(1.f);
    // Get the vector from the grid point to P
    glm::vec2 diff = P - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}

float Terrain::perlin(glm::vec2 uv) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
            for(int dy = 0; dy <= 1; ++dy) {
                surfletSum += surflet(uv, glm::floor(uv) + glm::vec2(dx, dy));
            }
    } return surfletSum;
}

float Terrain::worley(glm::vec2 uv) {
    uv *= 10; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::fract(uv);
    float minDist = 1.0; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = noise2D(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            glm::vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = glm::length(diff);
            minDist = glm::min(minDist, dist);
        }
    } return minDist;
}

float Terrain::hills(glm::vec2 xz) {
    float h = 0;
    float freq = 200.f;
    float dF = 0.5;

    for (int i = 0; i < 4; ++i) {
        h += perlin(xz / freq);
        freq *= dF;
    }

    float flatten = 2.f;
    float sharpen = 1.25f;
    float max = -0.25f;
    float min = -0.9f;
    if (h > max) {
        h -= max;
        h /= flatten;
        h += max;
    }

    if (h < min) {
        h -= min;
        h *= sharpen;
        h += min;
    } return floor(130.f + (h * 50.f));
}

float Terrain::mountains(glm::vec2 xz) {
    float h = 0;
    float amp = 0.5;
    float freq = 175.f;

    for (int i = 0; i < 4; ++i) {
        float h1 = perlin(xz / freq);
        h1 = 1. - abs(h1);
        h1 = pow(h1, 1.25);
        h += h1 * amp;

        amp *= 0.5;
        freq *= 0.5;
    } return floor(100.f + h * 150.f);
}

float Terrain::forest(glm::vec2 xz) {
    float h = 0;

    float amp = 0.5;
    float freq = 90.f;

    for (int i = 0; i < 4; ++i) {
        h += amp * perlin(xz / freq);
        freq *= 0.5;
        amp *= 0.5;
    } return floor(150.f + h * 20);
}

float Terrain::islands(glm::vec2 xz) {
    float h = 0;

    float amp = 0.5;
    float freq = 200.f;

    for (int i = 0; i < 4; ++i) {
        h += amp * perlin(xz / freq);
        freq *= 0.25;
        amp *= 0.25;
    } return floor(35.f + h * 100);
}

float Terrain::blendTerrain(glm::vec2 uv, float h1, float h2) {
    double p = perlin(uv);
    float heightMix = glm::smoothstep(0.25, 0.75, p);

    // perform linear interpolation
    float height = ((1 - heightMix) * h1) + (heightMix * h2);
    return height;
}
