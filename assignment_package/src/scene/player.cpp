#include "player.h"
#include <QString>
#include <iostream>
#include <QJsonArray>
#include "../mygl.h"

Player::Player(glm::vec3 pos, const Terrain& terrain, OpenGLContext* context)
    : Entity(pos, context), m_velocity(0, 0, 0), m_acceleration(0, 0, 0),
      m_camera(pos + glm::vec3(0, 1.5f, 0), context),
      m_thirdPersonCamera(pos + glm::normalize(m_camera.m_up) * 2.f + glm::normalize(
                              m_camera.m_forward) * -5.f, context),
      m_frontViewCamera(pos + glm::normalize(m_camera.m_forward) * 5.f, context),
      mcr_terrain(terrain), infAxis(-1), mcr_camera(&m_camera) {}

Player::~Player() {}

void Player::changeCamera(InputBundle& inputs) {
    if (mcr_camera == &m_camera) {
        mcr_camera = &m_thirdPersonCamera;
        this->calculateThirdPersonCameraRotation();
        inputs.inThirdPerson = true;
    } else if (mcr_camera == &m_thirdPersonCamera) {
        mcr_camera = &m_frontViewCamera;
        this->calculateFrontViewCameraRotation();
        inputs.inThirdPerson = true;
    } else {
        mcr_camera = &m_camera;
        inputs.inThirdPerson = false;
    }

}


void Player::tick(float dT, InputBundle& inputs) {
    isOnGround(inputs);
    isInLiquid(inputs);
    isUnderLiquid(inputs);
    processInputs(inputs);
    computePhysics(dT, inputs);
    Entity::animate(dT, inputs);
}

void Player::processInputs(InputBundle& inputs) {
    // TODO: Update the Player's velocity and acceleration based on the
    // state of the inputs.
    m_acceleration = glm::vec3();

    if (inputs.flightMode) { // flight mode on
        if (inputs.wPressed) {
            m_acceleration += m_forward;
        }

        if (inputs.sPressed) {
            m_acceleration += -m_forward;
        }

        if (inputs.dPressed) {
            m_acceleration += m_right;
        }

        if (inputs.aPressed) {
            m_acceleration -= m_right;
        }

        if (inputs.ePressed) {
            m_acceleration += m_up;
        }

        if (inputs.qPressed) {
            m_acceleration -= m_up;
        }
    } else { // flight mode off
        if (inputs.wPressed) {
            m_acceleration += glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
        }

        if (inputs.sPressed) {
            m_acceleration += -glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
        }

        if (inputs.dPressed) {
            m_acceleration += glm::normalize(glm::vec3(m_right.x, 0.0f, m_right.z));
        }

        if (inputs.aPressed) {
            m_acceleration += -glm::normalize(glm::vec3(m_right.x, 0.0f, m_right.z));
        }

        if (inputs.spacePressed) {
            if (inputs.inLiquid) {
                m_acceleration += glm::vec3(0.f, 1.5f, 0.f);
            } else if (inputs.onGround) {
                m_acceleration += glm::vec3(0.f, 10.f, 0.f);
            }
        }
    }

    m_acceleration *= 20.f;
}

void Player::computePhysics(float dT, InputBundle& inputs) {
    glm::vec3 rayDirection = glm::vec3();

    if (!inputs.flightMode) {
        float friction = 0.7f;
        m_velocity.x *= friction;
        m_velocity.z *= friction;
        float airFriction = 0.95f;
        m_velocity.y *= airFriction;
        glm::vec3 gravity = glm::vec3(0.f, -12.f, 0.f);
        m_acceleration += gravity;

        if (inputs.inLiquid) {
            m_velocity *= 0.5f;
        }
    } else {
        float friction = 0.9f;
        m_velocity *= friction;
    }

    m_velocity += m_acceleration * (dT / 10.f);

    if (!inputs.flightMode) {
        detectCollision();
    }

    this->moveAlongVector(m_velocity);
    inputs.isMoving = (glm::abs(m_velocity.x / (dT / 1000.f)) >= 3.f) || (glm::abs(m_velocity.z / (dT / 1000.f)) >= 3.f);
}

