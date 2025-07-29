#pragma once
#include "glm_includes.h"
#include "scene/entity.h"

//A perspective projection camera
//Receives its eye position and reference point from the scene XML file
class Camera : public Entity
{
private:
    float m_fovy;
    unsigned int m_width, m_height;  // Screen dimensions
    float m_near_clip;               // Near clip plane distance
    float m_far_clip;                // Far clip plane distance
    float m_aspect;                  // Aspect ratio

public:
    Camera(glm::vec3 pos, OpenGLContext* context);
    Camera(unsigned int w, unsigned int h, glm::vec3 pos, OpenGLContext* context);
    Camera(const Camera& c, OpenGLContext* context);
    void setWidthHeight(unsigned int w, unsigned int h);

    void tick(float dT, Terrain& terrain) override;

    glm::mat4 getViewProj() const;
};
