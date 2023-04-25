#include "zombie.h"
#include "biome.h"
#include <iostream>

Zombie::Zombie(OpenGLContext* context)
    : Entity(context), needsRespawn(true)
{}

void Zombie::respawn(Chunk* c) {
    int randomViableBlock = Biome::getRandomIntInRange(0, c->viableSpawnBlocks.size() - 1);

    this->m_position = c->viableSpawnBlocks[randomViableBlock] + glm::vec3(c->getWorldPos().x, 1, c->getWorldPos().y);
    this->needsRespawn = false;

    std::cout << glm::to_string(this->m_position) << std::endl;
}

void Zombie::tick(float dT, InputBundle& inputs) {
    return;
}