void Player::isInLiquid(InputBundle& input) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
    bool acc = false;

    for (int x = 0; x <= 1; x++) {
        for (int z = 2; z <= 3; z++) {
            for (int y = 0; y <= 1; y++) {
                glm::vec3 p = glm::vec3(bottomLeftVertex.x + playerDimensions[x],
                                        bottomLeftVertex.y + y,
                                        bottomLeftVertex.z + playerDimensions[z]);

                if (mcr_terrain.getBlockAt(p) == WATER || mcr_terrain.getBlockAt(p) == LAVA) {
                    acc = acc || true;
                } else {
                    acc = acc || false;
                }
            }
        }
    }

    input.inLiquid = acc;
}

void Player::isUnderLiquid(InputBundle& input) {
    glm::vec3 middleLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
    bool underWater = false;
    bool underLava = false;

    for (int x = 0; x <= 1; x++) {
        for (int z = 2; z <= 3; z++) {
            glm::vec3 p = glm::vec3(middleLeftVertex.x + playerDimensions[x],
                                    middleLeftVertex.y + 1.f,
                                    middleLeftVertex.z + playerDimensions[z]);

            if (mcr_terrain.getBlockAt(p) == WATER) {
                underWater = underWater || true;
            } else if (mcr_terrain.getBlockAt(p) == LAVA) {
                underLava = underLava || true;
            } else {
                underWater = underWater || false;
                underLava = underLava || false;
            }
        }
    }

    input.underWater = underWater;
    input.underLava = underLava;
}

void Player::isOnGround(InputBundle& input) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0, 0.5f);
    bool acc = false;

    for (int x = 0; x <= 1; x++) {
        for (int z = 2; z <= 3; z++) {
            glm::vec3 p = glm::vec3(bottomLeftVertex.x + playerDimensions[x],
                                    bottomLeftVertex.y - 0.05f,
                                    bottomLeftVertex.z + playerDimensions[z]);

            if (mcr_terrain.getBlockAt(p) != EMPTY &&
                !Chunk::isHPlane(mcr_terrain.getBlockAt(p)) &&
                !Chunk::isCross2(mcr_terrain.getBlockAt(p)) &&
                !Chunk::isCross4(mcr_terrain.getBlockAt(p)) &&
                mcr_terrain.getBlockAt(p) != WATER &&
                mcr_terrain.getBlockAt(p) != LAVA) {
                acc = acc || true;
            } else {
                acc = acc || false;
            }
        }
    }

    input.onGround = acc;
}

void Player::detectCollision() {
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

                if (gridMarch(rayOrigin, rayDirectionX, &outDistX, &outBlockHitX) && (outDistX < minOutDistX)) {
                    minOutDistX = outDistX;
                }

                if (gridMarch(rayOrigin, rayDirectionY, &outDistY, &outBlockHitY) && (outDistY < minOutDistY)) {
                    minOutDistY = outDistY;
                }

                if (gridMarch(rayOrigin, rayDirectionZ, &outDistZ, &outBlockHitZ) && (outDistZ < minOutDistZ)) {
                    minOutDistZ = outDistZ;
                }
            }
        }
    }

    float threshold = 0.005f;

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
        } else {
            m_velocity.x = (minOutDistX * 0.8) * glm::sign(m_velocity.x);
        }
    }

    if (minOutDistZ < std::numeric_limits<float>::infinity()) {
        if (minOutDistZ < threshold) {
            m_velocity.z = 0.f;
        } else {
            m_velocity.z = (minOutDistZ * 0.8) * glm::sign(m_velocity.z);
        }
    }
}

bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, float* out_dist, glm::ivec3* out_blockHit) {
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
        BlockType cellType = mcr_terrain.getBlockAt(currCell.x, currCell.y, currCell.z);

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

BlockType Player::removeBlock(Terrain* terrain) {
    glm::vec3 rayOrigin = m_camera.m_position;
    glm::vec3 rayDirection = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 outBlockHit = glm::ivec3();

    if (gridMarch(rayOrigin, rayDirection, &outDist, &outBlockHit)) {
        std::cout<<"remove \n";
        BlockType blockType = terrain->getBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z);
        inventory.addItem(blockType);
        terrain->setBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z, EMPTY);
        Chunk* ch = terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get();
        ch->destroyVBOdata();
        ch->generateVBOData();
        ch->loadVBO();
        //terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
        //terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->generateVBOData();
        //terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->loadVBO();

        return blockType;
    }

    return EMPTY;
}

