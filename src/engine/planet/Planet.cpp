#include "Planet.hpp"

#include "TerrainFace.hpp"

Planet::Planet(u32 resolution) {
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

  for (int i = 0; i < 6; i++)
    terrainFaces[i] = TerrainFace(resolution, directions[i], palette[i]);
}

void Planet::add(const Texture& texture) {
  for (TerrainFace& tf : terrainFaces)
    tf.add(texture);
}

void Planet::draw(const Camera& camera, const Shader& shader) const {
  for (const TerrainFace& face : terrainFaces)
    face.draw(camera, shader);
}
