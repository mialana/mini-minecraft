#include "player.h"
#include <QString>
#include <iostream>

Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos), m_velocity(0,0,0), m_acceleration(0,0,0),
      m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain),
      infAxis(-1), mcr_camera(m_camera)
{}

Player::~Player()
{}

void Player::tick(float dT, InputBundle &input) {
    isOnGround(input);
    isInLiquid(input);
    isUnderLiquid(input);
    processInputs(input);
    computePhysics(dT, input);
}

void Player::processInputs(InputBundle &inputs) {
    // TODO: Update the Player's velocity and acceleration based on the
    // state of the inputs.
    float acceleration = 30.f;

    if (inputs.flightMode) { // flight mode on

        m_acceleration = glm::vec3();
        m_velocity = glm::vec3();
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
        if (inputs.ePressed) {
            m_acceleration += glm::normalize(glm::vec3(0.0f, m_up.y, 0.0f));
        }
        if (inputs.qPressed) {
            m_acceleration +=  -glm::normalize(glm::vec3(0.0f, m_up.y, 0.0f));
        }
    } else { // flight mode off
        m_acceleration = glm::vec3();
        m_velocity = glm::vec3();
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
                 m_velocity += glm::vec3(0.f, 10.f, 0.f);
            } else if (inputs.onGround) {
                // jump on ground
                m_velocity += glm::vec3(0.f, 6.f, 0.f);
            }
        }
    }
    m_acceleration *= acceleration;
}

void Player::computePhysics(float dT, InputBundle& inputs) {

    glm::vec3 rayDirection = glm::vec3();
    glm::vec3 gravity = glm::vec3(0.f, -40.f, 0.f); // -9.8 for gravity * 10 for mass lol

    if (!inputs.flightMode) {

        if(!inputs.onGround) {
            m_acceleration += gravity;
            m_velocity += m_acceleration * (dT / 10.f);
        } else {
            m_velocity += m_acceleration * (dT / 10.f);
        }
        rayDirection = m_velocity;
        if (inputs.inLiquid) {
            rayDirection *= 0.3f;
        }
        detectCollision(&rayDirection);
    }
    else
    {
        m_velocity += m_acceleration * (dT / 10.f);
        rayDirection = m_velocity;
    }
    this->moveAlongVector(rayDirection);
}

void Player::isInLiquid(InputBundle &input) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, -1.f, 0.5f);
    bool acc = false;
    for (int x = 0; x <= 1; x++) {
        for (int z = 2; z <= 3; z++) {
            for (int y = 0; y <= 1; y++) {
                glm::vec3 p = glm::vec3(bottomLeftVertex.x + playerDimensions[x], bottomLeftVertex.y + y,
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

void Player::isUnderLiquid(InputBundle &input) {
    glm::vec3 middleLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
    bool underWater = false;
    bool underLava = false;
    for (int x = 0; x <= 1; x++) {
        for (int z = 2; z <= 3; z++) {
            glm::vec3 p = glm::vec3(middleLeftVertex.x + playerDimensions[x], middleLeftVertex.y + 1.f,
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

void Player::isOnGround(InputBundle &input) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0, 0.5f);
    bool acc = false;
    for (int x = 0; x <= 1; x++) {
        for (int z = 2; z <= 3; z++) {
            glm::vec3 p = glm::vec3(bottomLeftVertex.x + playerDimensions[x], bottomLeftVertex.y - 0.05f,
                          bottomLeftVertex.z + playerDimensions[z]);
            if (mcr_terrain.getBlockAt(p) != EMPTY
                    && mcr_terrain.getBlockAt(p) != WATER
                     && mcr_terrain.getBlockAt(p) != LAVA) {
                acc = acc || true;
            } else {
                acc = acc || false;
            }
        }
    }
    input.onGround = acc;
}

void Player::detectCollision(glm::vec3 *rayDirection) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);

    glm::vec3 rayDirectionX = glm::vec3((*rayDirection).x, 0.f, 0.f);
    glm::vec3 rayDirectionY = glm::vec3(0.f, (*rayDirection).y, 0.f);
    glm::vec3 rayDirectionZ = glm::vec3(0.f, 0.f, (*rayDirection).z);

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
                glm::vec3 rayOrigin = bottomLeftVertex + glm::vec3(playerDimensions[x], playerDimensions[y], playerDimensions[z]);
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
            (*rayDirection).y = 0.f;

        } else {
            (*rayDirection).y = (minOutDistY * 0.8) * glm::sign((*rayDirection).y);
        }
    }

    if (minOutDistX < std::numeric_limits<float>::infinity()) {
        if (minOutDistX < threshold) {
            (*rayDirection).x = 0.f;
        } else {
            (*rayDirection).x = (minOutDistX * 0.8) * glm::sign((*rayDirection).x);
        }
    }

    if (minOutDistZ < std::numeric_limits<float>::infinity()) {
        if (minOutDistZ < threshold) {
            (*rayDirection).z = 0.f;
        } else {
            (*rayDirection).z = (minOutDistZ * 0.8) * glm::sign((*rayDirection).z);
        }
    }
}

bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                                 float *out_dist, glm::ivec3 *out_blockHit) {
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
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }
        infAxis = interfaceAxis;
        curr_t += min_t;
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset = glm::ivec3(0, 0, 0);
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        BlockType cellType = mcr_terrain.getBlockAt(currCell.x, currCell.y, currCell.z);

        if (cellType != EMPTY && cellType != WATER && cellType != LAVA) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }

    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

BlockType Player::removeBlock(Terrain* terrain) {
    glm::vec3 rayOrigin = m_camera.mcr_position;
    glm::vec3 rayDirection = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 outBlockHit = glm::ivec3();

    std::cout<<"removeing \n";

    if (gridMarch(rayOrigin, rayDirection, &outDist, &outBlockHit)) {
        std::cout<<"should remove \n";
        BlockType blockType = terrain->getBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z);
        terrain->setBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z, EMPTY);
        terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
        terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->generateVBOData();
        terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->loadVBO();
        return blockType;
    }
    return EMPTY;
}


// add a block on right click
BlockType Player::placeBlock(Terrain* terrain, BlockType currBlockType) {
    glm::vec3 rayOrigin = m_camera.mcr_position;
    glm::vec3 rayDirection = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 outBlockHit = glm::ivec3();

    if (gridMarch(rayOrigin, rayDirection, &outDist, &outBlockHit)) {
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
    return EMPTY;
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}
