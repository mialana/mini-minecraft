#include "entity.h"
#include <QJsonArray>
#include <iostream>
#include "mygl.h"

Entity::Entity(OpenGLContext* context)
    :  Entity(glm::vec3(0, 0, 0), context)
{}

Entity::Entity(glm::vec3 pos, OpenGLContext* context)
    : m_velocity(0, 0, 0), m_acceleration(0, 0, 0), m_forward(0, 0, -1), m_right(1, 0, 0), m_up(0, 1, 0), m_position(pos), m_geom3D(context)
{}

Entity::Entity(const Entity& e, OpenGLContext* context)
    : m_velocity(0, 0, 0), m_acceleration(0, 0, 0), m_forward(e.m_forward), m_right(e.m_right), m_up(e.m_up), m_position(e.m_position),
      m_geom3D(context)
{}

Entity::~Entity()
{}


void Entity::moveAlongVector(glm::vec3 dir) {
    m_position += dir;
}

void Entity::moveForwardLocal(float amount) {
    m_position += amount * m_forward;
}

void Entity::moveRightLocal(float amount) {
    m_position += amount * m_right;
}

void Entity::moveUpLocal(float amount) {
    m_position += amount * m_up;
}

void Entity::moveForwardGlobal(float amount) {
    m_position += glm::vec3(0, 0, amount);
}

void Entity::moveRightGlobal(float amount) {
    m_position += glm::vec3(amount, 0, 0);
}

void Entity::moveUpGlobal(float amount) {
    m_position += glm::vec3(0, amount, 0);
}

void Entity::rotateOnForwardLocal(float degrees) {
    float rad = glm::radians(degrees);
    glm::mat4 rot(glm::rotate(glm::mat4(), rad, m_forward));
    m_right = glm::vec3(rot * glm::vec4(m_right, 0.f));
    m_up = glm::vec3(rot * glm::vec4(m_up, 0.f));
}

void Entity::rotateOnRightLocal(float degrees) {
    float rad = glm::radians(degrees);
    glm::mat4 rot(glm::rotate(glm::mat4(), rad, m_right));
    m_forward = glm::vec3(rot * glm::vec4(m_forward, 0.f));
    m_up = glm::vec3(rot * glm::vec4(m_up, 0.f));
}

void Entity::rotateOnUpLocal(float degrees) {
    float rad = glm::radians(degrees);
    glm::mat4 rot(glm::rotate(glm::mat4(), rad, m_up));
    m_right = glm::vec3(rot * glm::vec4(m_right, 0.f));
    m_forward = glm::vec3(rot * glm::vec4(m_forward, 0.f));
}


void Entity::rotateOnForwardGlobal(float degrees) {
    float rad = glm::radians(degrees);
    m_forward = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0, 0, 1)) * glm::vec4(m_forward, 0.f));
    m_right = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0, 0, 1)) * glm::vec4(m_right, 0.f));
    m_up = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0, 0, 1)) * glm::vec4(m_up, 0.f));
}

void Entity::rotateOnRightGlobal(float degrees) {
    float rad = glm::radians(degrees);
    m_forward = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(1, 0, 0)) * glm::vec4(m_forward, 0.f));
    m_right = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(1, 0, 0)) * glm::vec4(m_right, 0.f));
    m_up = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(1, 0, 0)) * glm::vec4(m_up, 0.f));
}

void Entity::rotateOnUpGlobal(float degrees) {
    float rad = glm::radians(degrees);
    m_forward = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0, 1, 0)) * glm::vec4(m_forward, 0.f));
    m_right = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0, 1, 0)) * glm::vec4(m_right, 0.f));
    m_up = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0, 1, 0)) * glm::vec4(m_up, 0.f));
}

void Entity::constructSceneGraph(QJsonArray data) {
    bodyT = mkU<TranslateNode>(
                &m_geom3D, glm::vec3(m_position.x, m_position.y + 1.05f, m_position.z));
    nodePointerMap.insert({"BodyT", bodyT.get()});

    for (auto i = data.begin(), end = data.end(); i != end; i++) {
        QJsonObject obj = i->toObject();
        QString key = obj["name"].toString();

        uPtr<Node> newNode;

        if (obj["nodeType"].toString() == "translation") {
            glm::vec3 translation =
                MyGL::convertQJsonArrayToGlmVec3(obj["translation"].toArray());
            newNode = mkU<TranslateNode>(&m_geom3D, translation);
        } else if (obj["nodeType"].toString() == "rotation") {
            int degrees = obj["degrees"].toInt();
            glm::vec3 aor =
                MyGL::convertQJsonArrayToGlmVec3(obj["axisOfRotation"].toArray());
            newNode = mkU<RotateNode>(&m_geom3D, degrees, aor);
        } else if (obj["nodeType"].toString() == "scale") {
            glm::vec3 scale = MyGL::convertQJsonArrayToGlmVec3(obj["scale"].toArray());
            newNode = mkU<ScaleNode>(&m_geom3D, scale);
            newNode->geomType = obj["geomType"].toString();
        }

        newNode->name = key;
        nodePointerMap.insert({key, newNode.get()});

        QString parent = obj["parent"].toString();
        nodePointerMap.at(parent)->addChild(std::move(newNode));
    }
}

