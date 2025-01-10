#include "camera.h"

Camera::Camera(MyGL& context, Terrain& terrain, glm::vec3 pos, std::optional<unsigned int> w, std::optional<unsigned int> h)
    : Entity(context, terrain, pos), m_fovy(45), m_width(w.value()), m_height(h.value()),
      m_near_clip(0.5f), m_far_clip(1000.f), m_aspect(w.value() / static_cast<float>(h.value()))
{}

void Camera::setWidthHeight(unsigned int w, unsigned int h) {
    m_width = w;
    m_height = h;
    m_aspect = w / static_cast<float>(h);
}

glm::mat4 Camera::getViewProj() const {
    return glm::perspective(glm::radians(m_fovy), m_aspect, m_near_clip,
                            m_far_clip) * glm::lookAt(m_position, m_position + m_forward, m_up);
}
