#pragma once

#include "../engine/Camera.hpp"
#include "../engine/mesh/texture/Texture.hpp"

class Planet {
public:
  static const Texture* getTexNormalheightmap(const bool& id);
  static const Texture* getTexWorld(const bool& id);

  static void addTexNormalheightmaps(const Texture* tex);
  static void addTexWorldcolors(const Texture* tex);

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

  static const Texture* normalheightmaps;
  static const Texture* worldColors;

  u32 resolution;
  u32 chunks;
  float radius;
  float heightmapScale = 0.06f;
  struct TerrainFace* terrainFaces = nullptr;

  bool colorChunksInsteadOfFaces = true;

private:
  void build();
};

