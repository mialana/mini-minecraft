#include "biome.h"
#include <iostream>

Biome::Biome()
{

}

float Biome::noise1D(float x) {
    return glm::fract(std::pow(x, 4) / 4.f);
}

float Biome::interpNoise(float x) {
    int intX = int(floor(x));
    float fractX = glm::fract(x);

    float v1 = noise1D(intX);
    float v2 = noise1D(intX + 1);
    return glm::mix(v1, v2, fractX);
}

float Biome::fbm(float x) {
    float total = 0;
    float persistence = 0.5f;
    int octaves = 8;
    float freq = 2.f;
    float amp = 0.5f;
    for(int i = 1; i <= octaves; i++) {
        total += interpNoise(x * freq) * amp;

        freq *= 2.f;
        amp *= persistence;
    }
    return total;
}

float Biome::noise1D(glm::vec2 p) {
    return glm::fract(sin(glm::dot(p, glm::vec2(127.1, 311.7))) * 43758.5453);
}

glm::vec2 Biome::noise2D(glm::vec2 p) {
    float x = glm::fract(43758.5453 * glm::sin(glm::dot(p, glm::vec2(127.1, 311.7))));
    float y = glm::fract(43758.5453 * glm::sin(glm::dot(p, glm::vec2(269.5, 183.3))));
    return glm::vec2(x, y);
}

float Biome::noise1D(glm::vec3 p) {
    return glm::fract(sin((glm::dot(p, glm::vec3(127.1,
                                  311.7,
                                  191.999)))) *
                 43758.5453);
}


glm::vec3 Biome::noise3D(glm::vec2 p) {
    float x = glm::fract(43758.5453 * glm::sin(glm::dot(p, glm::vec2(127.1, 311.7))));
    float y = glm::fract(43758.5453 * glm::sin(glm::dot(p, glm::vec2(269.5, 183.3))));
    float z = glm::fract(43758.5453 * glm::sin(glm::dot(p, glm::vec2(420.6, 631.2))));
    return glm::vec3(x, y, z);
}

float Biome::interpNoise(float x, float y) {
    int intX = int(floor(x));
    float fractX = glm::fract(x);
    int intY = int(floor(y));
    float fractY = glm::fract(y);

    float v1 = noise1D(glm::vec2(intX, intY));
    float v2 = noise1D(glm::vec2(intX + 1, intY));
    float v3 = noise1D(glm::vec2(intX, intY + 1));
    float v4 = noise1D(glm::vec2(intX + 1, intY + 1));

    // mix is a glsl fn that returns a lin interp btwn 2 vals based on some t s.t. 0<t<1
    float i1 = glm::mix(v1, v2, fractX);
    float i2 = glm::mix(v3, v4, fractX);
    return glm::mix(i1, i2, fractY);
}

float Biome::fbm(const glm::vec2 uv) {
    float total = 0;
    float persistence = 0.5f;
    int octaves = 8;
    float freq = 2.f;
    float amp = 0.5f;
    for(int i = 1; i <= octaves; i++) {
        total += interpNoise(uv.x * freq,
                               uv.y * freq) * amp;
        freq *= 2.f;
        amp *= persistence;
    }
    return total;
}

float Biome::worley(glm::vec2 uv) {
    uv *= 10; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::fract(uv);
    float minDist = 1.0; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = noise2D(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            glm::vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = glm::length(diff);
            minDist = glm::min(minDist, dist);
        }
    } return minDist;
}

glm::vec2 Biome::voronoi(glm::vec2 uv, int scale) {
    glm::vec2 offset = noise2D(uv / 217.f); // Get the Voronoi centerpoint for this cell
    offset *= scale;
    return glm::floor(uv + offset);
}

float Biome::surflet1(glm::vec2 P, glm::vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial
    float distX = abs(P.x - gridPoint.x);
    float distY = abs(P.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.f) + 15 * pow(distX, 4.f) - 10 * pow(distX, 3.f);
    float tY = 1 - 6 * pow(distY, 5.f) + 15 * pow(distY, 4.f) - 10 * pow(distY, 3.f);
    // Get the random vector for the grid point
    glm::vec2 gradient = 2.f * noise2D(gridPoint) - glm::vec2(1.f);
    // Get the vector from the grid point to P
    glm::vec2 diff = P - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}

