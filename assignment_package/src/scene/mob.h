#pragma once

#include "entity.h"
#include "scene/chunk.h"

class Zombie : public Entity
{
public:
    float timeSinceLastPathRecompute;
    bool needsRespawn;
    glm::vec3 directionOfTravel;

    Zombie(OpenGLContext*);

    void tick(float dT, Terrain& terrain) override;

    void respawn(Chunk* c);

    void pathFind();
};
