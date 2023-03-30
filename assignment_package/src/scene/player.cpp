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
    float acceleration = 20.0f;

    if (inputs.flightMode) { // flight mode on
        if (inputs.wPressed) {
            m_acceleration = 3 * acceleration * this->m_forward;
        } else if (inputs.sPressed) {
            m_acceleration = 3 * -acceleration * this->m_forward;
        } else if (inputs.dPressed) {
            m_acceleration = 3 * acceleration * this->m_right;
        } else if (inputs.aPressed) {
            m_acceleration = 3 * -acceleration * this->m_right;
        } else if (inputs.ePressed) {
            m_acceleration = 3 * acceleration * this->m_up;
        } else if (inputs.qPressed) {
            m_acceleration = 3 * -acceleration * this->m_up;
        } else {
            m_velocity = glm::vec3();
            m_acceleration = glm::vec3();
        }
    } else { // flight mode off
        isOnGround(terrain, inputs);
        if (inputs.wPressed) {
            m_acceleration = acceleration * glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z));
        } else if (inputs.sPressed) {
            m_acceleration = -acceleration * glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z));
        } else if (inputs.dPressed) {
            m_acceleration = acceleration * glm::normalize(glm::vec3(m_right.x, 0, m_right.z));
        } else if (inputs.aPressed) {
            m_acceleration = -acceleration * glm::normalize(glm::vec3(m_right.x, 0, m_right.z));
        } else if (inputs.spacePressed) {
            if (inputs.onGround) {
                // jump on ground
                m_velocity = acceleration * this->m_up;
            }
        } else {
            m_velocity = glm::vec3();
            m_acceleration = glm::vec3();
        }
    }
}

void Player::computePhysics(float dT, const Terrain &terrain, InputBundle& inputs) {
    // TODO: Update the Player's position based on its acceleration
    // and velocity, and also perform collision detection.
    m_velocity *= 0.95f; // reduce velocity for friction + drag
    m_velocity += m_acceleration * dT;
    glm::vec3 rayDirection = m_velocity * dT;
    glm::vec3 gravity = glm::vec3(0.f, -98.f, 0.f); // -9.8 for gravity * 10 for mass lol

    // handle collision & gravity while the player is in non-flightmode
    // if the player is not on ground, apply gravity
    if (!inputs.flightMode) {
        isOnGround(terrain, inputs);
        if (!inputs.onGround) {
            // falling
            m_acceleration = gravity;
            m_velocity += m_acceleration * dT;
        } else if (inputs.onGround && !inputs.spacePressed) {
            // come back down after jumping on ground
            m_velocity.y = 0.f;
        }
        rayDirection = m_velocity * dT;
        detectCollision(&rayDirection, terrain);
    }
    this->moveAlongVector(rayDirection);
}

bool Player::isOnGround(const Terrain &terrain, InputBundle &input) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0, 0.5f);
    for (int x = 0; x <= 1; x++) {
        for (int z = 0; z <= 1; z++) {
            glm::vec3 p = glm::vec3(floor(bottomLeftVertex.x) + x, floor(bottomLeftVertex.y - 0.005f),
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
        for (int z = 0; z >= -1; z--) {
            for (int y = 0; y <= 2; y++) {
                glm::vec3 rayOrigin = bottomLeftVertex + glm::vec3(x, y, z);
                if (gridMarch(rayOrigin, *rayDirection, terrain, &outDist, &outBlockHit)) {
                    float distance = glm::min(outDist - 0.005f, abs(glm::length(this->m_position - glm::vec3(outBlockHit))));
                    *rayDirection = distance * glm::normalize(*rayDirection);
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
