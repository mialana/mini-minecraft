#include "mob.h"
#include "biome.h"
#include <iostream>


Mob::Mob(OpenGLContext* context)
    : Entity(context), needsRespawn(true)
{
    this->m_inputs.flightMode = false;
    this->m_inputs.isMoving = false;
    this->m_inputs.inThirdPerson = true;
}

bool Mob::isIdle() {
    float x_dist = this->m_position.x - this->m_inputs.playerPosition.x;
    float z_dist = this->m_position.z - this->m_inputs.playerPosition.z;

    return (x_dist < 25.f && z_dist < 25.f) ? false : true;
}

void Mob::respawn(Chunk* c) {
    // spawn 5 blocks above the viable block
    this->m_position = c->getRandomViableBlock() + glm::vec3(c->getWorldPos().x, 5, c->getWorldPos().y);
    this->rotateOnUpGlobal(Biome::getRandomIntInRange(0, 359));

    // associate this chunk to the mob
    this->currChunk = c;

    this->needsRespawn = false;

    glm::mat4 bodyRotateMatrix = glm::lookAt(glm::vec3(), glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z)), glm::vec3(0, 1, 0));
    (static_cast<RotateNode*>(nodePointerMap["BodyR"]))->overriddenTransformMatrix = glm::inverse(bodyRotateMatrix);

    if (m_inputs.isZombie) {
        glm::mat4 headRotateMatrix = glm::lookAt(m_position + glm::vec3(0.f, 1.65f, 0.f), m_position + glm::vec3(0.f, 1.65f, 0.f) + m_forward, glm::vec3(0, 1, 0));
        (static_cast<RotateNode*>(nodePointerMap["HeadR"]))->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
    } else if (m_inputs.isPig) {
        glm::mat4 headRotateMatrix = glm::lookAt(m_position + glm::vec3(0.f, 0.9f, 0.f) + m_forward * 0.55f, m_position + glm::vec3(0.f, 0.9f, 0.f) + m_forward * 0.55f + m_forward, glm::vec3(0, 1, 0));
        (static_cast<RotateNode*>(nodePointerMap["HeadR"]))->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
    }
}

void Mob::tick(float dT, Terrain& terrain) {
    if (!this->needsRespawn) {
        computePhysics(dT, terrain);
        animate(dT);
        isInLiquid(terrain);
        isUnderLiquid(terrain);
        pathFind();

        timeSinceLastPathRecompute += dT;
    }

    if (glm::distance(this->m_position, this->m_inputs.playerPosition) > 100.f) {
        this->needsRespawn = true;
    }
}

float computeGVal(glm::ivec2 _curr, glm::ivec2 _target) {
    int dx = abs(_curr[0] - _target[0]);
    int dz = abs(_curr[1] - _target[1]);

    return sqrt(dx * dx + dz * dz);
}

float computeHVal(glm::ivec2 _curr, glm::ivec2 _target) {
    // calculate diagonal distance for h-value

    int dx = abs(_curr[0] - _target[0]);
    int dz = abs(_curr[1] - _target[1]);

    return (dx + dz) + (sqrt(2) - 2) * std::min(dx, dz);
}

/* For now, mob will only path find within their current chunk.
 * This function converts to local chunk positions for A*, calls the algorithm, then converts back and sets DirectionOfTravel correctly.
 */
