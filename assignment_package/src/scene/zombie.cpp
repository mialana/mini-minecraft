#include "zombie.h"

Zombie::Zombie(OpenGLContext* context)
    : Entity(context), needsRespawn(true)
{}

void Zombie::respawn(Chunk* c) {


    this->needsRespawn = false;
}
