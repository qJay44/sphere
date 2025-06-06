#pragma once

#include "../engine/Camera.hpp"
#include "../engine/mesh/texture/Texture.hpp"

class Earth {
public:
  Earth(u32 resolution, u32 chunksPerFace, float radius);
  ~Earth();

  static void loadTextures();

  const u32&   getResolution()     const;
  const float& getRadius()         const;
  const float& getHeightmapScale() const;

  void rebuild();
  void rebuild(int resolution, float radius);
  void draw(const Camera* camera, const Shader& shader) const;

private:
  friend struct gui;
  friend struct TerrainFace;

  static Texture* texNormalheightmapsLand;
  static Texture* texHeightmapsWater;
  static Texture* texWorldColors;
  static Texture* texBorders;
  static Texture* texNormalmapWave0;
  static Texture* texNormalmapWave1;

  struct TerrainFace* terrainFaces = nullptr;

  u32 resolution;
  u32 chunks;

  float radius;
  float heightmapScale = 2.f;
  float seaLevel = 0.f;
  float lightMultiplier = 3.f;
  float ambient = 0.2f;
  float specularLight = 0.5f;

  vec3 bordersColor = vec3(0.55f);
  vec3 waterShallowColor = vec3(0.f, 0.705f, 0.799f);
  vec3 waterDeepColor = vec3(0.f, 0.127f, 0.255f);
  float waterSpecularSmoothness = 0.325f;
  float waterDeepFactor = -0.325f;
  float waterWaveFreq = 0.01f;
  float waterWaveResMult = 25.f;

  bool printBuildInfo = false;

private:
  void build();
};