void Mob::computeAStar() {
    glm::ivec2 currXZPos = this->currChunk->computeLocalPos(this->m_position);

    glm::vec3 targetBlock = this->currChunk->getRandomViableBlock();
    glm::ivec2 targetXZPos = glm::ivec2(floor(targetBlock.x), floor(targetBlock.z));
//    // reset AStar
//    this->currAStar = AStar();

//    currAStar.start = Cell(currXZPos);
//    currAStar.end = Cell(targetXZPos);

//    currASt

//    currAStar.path.push_back()
    this->currAStar = AStar();
    int openList;

    std::vector<std::vector<bool>> closedList;

    while (!openList.empty()) {
        Cell* currCell = openList.front();
        for c in currCell.children:
            c.gVal = currCell.gVal + computeGVal(currCell->position, c->position);
            c.hVal = computeHVal(c->position, this->currAStar.end->position);

            c.fVal = c.gVal + c.hVal;

            if (not blocked and not on closed list) {
                if (not inOpenList or current f is greater than the one just found) {
                    grid[pos].parent = currCell;
                    grid[pos].gVal = c.gVal;
                    grid[pos].hVal = c.hVal;
                    grid[pos].fVal = c.fVal;
                }

            closedList.push_back(c);
    }
}

void Mob::pathFind() {
    if (m_inputs.isZombie && this->isIdle()) {
        directionOfTravel = this->m_inputs.playerPosition + glm::vec3(0, 0, 1) - this->m_position;
        directionOfTravel = glm::normalize(glm::vec3(directionOfTravel.x, 0, directionOfTravel.z));
    } else {
        if (timeSinceLastPathRecompute > 3.f) {
            timeSinceLastPathRecompute = 0.f;

            //
            this->handleAStar();

            if (Biome::getRandomIntInRange(0, 2) > 0 || (m_inputs.inLiquid)) {
                directionOfTravel = glm::vec3(Biome::getRandomIntInRange(-5, 5),
                                              Biome::getRandomIntInRange(-5, 5),
                                              Biome::getRandomIntInRange(-5, 5));
                directionOfTravel = glm::normalize(glm::vec3(directionOfTravel.x, 0, directionOfTravel.z));
            } else {
                directionOfTravel = glm::vec3();
                this->rotateOnUpGlobal(Biome::getRandomIntInRange(0, 359));
            }
        }
    }

    this->m_acceleration = glm::vec3();
    if (directionOfTravel != glm::vec3()) {
        // set acceleration values
        m_acceleration += directionOfTravel;

        if (m_inputs.inLiquid && (m_inputs.underLava || m_inputs.underWater)) {
            m_acceleration += glm::vec3(0.f, 5.f, 0.f);
        }

        if (m_inputs.collisionDetected) {
            m_acceleration += glm::vec3(0.f, 10.f, 0.f);
        }

        // set head position
        glm::mat4 bodyRotateMatrix = glm::lookAt(glm::vec3(), glm::normalize(directionOfTravel), glm::vec3(0, 1, 0));
        (static_cast<RotateNode*>(nodePointerMap["BodyR"]))->overriddenTransformMatrix = glm::inverse(bodyRotateMatrix);

        if (m_inputs.isZombie && glm::distance(this->m_position, this->m_inputs.playerPosition) < 25.f) {
            glm::mat4 headRotateMatrix = glm::lookAt(m_position + glm::vec3(0.f, 1.65f, 0.f), m_inputs.playerPosition + glm::vec3(0.f, 1.65f, 0.f), glm::vec3(0, 1, 0));
            (static_cast<RotateNode*>(nodePointerMap["HeadR"]))->overriddenTransformMatrix = glm::inverse(headRotateMatrix);

            m_acceleration *= 10.f;
        } else {
            if (m_inputs.isZombie) {
                glm::mat4 headRotateMatrix = glm::lookAt(m_position + glm::vec3(0.f, 1.65f, 0.f), m_position + glm::vec3(0.f, 1.65f, 0.f) + glm::normalize(directionOfTravel), glm::vec3(0, 1, 0));
                (static_cast<RotateNode*>(nodePointerMap["HeadR"]))->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
            } else if (m_inputs.isPig) {
                glm::mat4 headRotateMatrix = glm::lookAt(m_position + glm::vec3(0.f, 0.9f, 0.f) + glm::normalize(directionOfTravel) * 0.55f, m_position + glm::vec3(0.f, 0.9f, 0.f) + glm::normalize(directionOfTravel), glm::vec3(0, 1, 0));
                (static_cast<RotateNode*>(nodePointerMap["HeadR"]))->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
            }

            m_acceleration *= 3.f;
        }
    } else {
        glm::mat4 bodyRotateMatrix = glm::lookAt(glm::vec3(), glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z)), glm::vec3(0, 1, 0));
        (static_cast<RotateNode*>(nodePointerMap["BodyR"]))->overriddenTransformMatrix = glm::inverse(bodyRotateMatrix);

        if (m_inputs.isZombie) {
            glm::mat4 headRotateMatrix = glm::lookAt(m_position + glm::vec3(0.f, 1.65f, 0.f), m_position + glm::vec3(0.f, 1.65f, 0.f) + m_forward, glm::vec3(0, 1, 0));
            (static_cast<RotateNode*>(nodePointerMap["HeadR"]))->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
        } else if (m_inputs.isPig) {
            glm::mat4 headRotateMatrix = glm::lookAt(m_position + glm::vec3(0.f, 0.9f, 0.f) + m_forward * 0.55f, m_position + glm::vec3(0.f, 0.9f, 0.f) + m_forward * 0.55f + m_forward, glm::vec3(0, 1, 0));
            (static_cast<RotateNode*>(nodePointerMap["HeadR"]))->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
        }
    }
}
