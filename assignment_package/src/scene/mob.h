#pragma once

#include "entity.h"
#include "scene/chunk.h"

class Mob : public Entity
{
private:
    bool m_showPathArrow;
    PathArrow m_pathArrow;
    glm::vec3 m_lastPosition;
    glm::vec3 m_realDirection;

public:
    float timeSinceLastPathRecompute;
    float timeSinceLastDirectionCompute;
    bool needsRespawn;
    glm::vec3 directionOfTravel;

    Mob(OpenGLContext*);

    void tick(float dT, Terrain& terrain) override;

    void respawn(Chunk* c);

    bool getShowPathArrow();
    void drawPathArrow(ShaderProgram& progFlat);

    /**
     * @brief changeShowPathArrow
     * Note: Also resets VBOdata
     * @return new value of `m_showPathArrow`
     */
    bool changeShowPathArrow();

    void pathFind();
};