void Entity::animate(float dT, InputBundle& inputs) {
    m_timer += (dT);
    if (m_timer > 50.f * M_PI) {
        m_timer = 0.f;
    }
    float maxAngle = 30.f;
    Player* p = dynamic_cast<Player*>(this);
    if ((p != nullptr && inputs.inThirdPerson) || p == nullptr) {
        if (inputs.isMoving) {
            if (nodePointerMap["LeftArmR"] != nullptr) {
                (static_cast<RotateNode*>(nodePointerMap["LeftArmR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f + M_PI);
            }
            if (nodePointerMap["RightArmR"] != nullptr) {
                (static_cast<RotateNode*>(nodePointerMap["RightArmR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f);
            }
            if (nodePointerMap["LeftLegR"] != nullptr) {
                (static_cast<RotateNode*>(nodePointerMap["LeftLegR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f);
            }
            if (nodePointerMap["RightLegR"] != nullptr) {
                (static_cast<RotateNode*>(nodePointerMap["RightLegR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f + M_PI);
            }
        } else {
            if (nodePointerMap["LeftArmR"] != nullptr && glm::abs((static_cast<RotateNode*>(nodePointerMap["LeftArmR"]))->degrees) >= 3.f) {
                (static_cast<RotateNode*>(nodePointerMap["LeftArmR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f + M_PI);
            } else {
                (static_cast<RotateNode*>(nodePointerMap["LeftArmR"]))->degrees = 0.f;
            }
            if (nodePointerMap["RightArmR"] != nullptr && glm::abs((static_cast<RotateNode*>(nodePointerMap["RightArmR"]))->degrees) >= 3.f) {
                (static_cast<RotateNode*>(nodePointerMap["RightArmR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f);
            } else {
                (static_cast<RotateNode*>(nodePointerMap["RightArmR"]))->degrees = 0.f;
            }
            if (nodePointerMap["LeftLegR"] != nullptr && glm::abs((static_cast<RotateNode*>(nodePointerMap["LeftLegR"]))->degrees) >= 3.f) {
                (static_cast<RotateNode*>(nodePointerMap["LeftLegR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f);
            } else {
                (static_cast<RotateNode*>(nodePointerMap["LeftLegR"]))->degrees = 0.f;
            }
            if (nodePointerMap["RightLegR"] != nullptr && glm::abs((static_cast<RotateNode*>(nodePointerMap["RightLegR"]))->degrees) >= 3.f) {
                (static_cast<RotateNode*>(nodePointerMap["RightLegR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f + M_PI);
            } else {
                (static_cast<RotateNode*>(nodePointerMap["RightLegR"]))->degrees = 0.f;
            }
        }
    }

    glm::mat4 bodyRotateMatrix = glm::lookAt(glm::vec3(), glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z)), glm::vec3(0, 1, 0));
    (static_cast<RotateNode*>(nodePointerMap["BodyR"]))->overriddenTransformMatrix = glm::inverse(bodyRotateMatrix);

    glm::mat4 headRotateMatrix = glm::lookAt(m_position + glm::vec3(0.f, 1.65f, 0.f), m_position + glm::vec3(0.f, 1.65f, 0.f) + m_forward, glm::vec3(0, 1, 0));
    (static_cast<RotateNode*>(nodePointerMap["HeadR"]))->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
}

void Entity::drawSceneGraph(const uPtr<Node>& currNode, glm::mat4 currTransformation, ShaderProgram& progShader) {
    if (currNode->name == "BodyR") {
        currTransformation *= currNode->overriddenTransformMatrix;
    } else if (currNode->name == "HeadR") {
        currTransformation = glm::translate(this->m_position + glm::vec3(0.f, 1.65f, 0.f)) * currNode->overriddenTransformMatrix;
    } {
        currTransformation *= currNode->transformMatrix();
    }

    for (const auto& n : currNode->getChildren()) {
        drawSceneGraph(n, currTransformation, progShader);
    }

    if (currNode->geomType != "") {
        progShader.setModelMatrix(currTransformation);

        currNode->geometry->addType(currNode->geomType);

        currNode->geometry->destroyVBOdata();
        currNode->geometry->createVBOdata();

        progShader.draw(*(currNode->geometry));
    }
}
