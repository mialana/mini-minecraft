#pragma once

#include "entity.h"
#include "scene/chunk.h"

class Mob : public Entity
{
public:
    float timeSinceLastPathRecompute;
    bool needsRespawn;
    glm::vec3 directionOfTravel;

    Mob(MyGL&, Terrain&);

    void tick(float dT) override;

    void respawn(Chunk* c, std::optional<glm::vec3> = std::nullopt);

    void pathFind();
};
