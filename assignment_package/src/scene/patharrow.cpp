#include "patharrow.h"

PathArrow::PathArrow(OpenGLContext* mp_context)
    : Drawable(mp_context)
    , m_arrowColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
    , m_arrowLength(1.f)
{
    m_indices = {
        0,  1,  2,  1,  2,  3,              // one face
        1,  3,  6,  3,  6,  7,              // one face
        6,  7,  10, 7,  10, 11,             // one face
        10, 11, 12, 11, 12, 13,             // one face
        8,  9,  12, 9,  12, 13,             // one face
        4,  5,  8,  5,  8,  9,              // one face
        0,  2,  4,  2,  4,  5,              // one face
        0,  1,  4,  1,  4,  6,              // one face
        2,  3,  5,  3,  5,  7,              // one face
        4,  8,  12, 4,  6,  12, 6, 10, 12,  // arrow face
        5,  9,  13, 5,  7,  11, 7, 11, 13   // arrow face
    };

    this->_generatePositions();
    this->_generateNormals();

    for (int i = 0; i < 14; i++) {
        m_colors.push_back(glm::vec4(m_arrowColor));
    }
}

void PathArrow::_generatePositions()
{
    m_positions.push_back(glm::vec4(-0.1f, 0.1f, 0.f, 1.f));
    m_positions.push_back(glm::vec4(0.1f, 0.1f, 0.f, 1.f));
    m_positions.push_back(glm::vec4(-0.1f, -0.1f, 0.f, 1.f));
    m_positions.push_back(glm::vec4(0.1f, -0.1f, 0.f, 1.f));

    m_positions.push_back(glm::vec4(-0.1f, 0.1f, -m_arrowLength, 1.f));
    m_positions.push_back(glm::vec4(-0.1f, -0.1f, -m_arrowLength, 1.f));

    m_positions.push_back(glm::vec4(0.1f, 0.1f, -m_arrowLength, 1.f));
    m_positions.push_back(glm::vec4(0.1f, -0.1f, -m_arrowLength, 1.f));

    m_positions.push_back(glm::vec4(-0.2f, 0.1f, -m_arrowLength, 1.f));
    m_positions.push_back(glm::vec4(-0.2f, -0.1f, -m_arrowLength, 1.f));

    m_positions.push_back(glm::vec4(0.2f, 0.1f, -m_arrowLength, 1.f));
    m_positions.push_back(glm::vec4(0.2f, -0.1f, -m_arrowLength, 1.f));

    m_positions.push_back(glm::vec4(0.f, 0.1f, -m_arrowLength - 0.2f, 1.f));
    m_positions.push_back(glm::vec4(0.f, -0.1f, -m_arrowLength - 0.2f, 1.f));
}

void PathArrow::_generateNormals()
{
    if (m_indices.empty()) {
        qDebug() << "Fill indices first";
    }

    m_normals.push_back(glm::vec4(0.f, 0.f, 1.f, 1.f));
    m_normals.push_back(glm::vec4(0.f, 0.f, 1.f, 1.f));

    //
    m_normals.push_back(glm::vec4(1.f, 0.f, 0.f, 1.f));
    m_normals.push_back(glm::vec4(1.f, 0.f, 0.f, 1.f));

    m_normals.push_back(glm::vec4(0.f, 0.f, 1.f, 1.f));
    m_normals.push_back(glm::vec4(0.f, 0.f, 1.f, 1.f));

    m_normals.push_back(glm::vec4(1.f, 0.f, -1.f, 1.f));
    m_normals.push_back(glm::vec4(1.f, 0.f, -1.f, 1.f));

    m_normals.push_back(glm::vec4(-1.f, 0.f, -1.f, 1.f));
    m_normals.push_back(glm::vec4(-1.f, 0.f, -1.f, 1.f));

    m_normals.push_back(glm::vec4(0.f, 0.f, 1.f, 1.f));
    m_normals.push_back(glm::vec4(0.f, 0.f, 1.f, 1.f));

    m_normals.push_back(glm::vec4(-1.f, 0.f, 0.f, 1.f));
    m_normals.push_back(glm::vec4(-1.f, 0.f, 0.f, 1.f));

    m_normals.push_back(glm::vec4(0.f, 1.f, 0.f, 1.f));
    m_normals.push_back(glm::vec4(0.f, 1.f, 0.f, 1.f));

    m_normals.push_back(glm::vec4(0.f, -1.f, 0.f, 1.f));
    m_normals.push_back(glm::vec4(0.f, -1.f, 0.f, 1.f));

    m_normals.push_back(glm::vec4(0.f, 1.f, 0.f, 1.f));
    m_normals.push_back(glm::vec4(0.f, 1.f, 0.f, 1.f));
    m_normals.push_back(glm::vec4(0.f, 1.f, 0.f, 1.f));

    m_normals.push_back(glm::vec4(0.f, -1.f, 0.f, 1.f));
    m_normals.push_back(glm::vec4(0.f, 1.f, 0.f, 1.f));
    m_normals.push_back(glm::vec4(0.f, 1.f, 0.f, 1.f));
}

bool PathArrow::changeColor(glm::vec4 c)
{
    if (m_arrowColor != c) {
        m_arrowColor = c;

        m_colors.clear();

        for (int i = 0; i < 14; i++) {
            m_colors.push_back(c);
        }

        this->destroyVBOdata();
        this->createVBOdata();

        return true;
    }
    return false;
}

bool PathArrow::changeLength(float l)
{
    if (m_arrowLength != l) {
        m_arrowLength = l;

        m_positions.clear();
        _generatePositions();

        this->destroyVBOdata();
        this->createVBOdata();

        return true;
    }
    return false;
}

void PathArrow::createVBOdata()
{
    m_oCount = m_indices.size();

    generateOIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_indices.size() * sizeof(GLuint),
                             m_indices.data(),
                             GL_STATIC_DRAW);

    generateOPos();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufPos);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_positions.size() * sizeof(glm::vec4),
                             m_positions.data(),
                             GL_STATIC_DRAW);

    generateONor();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufNor);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_normals.size() * sizeof(glm::vec4),
                             m_normals.data(),
                             GL_STATIC_DRAW);

    generateOCol();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufCol);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_colors.size() * sizeof(glm::vec4),
                             m_colors.data(),
                             GL_STATIC_DRAW);
}

bool PathArrow::hasVBOdata()
{
    return this->m_oIdxGenerated && this->m_oPosGenerated && this->m_oNorGenerated
           && this->m_oColGenerated;
}
