#include "entity.h"
#include <QJsonArray>
#include <iostream>
#include "mygl.h"
#include "terrain.h"

Entity::Entity(OpenGLContext* context)
    :  Entity(glm::vec3(0, 0, 0), context)
{}

Entity::Entity(glm::vec3 pos, OpenGLContext* context)
    : m_inputs(InputBundle()), infAxis(-1), m_velocity(glm::vec3(0, 0, 0)), m_acceleration(glm::vec3(0, 0, 0)), m_forward(0, 0, -1), m_right(1, 0, 0), m_up(0, 1, 0), m_position(pos),
      m_geom3D(context)
{}

Entity::Entity(const Entity& e, OpenGLContext* context)
    : m_inputs(e.m_inputs), infAxis(e.infAxis), m_velocity(e.m_velocity), m_acceleration(e.m_acceleration), m_forward(e.m_forward), m_right(e.m_right),
      m_up(e.m_up), m_position(e.m_position), m_geom3D(context)
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
    if (m_inputs.isPig) {
        bodyT = mkU<TranslateNode>(
                    &m_geom3D, glm::vec3(m_position.x, m_position.y + 0.8, m_position.z));
    } else {
        bodyT = mkU<TranslateNode>(
                    &m_geom3D, glm::vec3(m_position.x, m_position.y + 1.05f, m_position.z));
    }

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
            glm::vec3 aor = MyGL::convertQJsonArrayToGlmVec3(obj["axisOfRotation"].toArray());

            if (m_inputs.isZombie) {
                if (key == "LeftArmR" || key == "RightArmR") {
                    degrees = 90;
                }
            }
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

