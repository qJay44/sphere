#include "Planet.hpp"

#include "TerrainFace.hpp"

constexpr vec3 directions[6]{
  {1.f,  0.f,  0.f }, // Right
  {-1.f, 0.f,  0.f }, // Left
  {0.f,  1.f,  0.f }, // Top
  {0.f,  -1.f, 0.f }, // Bottom
  {0.f,  0.f,  -1.f}, // Back
  {0.f,  0.f,  1.f }, // Front
};

constexpr vec3 palette[6]{
  {0.f,    0.992f, 1.f   },
  {1.f,    0.149f, 0.f   },
  {1.f,    0.251f, 0.988f},
  {0.f,    0.976f, 0.173f},
  {0.024f, 0.204f, 0.988f},
  {0.996f, 0.984f, 0.169f},
};

const Texture* Planet::normalheightmaps = nullptr;
const Texture* Planet::worldColors = nullptr;

const Texture* Planet::getTexNormalheightmap(const bool& id) { return normalheightmaps; }
const Texture* Planet::getTexWorld(const bool& id) { return worldColors; }

void Planet::addTexNormalheightmaps(const Texture* tex) { normalheightmaps = tex; }
void Planet::addTexWorldcolors(const Texture* tex)      { worldColors = tex; }

Planet::Planet(u32 resolution, u32 chunksPerFace, float radius)
  : resolution(resolution),
    radius(radius) {
  chunks = chunksPerFace;
  terrainFaces = new TerrainFace[6];
  heightmapScale = radius * 0.065f;

  build();
}

Planet::~Planet() {
  if (terrainFaces) {
    delete[] terrainFaces;
    terrainFaces = nullptr;
  }
}

const u32&   Planet::getResolution()     const { return resolution;     }
const float& Planet::getRadius()         const { return radius;         }
const float& Planet::getHeightmapScale() const { return heightmapScale; }

void Planet::setHeightmapScale(const float& n) { heightmapScale = n; }

void Planet::rebuild() {
  rebuild(resolution, radius);
};

void Planet::rebuild(int resolution, float radius) {
  this->resolution = resolution;
  this->radius = radius;
  build();
}

void Planet::draw(const Camera* camera, const Shader& shader) const {
  assert(Planet::normalheightmaps != nullptr);
  assert(Planet::worldColors != nullptr);

  static const GLint heightmapScaleUniLoc = shader.getUniformLoc("u_heightmapScale");
  static const GLint nhmsLoc = shader.getUniformLoc("u_normalheightmaps");
  static const GLint wcLoc = shader.getUniformLoc("u_worldColors");
  static const GLint lightMultLoc = shader.getUniformLoc("u_lightMultiplier");

  shader.setUniform1f(lightMultLoc, lightMultiplier);
  shader.setUniform1f(heightmapScaleUniLoc, heightmapScale);
  shader.setUniformTexture(nhmsLoc, Planet::normalheightmaps->getUnit());
  shader.setUniformTexture(wcLoc, Planet::worldColors->getUnit());

  Planet::normalheightmaps->bind();
  Planet::worldColors->bind();

  for (u8 i = 0; i < 6; i++)
    terrainFaces[i].draw(camera, shader);

  Planet::normalheightmaps->unbind();
  Planet::worldColors->unbind();
}

void Planet::build() {
  delete[] terrainFaces;
  terrainFaces = new TerrainFace[6] (
    TerrainFace(directions[0], this),
    TerrainFace(directions[1], this),
    TerrainFace(directions[2], this),
    TerrainFace(directions[3], this),
    TerrainFace(directions[4], this),
    TerrainFace(directions[5], this)
  );

  for (u32 i = 0; i < 6; i++)
    printf("TerrainFace #%i vertices: %i\n", i, terrainFaces[i].chunks.back().getVerticesSize());
}

