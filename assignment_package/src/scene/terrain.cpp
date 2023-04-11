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

            shaderProgram->setModelMatrix(glm::translate(glm::mat4(1), glm::vec3(x, 0, z)));
            shaderProgram->drawInterleaved(*currChunk);
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

            float hillsY = Biome::hills(glm::vec2(x, z));
            float mountainsY = Biome::mountains(glm::vec2(x, z));
            float forestY = Biome::forest(glm::vec2(x, z));
            float islandsY = Biome::islands(glm::vec2(x, z));

            float hillsMountainsY = Biome::blendTerrain(glm::vec2(x, z), hillsY, mountainsY);
            float forestIslandsY = Biome::blendTerrain(glm::vec2(x, z), forestY, islandsY);
            float y = Biome::blendTerrain(glm::vec2(x, z), hillsMountainsY, forestIslandsY);

            float biomeType = Biome::blendTerrain(glm::vec2(x, z), 0.f, 1.f);
            int numDirtBlocks = 10 * Biome::fbm(glm::vec2(x, z));
            if (biomeType >= 0.5) {
                // hills
                for (int currY = 0; currY < y - 3 - numDirtBlocks; ++currY) {
                    setBlockAt(x, currY, z, STONE);
                }
                for (int currY = y - 3 - numDirtBlocks; currY < y - 1; ++currY) {
                    setBlockAt(x, currY, z, DIRT);
                }
                setBlockAt(x, y - 1, z, GRASS);
            } else {
                // mountains
                if (y <= 100) {
                    for (int currY = 0; currY < y - 1; ++currY) {
                        setBlockAt(x, currY, z, STONE);
                    }
                } else {
                    for (int currY = 0; currY < y - numDirtBlocks - 1; ++currY) {
                        setBlockAt(x, currY, z, STONE);
                    }
                    for (int currY = y - numDirtBlocks - 1; currY < y - 2; ++currY) {
                        setBlockAt(x, currY, z, DIRT);
                    }
                    setBlockAt(x, y - 2, z, GRASS);
                    setBlockAt(x, y - 1, z, SNOW);
                }
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
    }

    for (int i = 48; i < 96; i++) {
        for (int j = 0; j < 48; j++) {
            setBlockAt(i, 128, j, DIRT);
            setBlockAt(i, 129, j, WATER);

            if (i >= 72) {
                setBlockAt(i, 130, j, WATER);
            }
            if (i >= 84) {
                setBlockAt(i, 131, j, WATER);
                setBlockAt(i, 132, j, WATER);
                setBlockAt(i, 133, j, WATER);
                setBlockAt(i, 134, j, WATER);
                setBlockAt(i, 135, j, WATER);
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
