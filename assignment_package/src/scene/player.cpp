#include "player.h"
#include <QString>

Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos), m_velocity(0,0,0), m_acceleration(0,0,0),
      m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain),
      infAxis(-1), mcr_camera(m_camera)
{}

Player::~Player()
{}

void Player::tick(float dT, InputBundle &input) {
    processInputs(mcr_terrain, input);
    computePhysics(dT, mcr_terrain, input);
}

void Player::processInputs(const Terrain& terrain, InputBundle &inputs) {
    // TODO: Update the Player's velocity and acceleration based on the
    // state of the inputs.
    float acceleration = 10.0f;
    float gravMultiplier = 20.f;

    if (inputs.flightMode) { // flight mode on

        m_acceleration = glm::vec3();
        m_velocity = glm::vec3();
        if (inputs.wPressed) {
            m_acceleration += acceleration * glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
        }
        if (inputs.sPressed) {
            m_acceleration += -acceleration * glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
        }
        if (inputs.dPressed) {
            m_acceleration += acceleration * glm::normalize(glm::vec3(m_right.x, 0.0f, m_right.z));
        }
        if (inputs.aPressed) {
            m_acceleration += -acceleration * glm::normalize(glm::vec3(m_right.x, 0.0f, m_right.z));
        }
        if (inputs.ePressed) {
            m_acceleration += acceleration * glm::normalize(glm::vec3(0.0f, m_up.y, 0.0f));
        }
        if (inputs.qPressed) {
            m_acceleration +=  -acceleration * glm::normalize(glm::vec3(0.0f, m_up.y, 0.0f));
        }
    } else { // flight mode off
        isOnGround(terrain, inputs);
        m_acceleration = glm::vec3();
        m_velocity = glm::vec3();
        if (inputs.wPressed) {
            m_acceleration += acceleration * glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
        }
        if (inputs.sPressed) {
            m_acceleration += -acceleration * glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
        }
        if (inputs.dPressed) {
            m_acceleration += acceleration * glm::normalize(glm::vec3(m_right.x, 0.0f, m_right.z));
        }
        if (inputs.aPressed) {
            m_acceleration += -acceleration * glm::normalize(glm::vec3(m_right.x, 0.0f, m_right.z));
        }
        if (inputs.spacePressed) {
            if (inputs.onGround) {
                // jump on ground
                m_acceleration += acceleration * gravMultiplier * glm::normalize(glm::vec3(0.0f, m_up.y, 0.0f));
            }
        }
    }
}

void Player::computePhysics(float dT, const Terrain &terrain, InputBundle& inputs) {

    glm::vec3 rayDirection = glm::vec3();
    glm::vec3 gravity = glm::vec3(0.f, -9.8f, 0.f); // -9.8 for gravity * 10 for mass lol

    if (!inputs.flightMode) {

        isOnGround(terrain, inputs);

        if(!inputs.onGround)
        {
            m_acceleration += gravity;
            m_velocity += m_acceleration * dT;
        }
        else
        {
            m_velocity += m_acceleration * dT;
        }
        rayDirection = m_velocity;
        detectCollision(&rayDirection, terrain);
    }
    else
    {
        m_velocity += m_acceleration * dT;
        rayDirection = m_velocity;
    }
    this->moveAlongVector(rayDirection);
}

bool Player::isOnGround(const Terrain &terrain, InputBundle &input) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0, 0.5f);
    for (int x = 0; x <= 1; x++) {
        for (int z = 0; z <= 1; z++) {
            glm::vec3 p = glm::vec3(floor(bottomLeftVertex.x) + x, floor(bottomLeftVertex.y - 0.05f),
                          floor(bottomLeftVertex.z) + z);
            if (terrain.getBlockAt(p) != EMPTY
                    && terrain.getBlockAt(p) != WATER) {
                input.onGround = true;
            } else {
                input.onGround = false;
            }
        }
    }
    return input.onGround;
}

void Player::detectCollision(glm::vec3 *rayDirection, const Terrain &terrain) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
        glm::ivec3 outBlockHit = glm::ivec3();
        float outDist = 0.f;

        for (int x = 0; x <= 1; x++) {
            for (int z = 0; z <= 1; z++) {
                for (int y = 0; y <= 2; y++) {
                    glm::vec3 rayOrigin = bottomLeftVertex + glm::vec3(x, y, z);
                    if (gridMarch(rayOrigin, *rayDirection, terrain, &outDist, &outBlockHit)) {
                        float distance = glm::min(outDist - 0.005f, glm::length(this->m_position - glm::vec3(outBlockHit)));
                        *rayDirection = distance * glm::normalize(*rayDirection);
                        return;
                    }
                }
            }
        }
}

bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                                 const Terrain &terrain, float *out_dist,
                                 glm::ivec3 *out_blockHit) {
    float maxLen = glm::length(rayDirection); // farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // now all t values represent world dist

    float curr_t = 0.f;
    while (curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for (int i = 0; i < 3; ++i) { // Iterate over the three axes
            if (rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i]));

                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if (currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
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
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        // If the currCell contains something other than empty, return curr_t
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);

        // there's a block thts not water/lava, return yes for collision
        if (cellType != EMPTY && cellType != WATER) {
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

    if (gridMarch(rayOrigin, rayDirection, *terrain, &outDist, &outBlockHit)) {
        BlockType blockType = terrain->getBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z);
        terrain->setBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z, EMPTY);
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

    if (gridMarch(rayOrigin, rayDirection, *terrain, &outDist, &outBlockHit)) {
        if (infAxis == 2) {
            if (terrain->getBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z - glm::sign(rayDirection.z)) == EMPTY) {
                terrain->setBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z - glm::sign(rayDirection.z), currBlockType);
                return currBlockType;
            }
        } else if (infAxis == 1) {
            if (terrain->getBlockAt(outBlockHit.x, outBlockHit.y - glm::sign(rayDirection.y), outBlockHit.z) == EMPTY) {
                terrain->setBlockAt(outBlockHit.x, outBlockHit.y - glm::sign(rayDirection.y), outBlockHit.z, currBlockType);
                return currBlockType;
            }
        } else if (infAxis == 0) {
            if (terrain->getBlockAt(outBlockHit.x - glm::sign(rayDirection.x), outBlockHit.y, outBlockHit.z) == EMPTY) {
                terrain->setBlockAt(outBlockHit.x - glm::sign(rayDirection.x), outBlockHit.y, outBlockHit.z, currBlockType);
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