BlockType Player::placeBlock(Terrain* terrain, BlockType currBlockType) {
    glm::vec3 rayOrigin = m_camera.m_position;
    glm::vec3 rayDirection = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 outBlockHit = glm::ivec3();

    if (gridMarch(rayOrigin, rayDirection, &outDist, &outBlockHit)) {
        if (inventory.removeItem(currBlockType)) {
            std::cout << inventory.items[currBlockType].count << std::endl;
            if (infAxis == 2) {
                BlockType foundBlock = terrain->getBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z - glm::sign(rayDirection.z));
                if (foundBlock == EMPTY || foundBlock == WATER || foundBlock == LAVA) {
                    terrain->setBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z - glm::sign(rayDirection.z), currBlockType);
                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->generateVBOData();
                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->loadVBO();
                    return currBlockType;
                }
            } else if (infAxis == 1) {
                BlockType foundBlock = terrain->getBlockAt(outBlockHit.x, outBlockHit.y - glm::sign(rayDirection.y), outBlockHit.z);
                if (foundBlock == EMPTY || foundBlock == WATER || foundBlock == LAVA) {
                    terrain->setBlockAt(outBlockHit.x, outBlockHit.y - glm::sign(rayDirection.y), outBlockHit.z, currBlockType);
                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->generateVBOData();
                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->loadVBO();
                    return currBlockType;
                }
            } else if (infAxis == 0) {
                BlockType foundBlock = terrain->getBlockAt(outBlockHit.x - glm::sign(rayDirection.x), outBlockHit.y, outBlockHit.z);
                if (foundBlock == EMPTY || foundBlock == WATER || foundBlock == LAVA) {
                    terrain->setBlockAt(outBlockHit.x - glm::sign(rayDirection.x), outBlockHit.y, outBlockHit.z, currBlockType);
                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->generateVBOData();
                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->loadVBO();
                    return currBlockType;
                 }
            }
        }
    }

    return EMPTY;
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
    m_thirdPersonCamera.setWidthHeight(w, h);
    m_frontViewCamera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
    m_thirdPersonCamera.moveAlongVector(dir);
    m_frontViewCamera.moveAlongVector(dir);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += dir;
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
    m_thirdPersonCamera.moveForwardLocal(amount);
    m_frontViewCamera.moveForwardLocal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += m_forward * amount;
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
    m_thirdPersonCamera.moveRightLocal(amount);
    m_frontViewCamera.moveRightLocal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += m_right * amount;
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
    m_thirdPersonCamera.moveUpLocal(amount);
    m_frontViewCamera.moveUpLocal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += m_up * amount;
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
    m_thirdPersonCamera.moveForwardGlobal(amount);
    m_frontViewCamera.moveForwardGlobal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += glm::vec3(0, 0, amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
    m_thirdPersonCamera.moveRightGlobal(amount);
    m_frontViewCamera.moveRightGlobal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += glm::vec3(amount, 0, 0);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
    m_thirdPersonCamera.moveUpGlobal(amount);
    m_frontViewCamera.moveUpGlobal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += glm::vec3(0, amount, 0);
}

void Player::calculateThirdPersonCameraRotation() {
    m_thirdPersonCamera.m_position = m_camera.m_position + glm::normalize(
                                         m_camera.m_up) * 2.f + glm::normalize(m_camera.m_forward) * -5.f;
    m_thirdPersonCamera.m_forward = glm::normalize(m_camera.m_position -
                                                   m_thirdPersonCamera.m_position);
    m_thirdPersonCamera.m_right = m_camera.m_right;
    m_thirdPersonCamera.m_up = -glm::cross(m_thirdPersonCamera.m_forward, m_thirdPersonCamera.m_right);
}
void Player::calculateFrontViewCameraRotation() {
    m_frontViewCamera.m_position = m_camera.m_position + glm::normalize(m_camera.m_forward) * 5.f;
    m_frontViewCamera.m_forward = glm::normalize(m_camera.m_position - m_frontViewCamera.m_position);
    m_frontViewCamera.m_right = -m_camera.m_right;
    m_frontViewCamera.m_up = -glm::cross(m_frontViewCamera.m_forward, m_frontViewCamera.m_right);
}

void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateFrontViewCameraRotation();
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateFrontViewCameraRotation();
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateFrontViewCameraRotation();
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateFrontViewCameraRotation();
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateFrontViewCameraRotation();
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateFrontViewCameraRotation();
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " +
                    std::to_string(m_position.y) + ", " +
                    std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " +
                    std::to_string(m_velocity.y) + ", " +
                    std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " +
                    std::to_string(m_acceleration.y) + ", " +
                    std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " +
                    std::to_string(m_forward.y) + ", " +
                    std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}
