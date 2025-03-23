#pragma once

#include "drawable.h"
#include <glm_includes.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QJsonObject>

class Geometry3D : public Drawable
{
public:
    QString m_type;
    std::vector<glm::vec4> m_positions;
    std::vector<glm::vec4> m_uvs;
    std::unordered_map<QString, std::vector<glm::vec4>> m_uvObj;

    Geometry3D(OpenGLContext* context);

    void addType(QString t);
    static void createIndices(std::vector<GLuint>& indices);
    static void createNormals(std::vector<glm::vec4>& normals);
    void createVBOdata() override;
};
