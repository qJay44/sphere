#pragma once

#include "../engine/Camera.hpp"
#include "../engine/mesh/texture/Texture.hpp"
#include "../engine/mesh/Mesh.hpp"

class Earth {
public:
  Earth(u32 resolution, u32 chunksPerFace, float radius);
  ~Earth();

  static void loadTextures();

  const u32&   getResolution()     const;
  const float& getRadius()         const;
  const float& getHeightmapScale() const;
  const float& getSeaLevel()       const;

  void setHeightmapScale(const float& n);
  void setCountriesBorders(const Mesh<Vertex1>& mesh);

  void rebuild();
  void rebuild(int resolution, float radius);
  void draw(const Camera* camera, const Shader& shader) const;

private:
  friend struct gui;
  friend struct TerrainFace;

  static Texture* texNormalheightmaps;
  static Texture* texWorldColors;
  static Texture* texBorders;

  u32 resolution;
  u32 chunks;
  float radius;
  float heightmapScale = 1.f;
  float lightMultiplier = 3.f;
  struct TerrainFace* terrainFaces = nullptr;

  float borderDataScale = 2.1f;
  float borderHeightMultiplier = 1.4f;
  float borderThickness = 1.f;
  vec3 borderColor = vec3(0.55f);

  Mesh<Vertex1> countriesBorders;

  bool printBuildInfo = false;

private:
  void build();
};

