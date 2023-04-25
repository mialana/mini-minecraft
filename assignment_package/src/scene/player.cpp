#include "player.h"
#include <QString>
#include <iostream>
#include <QJsonArray>
#include "../mygl.h"

Player::Player(glm::vec3 pos, const Terrain& terrain, OpenGLContext* context)
    : Entity(pos, context),
      m_camera(pos + glm::vec3(0, 1.5f, 0), context),
      m_thirdPersonCamera(pos + glm::normalize(m_camera.m_up) * 2.f + glm::normalize(
                              m_camera.m_forward) * -5.f, context),
      m_frontViewCamera(pos + glm::normalize(m_camera.m_forward) * 5.f, context),
      mcr_terrain(terrain), mcr_camera(&m_camera) {}

Player::~Player() {}

void Player::changeCamera() {
    if (mcr_camera == &m_camera) {
        mcr_camera = &m_thirdPersonCamera;
        this->calculateThirdPersonCameraRotation();
        m_inputs.inThirdPerson = true;
    } else if (mcr_camera == &m_thirdPersonCamera) {
        mcr_camera = &m_frontViewCamera;
        this->calculateFrontViewCameraRotation();
        m_inputs.inThirdPerson = true;
    } else {
        mcr_camera = &m_camera;
        m_inputs.inThirdPerson = false;
    }

}


void Player::tick(float dT, Terrain& terrain) {
    Entity::isOnGround(terrain);
    Entity::isInLiquid(terrain);
    Entity::isUnderLiquid(terrain);
    processInputs();
    Entity::computePhysics(dT, terrain);
    Entity::animate(dT);

    glm::mat4 bodyRotateMatrix = glm::lookAt(glm::vec3(), glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z)), glm::vec3(0, 1, 0));
    (static_cast<RotateNode*>(nodePointerMap["BodyR"]))->overriddenTransformMatrix = glm::inverse(bodyRotateMatrix);

    glm::mat4 headRotateMatrix = glm::lookAt(m_position + glm::vec3(0.f, 1.65f, 0.f), m_position + glm::vec3(0.f, 1.65f, 0.f) + m_forward, glm::vec3(0, 1, 0));
    (static_cast<RotateNode*>(nodePointerMap["HeadR"]))->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
}

void Player::processInputs() {
    // TODO: Update the Player's velocity and acceleration based on the
    // state of the inputs.
    m_acceleration = glm::vec3();

    if (m_inputs.flightMode) { // flight mode on
        if (m_inputs.wPressed) {
            m_acceleration += m_forward;
        }

        if (m_inputs.sPressed) {
            m_acceleration += -m_forward;
        }

        if (m_inputs.dPressed) {
            m_acceleration += m_right;
        }

        if (m_inputs.aPressed) {
            m_acceleration -= m_right;
        }

        if (m_inputs.ePressed) {
            m_acceleration += m_up;
        }

        if (m_inputs.qPressed) {
            m_acceleration -= m_up;
        }
    } else { // flight mode off
        if (m_inputs.wPressed) {
            m_acceleration += glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
        }

        if (m_inputs.sPressed) {
            m_acceleration += -glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
        }

        if (m_inputs.dPressed) {
            m_acceleration += glm::normalize(glm::vec3(m_right.x, 0.0f, m_right.z));
        }

        if (m_inputs.aPressed) {
            m_acceleration += -glm::normalize(glm::vec3(m_right.x, 0.0f, m_right.z));
        }

        if (m_inputs.spacePressed) {
            if (m_inputs.inLiquid) {
                m_acceleration += glm::vec3(0.f, 1.5f, 0.f);
            } else if (m_inputs.onGround) {
                m_acceleration += glm::vec3(0.f, 10.f, 0.f);
            }
        }
    }

    m_acceleration *= 20.f;
}

BlockType Player::removeBlock(Terrain* terrain) {
    glm::vec3 rayOrigin = m_camera.m_position;
    glm::vec3 rayDirection = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 outBlockHit = glm::ivec3();

//    if (gridMarch(rayOrigin, rayDirection, &outDist, &outBlockHit, *terrain)) {
//        std::cout<<"remove \n";
//        BlockType blockType = terrain->getBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z);
//        inventory.addItem(blockType);
//        terrain->setBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z, EMPTY);
//        Chunk* ch = terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get();
//        ch->destroyVBOdata();
//        ch->generateVBOData();
//        ch->loadVBO();
//        //terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
//        //terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->generateVBOData();
//        //terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->loadVBO();

//        return blockType;
//    }

    return EMPTY;
}

BlockType Player::placeBlock(Terrain* terrain, BlockType currBlockType) {
    glm::vec3 rayOrigin = m_camera.m_position;
    glm::vec3 rayDirection = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 outBlockHit = glm::ivec3();

//    if (gridMarch(rayOrigin, rayDirection, &outDist, &outBlockHit, *terrain)) {
//        if (inventory.removeItem(currBlockType)) {
//            std::cout << inventory.items[currBlockType].count << std::endl;
//            if (infAxis == 2) {
//                BlockType foundBlock = terrain->getBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z - glm::sign(rayDirection.z));
//                if (foundBlock == EMPTY || foundBlock == WATER || foundBlock == LAVA) {
//                    terrain->setBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z - glm::sign(rayDirection.z), currBlockType);
//                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
//                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->generateVBOData();
//                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->loadVBO();
//                    return currBlockType;
//                }
//            } else if (infAxis == 1) {
//                BlockType foundBlock = terrain->getBlockAt(outBlockHit.x, outBlockHit.y - glm::sign(rayDirection.y), outBlockHit.z);
//                if (foundBlock == EMPTY || foundBlock == WATER || foundBlock == LAVA) {
//                    terrain->setBlockAt(outBlockHit.x, outBlockHit.y - glm::sign(rayDirection.y), outBlockHit.z, currBlockType);
//                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
//                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->generateVBOData();
//                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->loadVBO();
//                    return currBlockType;
//                }
//            } else if (infAxis == 0) {
//                BlockType foundBlock = terrain->getBlockAt(outBlockHit.x - glm::sign(rayDirection.x), outBlockHit.y, outBlockHit.z);
//                if (foundBlock == EMPTY || foundBlock == WATER || foundBlock == LAVA) {
//                    terrain->setBlockAt(outBlockHit.x - glm::sign(rayDirection.x), outBlockHit.y, outBlockHit.z, currBlockType);
//                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
//                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->generateVBOData();
//                    terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->loadVBO();
//                    return currBlockType;
//                 }
//            }
//        }
//    }

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
