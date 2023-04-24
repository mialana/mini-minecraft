#include "geometry3d.h"
#include <QJsonArray>
#include "mygl.h"

Geometry3D::Geometry3D(OpenGLContext* context)
    : Drawable(context) {
    QJsonObject dataObj = MyGL::importJson(":/data/geom3dData.json");
    QJsonArray jsonPosArr = dataObj["Geometry3DPositions"].toArray();
    QJsonObject jsonUVObj = dataObj["PlayerUVCoordinates"].toObject();

    for (int i = 0; i < 24; i++) {
        QJsonArray posVectorArr = jsonPosArr[i].toArray();
        this->m_positions.push_back(MyGL::convertQJsonArrayToGlmVec4(posVectorArr));
    }

    for (auto geomType : jsonUVObj.keys()) {
        QJsonArray uvVectorArr = jsonUVObj[geomType].toArray();
        std::vector<glm::vec4> newArr;

        for (int i = 0; i < 24; i++) {
            newArr.push_back(MyGL::convertQJsonArrayToGlmVec4(uvVectorArr[i].toArray()));
        }

        this->m_uvObj.insert({geomType, newArr});
    }
}

void Geometry3D::addType(QString t) {
    this->m_type = t;
    this->m_uvs = this->m_uvObj[t];
}

void Geometry3D::createIndices(std::vector<GLuint>& indices) {
    for (int i = 0; i < 6; i++) {
        indices.push_back(i * 4);
        indices.push_back(i * 4 + 1);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 3);
    }
}

void Geometry3D::createNormals(std::vector<glm::vec4>& normals) {
    for (int i = 0; i < 4; i++) {
        normals.push_back(glm::vec4(0.f, 0.f, -1.f, 0.f));
    }

    for (int i = 0; i < 4; i++) {
        normals.push_back(glm::vec4(1.f, 0.f, 0.f, 0.f));
    }

    for (int i = 0; i < 4; i++) {
        normals.push_back(glm::vec4(-1.f, 0.f, 0.f, 0.f));
    }

    for (int i = 0; i < 4; i++) {
        normals.push_back(glm::vec4(0.f, 0.f, 1.f, 0.f));
    }

    for (int i = 0; i < 4; i++) {
        normals.push_back(glm::vec4(0.f, 1.f, 0.f, 0.f));
        ;
    }

    for (int i = 0; i < 4; i++) {
        normals.push_back(glm::vec4(0.f, -1.f, 0.f, 0.f));
    }
}

void Geometry3D::createVBOdata() {
    std::vector<GLuint> indices;
    std::vector<glm::vec4> normals;
    Geometry3D::createIndices(indices);
    Geometry3D::createNormals(normals);

    m_oCount = indices.size();

    generateOIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(),
                             GL_STATIC_DRAW);

    generateOPos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER,
                             m_positions.size() * sizeof(glm::vec4),
                             m_positions.data(), GL_STATIC_DRAW);
    generateONor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec4),
                             normals.data(), GL_STATIC_DRAW);
    generateOUVs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufUVs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(glm::vec4),
                             m_uvs.data(), GL_STATIC_DRAW);

}
