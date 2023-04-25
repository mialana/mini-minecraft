#pragma once

#include "entity.h"
#include "scene/chunk.h"

class Zombie : public Entity
{
public:
    bool needsRespawn;

    Zombie(OpenGLContext*);

    void tick(float dT, InputBundle& inputs);

    void respawn(Chunk* c);
};
