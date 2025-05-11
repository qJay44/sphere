#pragma once

#include "../engine/Camera.hpp"
#include "../engine/mesh/texture/Texture.hpp"

class Planet {
public:
  static const Texture* getTexNormalheightmap(const bool& id);
  static const Texture* getTexWorld(const bool& id);

  static void addTexNormalheightmaps(const Texture* tex0, const Texture* tex1);
  static void addTexWorld(const Texture* tex0, const Texture* tex1);

  Planet(u32 resolution, u32 chunksPerFace, float radius);
  ~Planet();

  const u32&   getResolution()     const;
  const float& getRadius()         const;
  const float& getHeightmapScale() const;
  const float& getSeaLevel()       const;

  void setHeightmapScale(const float& n);
  void setSeaLevel(const float& n);

  void rebuild();
  void rebuild(int resolution, float radius);
  void draw(const Camera* camera, const Shader& shader) const;

private:
  friend struct gui;
  friend struct TerrainFace;

  static const Texture* normalheightmaps[2];
  static const Texture* world[2];

  u32 resolution;
  u32 chunks;
  float radius;
  float heightmapScale = 0.225f;
  float seaLevel = 0.f;
  struct TerrainFace* terrainFaces = nullptr;

  bool colorChunksInsteadOfFaces = true;

private:
  void build();
};