float Biome::surflet2(glm::vec2 P, glm::vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial
    float distX = abs(P.x - gridPoint.x);
    float distY = abs(P.y - gridPoint.y);
    float tX = 1 - 3 * pow(distX, 4.f) + 14 * pow(distX, 3.f) - 12 * pow(distX, 4.f);
    float tY = 1 - 3 * pow(distY, 4.f) + 14 * pow(distY, 3.f) - 12 * pow(distY, 4.f);
    // Get the random vector for the grid point
    glm::vec2 gradient = 2.f * noise2D(gridPoint) - glm::vec2(1.f);
    // Get the vector from the grid point to P
    glm::vec2 diff = P - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}

float Biome::perlin1(glm::vec2 uv) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            surfletSum += surflet1(uv, glm::floor(uv) + glm::vec2(dx, dy));
        }
    } return surfletSum;
}

float Biome::perlin2(glm::vec2 uv) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            surfletSum += surflet2(uv, glm::floor(uv) + glm::vec2(dx, dy));
        }
    } return surfletSum;
}
float Biome::surflet3D(glm::vec3 P, glm::vec3 gridPoint) {
    glm::vec3 dist = glm::abs(P - gridPoint);
    glm::vec3 t = glm::vec3(1.f) - 6.f * glm::vec3(pow(dist.x, 5.f), pow(dist.y, 5.f), pow(dist.z, 5.f))
            + 15.f * glm::vec3(pow(dist.x, 4.f), pow(dist.y, 4.f), pow(dist.z, 4.f))
            - 10.f * glm::vec3(pow(dist.x, 3.f), pow(dist.y, 3.f), pow(dist.z, 3.f));
    glm::vec3 gradient = Biome::noise3D(glm::vec2(gridPoint)) * 2.f - glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 diff = P - gridPoint;
    float height = glm::dot(diff, gradient);
    return height * t.x * t.y * t.z;
}

float Biome::perlin3D(glm::vec3 p) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            for(int dz = 0; dz <= 1; ++dz) {
                surfletSum += surflet3D(p, glm::floor(p) + (glm::vec3(dx, dy, dz)));
            }
        }
    }
    return surfletSum;
}

float Biome::hills(glm::vec2 xz) {
    float h = 0;
    float freq = 200.f;
    float dF = 0.5;

    for (int i = 0; i < 4; ++i) {
        h += perlin1(xz / freq);
        freq *= dF;
    }

    float flatten = 2.f;
    float sharpen = 1.25f;
    float max = -0.25f;
    float min = -0.9f;
    if (h > max) {
        h -= max;
        h /= flatten;
        h += max;
    }

    if (h < min) {
        h -= min;
        h *= sharpen;
        h += min;
    } return floor(160.f + (h * 50.f));
}

float Biome::mountains(glm::vec2 xz) {
    float h = 0;
    float amp = 0.5;
    float freq = 175.f;

    for (int i = 0; i < 4; ++i) {
        float h1 = perlin1(xz / freq);
        h1 = 1. - abs(h1);
        h1 = pow(h1, 1.25);
        h += h1 * amp;

        amp *= 0.5;
        freq *= 0.5;
    } return floor(150.f + h * 100.f);
}

float Biome::forest(glm::vec2 xz) {
    float h = 0;

    float amp = 0.5;
    float freq = 90.f;

    for (int i = 0; i < 4; ++i) {
        h += amp * perlin1(xz / freq);
        freq *= 0.5;
        amp *= 0.5;
    } return floor(100.f + h * 20) - 20;
}

float Biome::islands(glm::vec2 xz) {
    float h = 0;

    float amp = 0.5;
    float freq = 200.f;

    for (int i = 0; i < 4; ++i) {
        h += amp * perlin1(xz / freq);
        freq *= 0.25;
        amp *= 0.25;
    }

    h = (h + 1.f) / 2.f; // remap to 0-1

    float bar = 0.25f;
    float flatten = 2.f;
    if (h < bar) {
        h -= bar;
        h /= flatten;
        h += bar;
    }
    return floor(128.f - h * 100);
}

float Biome::blendTerrain(glm::vec2 uv, float h1, float h2) {
    double p = perlin1(uv);
    float heightMix = glm::smoothstep(0.25, 0.75, p);

    // perform linear interpolation
    float height = ((1 - heightMix) * h1) + (heightMix * h2);
    return height;
}
