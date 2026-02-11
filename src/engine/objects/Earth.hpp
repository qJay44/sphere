#pragma once

#include "../Camera.hpp"
#include "../texture/Texture.hpp"
#include "../frustum/Frustum.hpp"
#include "TerrainFace.hpp"
#include "PlanetAtmosphere.hpp"
#include "Light.hpp"

class Earth {
public:
  Earth(int resolution, int chunksPerSide, float radius);

  const int&   getResolution()       const;
  const float& getRadius()           const;
  const float& getAtmosphereRadius() const;
  const float& getHeightmapScale()   const;

  void loadTextures();
  void build();
  void update(const Light& light);
  void bakeOpticalDepth();
  void draw(const Camera* camera, const frustum::Frustum& frustum, Shader& shader) const;
  void drawAtmosphere(const Camera* camera, Shader& shader) const;

private:
  friend struct gui;

  int resolution;
  int chunksPerSide;
  float radius;

  Texture texHeightmapsWater;
  Texture texDistanceFieldWater;
  Texture texNormalheightmapsLand;
  Texture texWorldColors;
  Texture texBorders;
  Texture texNormalmapWave0;
  Texture texNormalmapWave1;
  Texture texBakedOpticalDepth;

  TerrainFace terrainFaces[6];

  float heightmapScale = 8.f;
  float lightMultiplier = 1.5f;
  float ambient = 0.2f;
  float specularLight = 2.5f;
  float triplanarBlendSharpness = 1.f;
  float lightDimScale = 2.5f;
  float tessDivs = 2.5f;

  vec3 bordersColor = vec3(0.55f);
  vec3 waterShallowColor{0.f, 0.705f, 0.799f};
  vec3 waterDeepColor{0.f, 0.127f, 0.255f};
  float waterSpecularSmoothness = 0.065f;
  float waterDeepFactor = 0.4f;
  float waterDeepEdgeStart = 0.7f;
  float waterDeepEdgeEnd = 0.8f;
  float waterWaveFreq = 0.01f;
  float waterWaveResMult = 0.1f;
  float waterShoreWaveFreq = 9500.f;
  float waterShoreWaveThresholdStart = 0.9995f;
  float waterShoreWaveThresholdEnd = 1.f;
  float waterShoreWaveAmplitude = 0.63f;
  float waterShoreWaveNoiseScale = 3.034f;
  float waterShoreWaveNoiseSpeed = 0.123f;
  float waterShoreWaveNoiseAmplitude = 6.843f;

  PlanetAtmosphere atmosphere;

  bool useFrustum = true;
  bool useTerrainFaceColors = false;
  bool useTerrainFaceChunkColors = false;
  bool printBuildInfo = false;
};

