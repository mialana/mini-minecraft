#pragma once
#include "glm_includes.h"

class Biome
{
public:
    Biome();

    static float noise1D(float x);
    static float noise1D(glm::vec2 p);
    static float noise1D(glm::vec3 p);
    static glm::vec2 noise2D(glm::vec2 p);
    static glm::vec3 noise3D(glm::vec2 p);
    static float interpNoise(float x, float y);
    static float interpNoise(float);

    static float fbm(float x);
    static float fbm(const glm::vec2 uv); // range 0 to 1
    static float worley(glm::vec2 uv); // range 0 to 1

    static float surflet1(glm::vec2 P, glm::vec2 gridPoint);
    static float surflet2(glm::vec2 P, glm::vec2 gridPoint);
    static float surflet3D(glm::vec3 P, glm::vec3 gridPoint);

    static float perlin1(glm::vec2 uv); // range -1 to 1
    static float perlin2(glm::vec2 uv);
    static float perlin3D(glm::vec3 p);

    static float hills(glm::vec2 p);
    static float mountains(glm::vec2 p);
    static float forest(glm::vec2 p);
    static float islands(glm::vec2 p);

    static float blendTerrain(glm::vec2 uv, float h1, float h2);
};
