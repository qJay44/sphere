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

  static Texture* texHeightmapsWater;
  static Texture* texDistanceFieldWater;
  static Texture* texNormalheightmapsLand;
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
  float lightMultiplier = 1.5f;
  float ambient = 0.2f;
  float specularLight = 0.5f;
  float triplanarBlendSharpness = 1.f;

  vec3 bordersColor = vec3(0.55f);
  vec3 waterShallowColor = vec3(0.f, 0.705f, 0.799f);
  vec3 waterDeepColor = vec3(0.f, 0.127f, 0.255f);
  float waterSpecularSmoothness = 0.065f;
  float waterDeepFactor = 0.4f;
  float waterDeepEdgeStart = 0.7f;
  float waterDeepEdgeEnd = 0.8f;
  float waterWaveFreq = 0.01f;
  float waterWaveResMult = 0.1f;
  float waterShoreWaveScale = 53.f;
  float waterShoreWaveFreq = 0.01f;
  float waterShoreWaveThreshold = 7.4f;
  float waterShoreWaveAmplitudeScale = 0.4f;
  float waterShoreWaveSmoothnessStart = 0.34f;
  float waterShoreWaveSmoothnessEnd = 0.829f;

  bool printBuildInfo = false;

private:
  void build();
};

