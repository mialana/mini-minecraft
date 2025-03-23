#pragma once

#include "entity.h"
#include "scene/chunk.h"

class Mob : public Entity
{
public:
    float timeSinceLastPathRecompute;
    float timeSinceLastDirectionCompute;
    bool needsRespawn;
    glm::vec3 directionOfTravel;

    Mob(OpenGLContext*);

    void tick(float dT, Terrain& terrain) override;

    void respawn(Chunk* c);

    void pathFind();
};
