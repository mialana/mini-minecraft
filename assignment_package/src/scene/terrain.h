#pragma once
#include "biome.h"
#include "chunk.h"
#include "scene/mob.h"
#include "shaderprogram.h"
#include "smartpointerhelp.h"
#include "workers.h"
#include <QMutex>
#include <QThreadPool>
#include <array>
#include <unordered_map>
#include <unordered_set>

const static std::vector<glm::ivec2> directionHelper = {
    glm::ivec2(16, 0),
    glm::ivec2(-16, 0),
    glm::ivec2(0, 16),
    glm::ivec2(0, -16),
};

// Helper functions to convert (x, z) to and from hash map key
int64_t toKey(int x, int z);
glm::ivec2 toCoords(int64_t k);

// The container class for all of the Chunks in the game.
// Ultimately, while Terrain will always store all Chunks,
// not all Chunks will be drawn at any given time as the world
// expands.
class Terrain
{
private:
    // Set and mutex of chunks that have block data
    std::unordered_set<Chunk*> m_blockDataChunks;
    QMutex m_blockDataChunksLock;
    // Vector and mutex of chunks that have VBO data
    std::vector<Chunk*> m_vboDataChunks;
    QMutex m_VBODataChunksLock;

    // Stores every Chunk according to the location of its lower-left corner
    // in world space.
    // We combine the X and Z coordinates of the Chunk's corner into one 64-bit
    // int so that we can use them as a key for the map, as objects like
    // std::pairs or glm::ivec2s are not hashable by default, so they cannot be
    // used as keys.
    std::unordered_map<int64_t, uPtr<Chunk>> m_chunks;

    // We will designate every 64 x 64 area of the world's x-z plane
    // as one "terrain generation zone". Every time the player moves
    // near a portion of the world that has not yet been generated
    // (i.e. its lower-left coordinates are not in this set), a new
    // 4 x 4 collection of Chunks is created to represent that area
    // of the world.
    // The world that exists when the base code is run consists of exactly
    // one 64 x 64 area with its lower-left corner at (0, 0).
    // When milestone 1 has been implemented, the Player can move around the
    // world to add more "terrain generation zone" IDs to this set.
    // While only the 3 x 3 collection of terrain generation zones
    // surrounding the Player should be rendered, the Chunks
    // in the Terrain will never be deleted until the program is terminated.
    std::unordered_set<int64_t> m_generatedTerrain;

    // TODO: DELETE ALL REFERENCES TO m_geomCube AS YOU WILL NOT USE
    // IT IN YOUR FINAL PROGRAM!
    // The instance of a unit cube we can use to render any cube.
    // Presently, Terrain::draw renders one instance of this cube
    // for every non-EMPTY block within its Chunks. This is horribly
    // inefficient, and will cause your game to run very slowly until
    // milestone 1's Chunk VBO setup is completed.

    bool firstTick = true;

public:
    Terrain(OpenGLContext* context);
    ~Terrain();

    OpenGLContext* mp_context;

    // multithreading functions

    float m_chunkTimer = 0.0f;

    void multithreadedWork(glm::vec3, glm::vec3, float);

    QSet<long long> borderingZone(glm::ivec2 coords, int radius, bool atEdge);
    void tryExpansion(glm::vec3 pos, glm::vec3 prevPos);
    void createVBOWorker(Chunk* chunk);
    void createVBOWorkers(const std::unordered_set<Chunk*>& chunks);
    void createBDWorker(long long zone);
    void checkThreadResults();

    // Instantiates a new Chunk and stores it in
    // our chunk map at the given coordinates.
    // Returns a pointer to the created Chunk.
    Chunk* instantiateChunkAt(int x, int z);
    // Do these world-space coordinates lie within
    // a Chunk that exists?
    bool hasChunkAt(int x, int z) const;
    // Assuming a Chunk exists at these coords,
    // return a mutable reference to it
    uPtr<Chunk>& getChunkAt(int x, int z);
    // Assuming a Chunk exists at these coords,
    // return a const reference to it
    const uPtr<Chunk>& getChunkAt(int x, int z) const;
    // Given a world-space coordinate (which may have negative
    // values) return the block stored at that point in space.
    BlockType getBlockAt(int x, int y, int z) const;
    BlockType getBlockAt(glm::vec3 p) const;

    // mountains = 0, hills = 1, forest = 2, islands = 3, caves = 4
    glm::vec4 getBiomeAt(glm::vec2 p) const;
    glm::vec4 getBiomeAt(int x, int z) const;

    // Given a world-space coordinate (which may have negative
    // values) set the block at that point in space to the
    // given type.
    void setBlockAt(int x, int y, int z, BlockType t);

    // Given a world-space coordinate (which may have negative
    // values) set the block at that point in space to the
    // given type. Then reset the VBO data of the chunk of that block.
    void changeBlockAt(int x, int y, int z, BlockType bt);

    void setBiomeAt(int x, int z, glm::vec4 b);

    // Draws every Chunk that falls within the bounding box
    // described by the min and max coords, using the provided
    // ShaderProgram
    void draw(int minX,
              int maxX,
              int minZ,
              int maxZ,
              ShaderProgram* shaderProgram,
              std::vector<uPtr<Mob>>& currMobs);

    // Initializes the Chunks that store the 64 x 256 x 64 block scene you
    // see when the base code is run.
    void CreateTestScene();

    void loadNewChunks(glm::vec3);

    std::pair<float, BiomeEnum> blendMultipleBiomes(glm::vec2 xz,
                                                    float forestH,
                                                    float mountH,
                                                    float hillH,
                                                    float islandH);
};
