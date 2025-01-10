#include "player.h"
#include <QString>
#include <iostream>
#include <QJsonArray>
#include "../mygl.h"

Player::Player(OpenGLContext& context, Terrain& terrain, glm::vec3 pos)
    : Entity(context, terrain, pos),
      m_firstPersonCamera(context, terrain, pos + glm::vec3(0, 1.5f, 0)),
      m_secondPersonCamera(context, terrain, pos + glm::normalize(m_firstPersonCamera.m_forward) * 5.f),
      m_thirdPersonCamera(context, terrain,
                          pos +
                          glm::normalize(m_firstPersonCamera.m_up) * 2.f +
                          glm::normalize(m_firstPersonCamera.m_forward) * -5.f
                          ),
      m_activeCameraView(FIRST)
{
}

CameraViews Player::changeActiveCamera() {
    if (m_activeCameraView == FIRST) {
        this->calculateThirdPersonCameraRotation();
        m_activeCameraView = THIRD;
    } else if (m_activeCameraView == THIRD) {
        this->calculateSecondPersonCameraRotation();
        m_activeCameraView = SECOND;
    } else if (m_activeCameraView == SECOND){
        m_activeCameraView = FIRST;
    } else {
        std::cout << "Active Camera View variable set incorrectly" << std::endl;
    }
    return m_activeCameraView;
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
    glm::vec3 rayOrigin = m_firstPersonCamera.m_position;
    glm::vec3 rayDirection = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 outBlockHit = glm::ivec3();

    if (gridMarch(rayOrigin, rayDirection, &outDist, &outBlockHit, *terrain)) {

        BlockType blockType = terrain->getBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z);
        inventory.addItem(blockType);
        terrain->setBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z, EMPTY);
        Chunk* ch = terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get();
        ch->destroyVBOdata();
        ch->generateVBOData();
        ch->loadVBO();
        return blockType;
    }

    return EMPTY;
}

BlockType Player::placeBlock(Terrain* terrain, BlockType currBlockType) {
    glm::vec3 rayOrigin = m_firstPersonCamera.m_position;
    glm::vec3 rayDirection = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 outBlockHit = glm::ivec3();
    BlockType outType = EMPTY;

    if (gridMarch(rayOrigin, rayDirection, &outDist, &outBlockHit, *terrain, &outType)) {
        if (outType == CLOTH_1) {
            MyGL* g = static_cast<MyGL*>(mcr_context);
            if (g) {
                g->showRecipe();
            }
        }
        else {
            if (inventory.removeItem(currBlockType)) {
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
    }
    return EMPTY;
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_firstPersonCamera.setWidthHeight(w, h);
    m_thirdPersonCamera.setWidthHeight(w, h);
    m_secondPersonCamera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_firstPersonCamera.moveAlongVector(dir);
    m_thirdPersonCamera.moveAlongVector(dir);
    m_secondPersonCamera.moveAlongVector(dir);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += dir;
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_firstPersonCamera.moveForwardLocal(amount);
    m_thirdPersonCamera.moveForwardLocal(amount);
    m_secondPersonCamera.moveForwardLocal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += m_forward * amount;
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_firstPersonCamera.moveRightLocal(amount);
    m_thirdPersonCamera.moveRightLocal(amount);
    m_secondPersonCamera.moveRightLocal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += m_right * amount;
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_firstPersonCamera.moveUpLocal(amount);
    m_thirdPersonCamera.moveUpLocal(amount);
    m_secondPersonCamera.moveUpLocal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += m_up * amount;
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_firstPersonCamera.moveForwardGlobal(amount);
    m_thirdPersonCamera.moveForwardGlobal(amount);
    m_secondPersonCamera.moveForwardGlobal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += glm::vec3(0, 0, amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_firstPersonCamera.moveRightGlobal(amount);
    m_thirdPersonCamera.moveRightGlobal(amount);
    m_secondPersonCamera.moveRightGlobal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += glm::vec3(amount, 0, 0);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_firstPersonCamera.moveUpGlobal(amount);
    m_thirdPersonCamera.moveUpGlobal(amount);
    m_secondPersonCamera.moveUpGlobal(amount);
    (static_cast<TranslateNode*>(nodePointerMap["BodyT"]))->translation += glm::vec3(0, amount, 0);
}

void Player::calculateThirdPersonCameraRotation() {
    m_thirdPersonCamera.m_position = m_firstPersonCamera.m_position + glm::normalize(
                                         m_firstPersonCamera.m_up) * 2.f + glm::normalize(m_firstPersonCamera.m_forward) * -5.f;
    m_thirdPersonCamera.m_forward = glm::normalize(m_firstPersonCamera.m_position -
                                                   m_thirdPersonCamera.m_position);
    m_thirdPersonCamera.m_right = m_firstPersonCamera.m_right;
    m_thirdPersonCamera.m_up = -glm::cross(m_thirdPersonCamera.m_forward, m_thirdPersonCamera.m_right);
}
void Player::calculateSecondPersonCameraRotation() {
    m_secondPersonCamera.m_position = m_firstPersonCamera.m_position + glm::normalize(m_firstPersonCamera.m_forward) * 5.f;
    m_secondPersonCamera.m_forward = glm::normalize(m_firstPersonCamera.m_position - m_secondPersonCamera.m_position);
    m_secondPersonCamera.m_right = -m_firstPersonCamera.m_right;
    m_secondPersonCamera.m_up = -glm::cross(m_secondPersonCamera.m_forward, m_secondPersonCamera.m_right);
}

void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_firstPersonCamera.rotateOnForwardLocal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateSecondPersonCameraRotation();
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_firstPersonCamera.rotateOnRightLocal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateSecondPersonCameraRotation();
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_firstPersonCamera.rotateOnUpLocal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateSecondPersonCameraRotation();
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_firstPersonCamera.rotateOnForwardGlobal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateSecondPersonCameraRotation();
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_firstPersonCamera.rotateOnRightGlobal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateSecondPersonCameraRotation();
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_firstPersonCamera.rotateOnUpGlobal(degrees);
    this->calculateThirdPersonCameraRotation();
    this->calculateSecondPersonCameraRotation();
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
