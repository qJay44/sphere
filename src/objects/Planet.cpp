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

const Texture* Planet::normalheightmaps[2];
const Texture* Planet::world[2];

const Texture* Planet::getTexNormalheightmap(const bool& id) { return normalheightmaps[id]; }
const Texture* Planet::getTexWorld(const bool& id) { return world[id]; }

void Planet::addTexNormalheightmaps(const Texture* tex0, const Texture* tex1) {
  normalheightmaps[0] = tex0;
  normalheightmaps[1] = tex1;
}

void Planet::addTexWorld(const Texture* tex0, const Texture* tex1) {
  world[0] = tex0;
  world[1] = tex1;
}

Planet::Planet(u32 resolution, u32 chunksPerFace, float radius)
  : resolution(resolution),
    radius(radius) {
  chunks = chunksPerFace;
  terrainFaces = new TerrainFace[6];

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
const float& Planet::getSeaLevel()       const { return seaLevel;       }

void Planet::setHeightmapScale(const float& n) { heightmapScale = n; }
void Planet::setSeaLevel(const float& n) { seaLevel = n; }

void Planet::rebuild() {
  rebuild(resolution, radius);
};

void Planet::rebuild(int resolution, float radius) {
  this->resolution = resolution;
  this->radius = radius;
  build();
}

void Planet::draw(const Camera* camera, const Shader& shader) const {
  assert(planet->normalheightmaps[0] != nullptr);
  assert(planet->normalheightmaps[1] != nullptr);
  assert(planet->world[0] != nullptr);
  assert(planet->world[1] != nullptr);

  static const GLint heightmapScaleUniLoc = shader.getUniformLoc("heightmapScale");
  static const GLint seaLevelUniLoc = shader.getUniformLoc("seaLevel");
  static const GLint nhms0Loc = shader.getUniformLoc("normalheightmap0");
  static const GLint nhms1Loc = shader.getUniformLoc("normalheightmap1");
  static const GLint world0Loc = shader.getUniformLoc("world0");
  static const GLint world1Loc = shader.getUniformLoc("world1");

  shader.setUniform1f(heightmapScaleUniLoc, heightmapScale * radius);
  shader.setUniform1f(seaLevelUniLoc, seaLevel);
  shader.setUniformTexture(nhms0Loc, 0);
  shader.setUniformTexture(nhms1Loc, 1);
  shader.setUniformTexture(world0Loc, 2);
  shader.setUniformTexture(world1Loc, 3);

  Planet::normalheightmaps[0]->bind();
  Planet::normalheightmaps[1]->bind();
  Planet::world[0]->bind();
  Planet::world[1]->bind();

  for (u8 i = 0; i < 6; i++)
    terrainFaces[i].draw(camera, shader);

  Planet::normalheightmaps[0]->unbind();
  Planet::normalheightmaps[1]->unbind();
  Planet::world[0]->unbind();
  Planet::world[1]->unbind();
}

void Planet::build() {
  const vec3 noColors[6] = {};
  const vec3* colors = colorChunksInsteadOfFaces ? noColors : palette;

  delete[] terrainFaces; terrainFaces = new TerrainFace[6] (
    TerrainFace(directions[0], this, colors[0]),
    TerrainFace(directions[1], this, colors[1]),
    TerrainFace(directions[2], this, colors[2]),
    TerrainFace(directions[3], this, colors[3]),
    TerrainFace(directions[4], this, colors[4]),
    TerrainFace(directions[5], this, colors[5])
  );
}

