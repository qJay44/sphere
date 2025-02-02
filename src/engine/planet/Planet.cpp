#include "Planet.hpp"

#include "TerrainFace.hpp"

static const vec3 directions[6]{
  {0.f,  1.f,  0.f }, // Up
  {0.f,  -1.f, 0.f }, // Down
  {-1.f, 0.f,  0.f }, // Left
  {1.f,  0.f,  0.f }, // Right
  {0.f,  0.f,  1.f }, // Front
  {0.f,  0.f,  -1.f}, // Rear
};

static const vec3 palette[6]{
  {0.004f, 0.745f, 0.996f},
  {1.f,    0.867f, 0.f   },
  {1.f,    0.49f,  0.f   },
  {1.f,    0.f,    0.427f},
  {0.678f, 1.f,    0.008f},
  {0.561f, 0.f,    1.f   },
};

Planet::Planet(u16 resolution, float radius) : resolution(resolution), radius(radius) {
  build();
}

Planet::~Planet() {
  delete[] terrainFaces;
}

const u16& Planet::getResolutino() const { return resolution; }
const float& Planet::getRadius() const { return radius; };

void Planet::add(const Texture& texture) {
  for (u8 i = 0; i < 6; i++)
    terrainFaces[i].add(texture);
}

void Planet::rebuild(u16 resolution, float radius) {
  this->resolution = resolution;
  this->radius = radius;
  delete[] terrainFaces;
  build();
}

void Planet::draw(const Camera& camera, const Shader& shader) const {
  for (u8 i = 0; i < 6; i++)
    terrainFaces[i].draw(camera, shader);
}

void Planet::build() {
  terrainFaces =  new TerrainFace[6] {
    TerrainFace(resolution, directions[0], palette[0], radius),
    TerrainFace(resolution, directions[1], palette[1], radius),
    TerrainFace(resolution, directions[2], palette[2], radius),
    TerrainFace(resolution, directions[3], palette[3], radius),
    TerrainFace(resolution, directions[4], palette[4], radius),
    TerrainFace(resolution, directions[5], palette[5], radius),
  };
}

