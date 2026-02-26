#pragma once

#include "../Camera.hpp"
#include "../texture/Texture2DArray.hpp"
#include "../texture/Texture2D.hpp"
#include "../frustum/Frustum.hpp"
#include "TerrainFace.hpp"
#include "PlanetAtmosphere.hpp"
#include "Light.hpp"
#include "TileManager.hpp"

class Earth {
public:
  Earth(int resolution, int chunksPerSide, float radius);

  const int&   getResolution()       const;
  const float& getRadius()           const;
  const float& getAtmosphereRadius() const;
  const float& getHeightmapScale()   const;

  void build();
  void createTextures();
  void update(const Light& light);
  void bakeOpticalDepth();
  void draw(const Camera* camera, const frustum::Frustum& frustum, Shader& shader);
  void drawAtmosphere(const Camera* camera, Shader& shader) const;

private:
  friend struct gui;

  static const fspath facesPath;

  int resolution;
  int chunksPerSide;
  float radius;
  TileManager tileManager;

  Texture2D texIndir32k;
  TextureVirtual texVirt32kColors;
  TextureVirtual texVirt32kHeightmapLand;
  TextureVirtual texVirt32kNormalmapLand;

  Texture2D texBathymetry;
  Texture2D texLandSDF;
  Texture2D texBorders;
  Texture2D texNormalmapWave0;
  Texture2D texNormalmapWave1;
  Texture2D texBakedOpticalDepth;

  TerrainFace terrainFaces[6];

  float heightmapScale = 8.f;
  float lightMultiplier = 1.5f;
  float ambient = 0.2f;
  float triplanarBlendSharpness = 1.f;
  float lightDimScale = 2.5f;
  float tessDivs = 2.5f;
  float seaLevel = 0.f;

  vec3 bordersColor = vec3(0.55f);
  vec3 waterShallowColor{0.f, 0.705f, 0.799f};
  vec3 waterDeepColor{0.f, 0.127f, 0.255f};
  float waterDeepFactor = 0.4f;
  float waterWaveFreq = 0.01f;
  float waterWaveResScale = 0.1f;

  PlanetAtmosphere atmosphere;

  int chunksDrawn = 0;

  bool useFrustum = true;
  bool useTerrainFaceColors = false;
  bool useTerrainFaceChunkColors = false;
  bool printBuildInfo = false;

private:
  static void tileLoad(const fspath& path, const Texture2DArray& tex);
};

