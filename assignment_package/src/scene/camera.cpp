#include "camera.h"
#include "glm_includes.h"

Camera::Camera(glm::vec3 pos, OpenGLContext* context)
    : Camera(400, 400, pos, context)
{}

Camera::Camera(unsigned int w, unsigned int h, glm::vec3 pos, OpenGLContext* context)
    : Entity(pos, context), m_fovy(45), m_width(w), m_height(h),
      m_near_clip(0.5f), m_far_clip(1000.f), m_aspect(w / static_cast<float>(h))
{}

Camera::Camera(const Camera& c, OpenGLContext* context)
    : Entity(c, context),
      m_fovy(c.m_fovy),
      m_width(c.m_width),
      m_height(c.m_height),
      m_near_clip(c.m_near_clip),
      m_far_clip(c.m_far_clip),
      m_aspect(c.m_aspect)
{}


void Camera::setWidthHeight(unsigned int w, unsigned int h) {
    m_width = w;
    m_height = h;
    m_aspect = w / static_cast<float>(h);
}


void Camera::tick(float dT, Terrain& terrain) {
    // Do nothing
}

glm::mat4 Camera::getViewProj() const {
    return glm::perspective(glm::radians(m_fovy), m_aspect, m_near_clip,
                            m_far_clip) * glm::lookAt(m_position, m_position + m_forward, m_up);
}
