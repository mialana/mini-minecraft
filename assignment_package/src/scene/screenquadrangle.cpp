#include "screenquadrangle.h"

ScreenQuadrangle::ScreenQuadrangle(OpenGLContext *context) : Drawable(context)
{}

void ScreenQuadrangle::createVBOdata()
{
    std::vector<GLuint> indices = {0, 1, 2, 0, 2, 3};
    std::vector<glm::vec4> positions = {glm::vec4(-1.f, -1.f, 0.99f, 1.f),
                                        glm::vec4(1.f, -1.f, 0.99f, 1.f),
                                        glm::vec4(1.f, 1.f, 0.99f, 1.f),
                                        glm::vec4(-1.f, 1.f, 0.99f, 1.f)};

    std::vector<glm::vec4> colors {glm::vec4(0.f, 0.f, 0.f, 1.f),
                                glm::vec4(1.f, 0.f, 0.f, 1.f),
                                glm::vec4(1.f, 1.f, 0.f, 1.f),
                                glm::vec4(0.f, 1.f, 0.f, 1.f)};

    m_oCount = 6;

    generateOIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    generateOPos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    generateOCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);
}