void Entity::animate(float dT) {
    m_timer += (dT);
    if (m_timer > 50.f * M_PI) {
        m_timer = 0.f;
    }
    float maxAngle = 30.f;

    if (m_inputs.inThirdPerson) {
        if (m_inputs.isMoving) {
            if (nodePointerMap["LeftArmR"] != nullptr && !m_inputs.isZombie) {
                (static_cast<RotateNode*>(nodePointerMap["LeftArmR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f + M_PI);
            }
            if (nodePointerMap["RightArmR"] != nullptr && !m_inputs.isZombie) {
                (static_cast<RotateNode*>(nodePointerMap["RightArmR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f);
            }
            if (nodePointerMap["LeftLegR"] != nullptr) {
                (static_cast<RotateNode*>(nodePointerMap["LeftLegR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f);
            }
            if (nodePointerMap["RightLegR"] != nullptr) {
                (static_cast<RotateNode*>(nodePointerMap["RightLegR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f + M_PI);
            }
        } else {
            if (nodePointerMap["LeftArmR"] != nullptr && glm::abs((static_cast<RotateNode*>(nodePointerMap["LeftArmR"]))->degrees && !m_inputs.isZombie) >= 3.f) {
                (static_cast<RotateNode*>(nodePointerMap["LeftArmR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f + M_PI);
            } else if (!m_inputs.isZombie) {
                (static_cast<RotateNode*>(nodePointerMap["LeftArmR"]))->degrees = 0.f;
            }
            if (nodePointerMap["RightArmR"] != nullptr && glm::abs((static_cast<RotateNode*>(nodePointerMap["RightArmR"]))->degrees && !m_inputs.isZombie) >= 3.f) {
                (static_cast<RotateNode*>(nodePointerMap["RightArmR"]))->degrees = maxAngle * glm::sin(m_timer * 10.f);
            } else if (!m_inputs.isZombie) {
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
}

void Entity::drawSceneGraph(const uPtr<Node>& currNode, glm::mat4 currTransformation, ShaderProgram& progShader) {
    if (currNode->name == "BodyR") {
        currTransformation *= currNode->overriddenTransformMatrix;
    } else if (currNode->name == "HeadR") {
        currTransformation = currNode->overriddenTransformMatrix;
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

void Entity::computePhysics(float dT, Terrain& terrain) {
    glm::vec3 rayDirection = glm::vec3();

    if (!m_inputs.flightMode) {
        float friction = 0.7f;
        m_velocity.x *= friction;
        m_velocity.z *= friction;
        float airFriction = 0.95f;
        m_velocity.y *= airFriction;
        glm::vec3 gravity = glm::vec3(0.f, -12.f, 0.f);
        m_acceleration += gravity;

        if (m_inputs.inLiquid) {
            m_velocity *= 0.5f;
        }
    } else {
        float friction = 0.9f;
        m_velocity *= friction;
    }

    m_velocity += m_acceleration * (dT / 10.f);

    if (!m_inputs.flightMode) {
        m_velocity = glm::vec3(m_velocity.x, glm::min(m_velocity.y, 0.25f), m_velocity.z);
        detectCollision(terrain);
    }

    this->moveAlongVector(m_velocity);
    m_inputs.isMoving = (glm::abs(m_velocity.x / (dT / 1000.f)) >= 3.f) || (glm::abs(m_velocity.z / (dT / 1000.f)) >= 3.f);
}

void Entity::detectCollision(Terrain& terrain) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
    glm::vec3 rayDirectionX = glm::vec3(m_velocity.x, 0.f, 0.f);
    glm::vec3 rayDirectionY = glm::vec3(0.f, m_velocity.y, 0.f);
    glm::vec3 rayDirectionZ = glm::vec3(0.f, 0.f, m_velocity.z);
    float minOutDistY = std::numeric_limits<float>::infinity();
    float minOutDistX = std::numeric_limits<float>::infinity();
    float minOutDistZ = std::numeric_limits<float>::infinity();

    for (int x = 0; x <= 1; x++) {
        for (int z = 2; z <= 3; z++) {
            for (int y = 4; y <= 5; y++) {
                glm::ivec3 outBlockHitX = glm::ivec3();
                glm::ivec3 outBlockHitY = glm::ivec3();
                glm::ivec3 outBlockHitZ = glm::ivec3();
                float outDistX = 0.f;
                float outDistY = 0.f;
                float outDistZ = 0.f;
                glm::vec3 rayOrigin = bottomLeftVertex + glm::vec3(playerDimensions[x],
                                                                   playerDimensions[y],
                                                                   playerDimensions[z]);

                if (gridMarch(rayOrigin, rayDirectionX, &outDistX, &outBlockHitX, terrain) && (outDistX < minOutDistX)) {
                    minOutDistX = outDistX;
                }

                if (gridMarch(rayOrigin, rayDirectionY, &outDistY, &outBlockHitY, terrain) && (outDistY < minOutDistY)) {
                    minOutDistY = outDistY;
                }

                if (gridMarch(rayOrigin, rayDirectionZ, &outDistZ, &outBlockHitZ, terrain) && (outDistZ < minOutDistZ)) {
                    minOutDistZ = outDistZ;
                }
            }
        }
    }

    float threshold = 0.005f;
    bool origCollisionDetected = m_inputs.collisionDetected;

    if (minOutDistY < std::numeric_limits<float>::infinity()) {
        if (minOutDistY < threshold) {
            m_velocity.y = 0.f;
        } else {
            m_velocity.y = (minOutDistY * 0.8) * glm::sign(m_velocity.y);
        }
    }

    if (minOutDistX < std::numeric_limits<float>::infinity()) {
        if (minOutDistX < threshold) {
            m_velocity.x = 0.f;
            this->m_inputs.collisionDetected = true;
        } else {
            m_velocity.x = (minOutDistX * 0.8) * glm::sign(m_velocity.x);
        }
    }

    if (minOutDistZ < std::numeric_limits<float>::infinity()) {
        if (minOutDistZ < threshold) {
            m_velocity.z = 0.f;
            this->m_inputs.collisionDetected = true;
        } else {
            m_velocity.z = (minOutDistZ * 0.8) * glm::sign(m_velocity.z);
        }
    }

    if (origCollisionDetected && m_inputs.collisionDetected) {
        m_inputs.collisionDetected = false;
    }
}

bool Entity::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, float* out_dist, glm::ivec3* out_blockHit, Terrain& terrain) {
    float maxLen = glm::length(rayDirection);
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection);
    float curr_t = 0.f;

    while (curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1;

        for (int i = 0; i < 3; ++i) {
            if (rayDirection[i] != 0) {
                float offset = glm::max(0.f, glm::sign(rayDirection[i]));

                if (currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }

                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen);

                if (axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }

        if (interfaceAxis == -1) {
            throw std::out_of_range( "interfaceAxis was -1 after the for loop in gridMarch!");
        }

        infAxis = interfaceAxis;
        curr_t += min_t;
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset = glm::ivec3(0, 0, 0);
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        BlockType cellType;
        try {
            cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
        } catch (std::exception){
            continue;
        }

        if (cellType != EMPTY && !Chunk::isHPlane(cellType) && !Chunk::isCross2(cellType)
                && !Chunk::isCross4(cellType) && cellType != WATER && cellType != LAVA) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }

    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

void Entity::isInLiquid(Terrain& terrain) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
    bool acc = false;

    for (int x = 0; x <= 1; x++) {
        for (int z = 2; z <= 3; z++) {
            for (int y = 0; y <= 1; y++) {
                glm::vec3 p = glm::vec3(bottomLeftVertex.x + playerDimensions[x],
                                        bottomLeftVertex.y + y,
                                        bottomLeftVertex.z + playerDimensions[z]);

                if (terrain.getBlockAt(p) == WATER || terrain.getBlockAt(p) == LAVA) {
                    acc = acc || true;
                } else {
                    acc = acc || false;
                }
            }
        }
    }

    m_inputs.inLiquid = acc;
}

void Entity::isOnGround(Terrain& terrain) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0, 0.5f);
    bool acc = false;

    for (int x = 0; x <= 1; x++) {
        for (int z = 2; z <= 3; z++) {
            glm::vec3 p = glm::vec3(bottomLeftVertex.x + playerDimensions[x],
                                    bottomLeftVertex.y - 0.05f,
                                    bottomLeftVertex.z + playerDimensions[z]);

            if (terrain.getBlockAt(p) != EMPTY &&
                !Chunk::isHPlane(terrain.getBlockAt(p)) &&
                !Chunk::isCross2(terrain.getBlockAt(p)) &&
                !Chunk::isCross4(terrain.getBlockAt(p)) &&
                terrain.getBlockAt(p) != WATER &&
                terrain.getBlockAt(p) != LAVA) {
                acc = acc || true;
            } else {
                acc = acc || false;
            }
        }
    }

    m_inputs.onGround = acc;
}


void Entity::isUnderLiquid(Terrain& terrain) {
    glm::vec3 middleLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
    bool underWater = false;
    bool underLava = false;

    for (int x = 0; x <= 1; x++) {
        for (int z = 2; z <= 3; z++) {
            glm::vec3 p = glm::vec3(middleLeftVertex.x + playerDimensions[x],
                                    middleLeftVertex.y + 1.f,
                                    middleLeftVertex.z + playerDimensions[z]);

            if (terrain.getBlockAt(p) == WATER) {
                underWater = underWater || true;
            } else if (terrain.getBlockAt(p) == LAVA) {
                underLava = underLava || true;
            } else {
                underWater = underWater || false;
                underLava = underLava || false;
            }
        }
    }

    m_inputs.underWater = underWater;
    m_inputs.underLava = underLava;
}
