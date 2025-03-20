#include "mob.h"
#include "biome.h"
#include <iostream>

Mob::Mob(OpenGLContext *context) : Entity(context), needsRespawn(true) {
  this->m_inputs.flightMode = false;
  this->m_inputs.isMoving = false;
  this->m_inputs.inThirdPerson = true;
}

void Mob::respawn(Chunk *c) {
  int randomViableBlock =
      Biome::getRandomIntInRange(0, c->viableSpawnBlocks.size() - 1);

  this->m_position = c->viableSpawnBlocks[randomViableBlock] +
                     glm::vec3(c->getWorldPos().x, 5, c->getWorldPos().y);
  this->rotateOnUpGlobal(Biome::getRandomIntInRange(0, 359));
  this->needsRespawn = false;

  glm::mat4 bodyRotateMatrix = glm::lookAt(
      glm::vec3(), glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z)),
      glm::vec3(0, 1, 0));
  (static_cast<RotateNode *>(nodePointerMap["BodyR"]))
      ->overriddenTransformMatrix = glm::inverse(bodyRotateMatrix);

  if (m_inputs.isZombie) {
    glm::mat4 headRotateMatrix =
        glm::lookAt(m_position + glm::vec3(0.f, 1.65f, 0.f),
                    m_position + glm::vec3(0.f, 1.65f, 0.f) + m_forward,
                    glm::vec3(0, 1, 0));
    (static_cast<RotateNode *>(nodePointerMap["HeadR"]))
        ->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
  } else if (m_inputs.isPig) {
    glm::mat4 headRotateMatrix = glm::lookAt(
        m_position + glm::vec3(0.f, 0.9f, 0.f) + m_forward * 0.55f,
        m_position + glm::vec3(0.f, 0.9f, 0.f) + m_forward * 0.55f + m_forward,
        glm::vec3(0, 1, 0));
    (static_cast<RotateNode *>(nodePointerMap["HeadR"]))
        ->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
  }
}

void Mob::tick(float dT, Terrain &terrain) {
  if (!this->needsRespawn) {
    computePhysics(dT, terrain);
    animate(dT);
    isInLiquid(terrain);
    isUnderLiquid(terrain);
    pathFind();
    drawPath(terrain);

    timeSinceLastPathRecompute += dT;
  }

  if (glm::distance(this->m_position, this->m_inputs.playerPosition) > 100.f) {
    this->needsRespawn = true;
  }
}

void Mob::pathFind() {
  m_acceleration = glm::vec3();
  if ((m_inputs.isZombie &&
       glm::distance(this->m_position, this->m_inputs.playerPosition) < 25.f)) {
    directionOfTravel =
        this->m_inputs.playerPosition + glm::vec3(0, 0, 1) - this->m_position;
    directionOfTravel =
        glm::normalize(glm::vec3(directionOfTravel.x, 0, directionOfTravel.z));
  } else {
    if (timeSinceLastPathRecompute > 3.f) {
      timeSinceLastPathRecompute = 0.f;
      if (Biome::getRandomIntInRange(0, 2) > 0 || (m_inputs.inLiquid)) {
        directionOfTravel = glm::vec3(Biome::getRandomIntInRange(-5, 5),
                                      Biome::getRandomIntInRange(-5, 5),
                                      Biome::getRandomIntInRange(-5, 5));
        directionOfTravel = glm::normalize(
            glm::vec3(directionOfTravel.x, 0, directionOfTravel.z));
      } else {
        directionOfTravel = glm::vec3();
        this->rotateOnUpGlobal(Biome::getRandomIntInRange(0, 359));
      }
    }
  }

  if (directionOfTravel != glm::vec3()) {
    m_acceleration += directionOfTravel;

    if (m_inputs.inLiquid && (m_inputs.underLava || m_inputs.underWater)) {
      m_acceleration += glm::vec3(0.f, 5.f, 0.f);
    }

    if (m_inputs.collisionDetected) {
      m_acceleration += glm::vec3(0.f, 10.f, 0.f);
    }

    glm::mat4 bodyRotateMatrix = glm::lookAt(
        glm::vec3(), glm::normalize(directionOfTravel), glm::vec3(0, 1, 0));
    (static_cast<RotateNode *>(nodePointerMap["BodyR"]))
        ->overriddenTransformMatrix = glm::inverse(bodyRotateMatrix);

    if (m_inputs.isZombie &&
        glm::distance(this->m_position, this->m_inputs.playerPosition) < 25.f) {
      glm::mat4 headRotateMatrix =
          glm::lookAt(m_position + glm::vec3(0.f, 1.65f, 0.f),
                      m_inputs.playerPosition + glm::vec3(0.f, 1.65f, 0.f),
                      glm::vec3(0, 1, 0));
      (static_cast<RotateNode *>(nodePointerMap["HeadR"]))
          ->overriddenTransformMatrix = glm::inverse(headRotateMatrix);

      m_acceleration *= 10.f;
    } else {
      if (m_inputs.isZombie) {
        glm::mat4 headRotateMatrix =
            glm::lookAt(m_position + glm::vec3(0.f, 1.65f, 0.f),
                        m_position + glm::vec3(0.f, 1.65f, 0.f) +
                            glm::normalize(directionOfTravel),
                        glm::vec3(0, 1, 0));
        (static_cast<RotateNode *>(nodePointerMap["HeadR"]))
            ->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
      } else if (m_inputs.isPig) {
        glm::mat4 headRotateMatrix =
            glm::lookAt(m_position + glm::vec3(0.f, 0.9f, 0.f) +
                            glm::normalize(directionOfTravel) * 0.55f,
                        m_position + glm::vec3(0.f, 0.9f, 0.f) +
                            glm::normalize(directionOfTravel),
                        glm::vec3(0, 1, 0));
        (static_cast<RotateNode *>(nodePointerMap["HeadR"]))
            ->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
      }

      m_acceleration *= 3.f;
    }
  } else {
    glm::mat4 bodyRotateMatrix = glm::lookAt(
        glm::vec3(), glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z)),
        glm::vec3(0, 1, 0));
    (static_cast<RotateNode *>(nodePointerMap["BodyR"]))
        ->overriddenTransformMatrix = glm::inverse(bodyRotateMatrix);

    if (m_inputs.isZombie) {
      glm::mat4 headRotateMatrix =
          glm::lookAt(m_position + glm::vec3(0.f, 1.65f, 0.f),
                      m_position + glm::vec3(0.f, 1.65f, 0.f) + m_forward,
                      glm::vec3(0, 1, 0));
      (static_cast<RotateNode *>(nodePointerMap["HeadR"]))
          ->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
    } else if (m_inputs.isPig) {
      glm::mat4 headRotateMatrix = glm::lookAt(
          m_position + glm::vec3(0.f, 0.9f, 0.f) + m_forward * 0.55f,
          m_position + glm::vec3(0.f, 0.9f, 0.f) + m_forward * 0.55f +
              m_forward,
          glm::vec3(0, 1, 0));
      (static_cast<RotateNode *>(nodePointerMap["HeadR"]))
          ->overriddenTransformMatrix = glm::inverse(headRotateMatrix);
    }
  }
}
