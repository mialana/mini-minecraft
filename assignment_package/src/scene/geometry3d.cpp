#include "geometry3d.h"
#include <QJsonArray>
#include "mygl.h"

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;

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

//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createCubeVertexPositions(glm::vec4 (&cub_vert_pos)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    for (glm::vec4 pos : cub_vert_pos) {
        pos -= 0.5;
    }
}


void createCubeVertexNormals(glm::vec4 (&cub_vert_nor)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,0,-1,0);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(1,0,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(-1,0,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,0,1,0);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,1,0,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,-1,0,0);
    }
}

void createCubeIndices(GLuint (&cub_idx)[CUB_IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}

void Geometry3D::createVBOdata() {
    GLuint sph_idx[CUB_IDX_COUNT];
    glm::vec4 sph_vert_nor[CUB_VERT_COUNT];

    createCubeVertexNormals(sph_vert_nor);
    createCubeIndices(sph_idx);

    m_oCount = CUB_IDX_COUNT;

    generateOIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, CUB_IDX_COUNT * sizeof(GLuint), sph_idx, GL_STATIC_DRAW);
    generateOPos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, CUB_VERT_COUNT * sizeof(glm::vec4), m_positions.data(), GL_STATIC_DRAW);
    generateONor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, CUB_VERT_COUNT * sizeof(glm::vec4), sph_vert_nor, GL_STATIC_DRAW);
    generateOCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_oBufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, CUB_VERT_COUNT * sizeof(glm::vec4), m_uvs.data(), GL_STATIC_DRAW);

}
