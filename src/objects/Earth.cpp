#include "Earth.hpp"

#include "TerrainFace.hpp"

// #define USE_DEGUG_LOAD

constexpr vec3 directions[6]{
  {1.f,  0.f,  0.f},  // Right
  {-1.f, 0.f,  0.f},  // Left
  {0.f,  1.f,  0.f},  // Top
  {0.f,  -1.f, 0.f},  // Bottom
  {0.f,  0.f,  1.f},  // Back
  {0.f,  0.f,  -1.f}, // Front
};

constexpr vec3 debug_terrainFaceColors[6] {
  {0.f,    0.992f, 1.f   },
  {1.f,    0.149f, 0.f   },
  {1.f,    0.251f, 0.988f},
  {0.f,    0.976f, 0.173f},
  {0.024f, 0.204f, 0.988f},
  {0.996f, 0.984f, 0.169f},
};

void Earth::loadTextures(const Shader& shader) {
  size_t unit = 0;

  #ifdef USE_DEGUG_LOAD

  texWorldColors = Texture(
    "res/tex/earth/faces/debug",
    "u_worldColors",
    unit++,
    GL_TEXTURE_CUBE_MAP
  );

  #else

  texHeightmapsWater = Texture(
    "res/tex/earth/faces/bathymetry21600",
    "u_heightmapsWater",
    unit++,
    GL_TEXTURE_CUBE_MAP,
    GL_R16I,
    GL_RED_INTEGER,
    GL_SHORT
  );

  texDistanceFieldWater = Texture(
    "res/tex/earth/faces/distanceFieldWater21600",
    "u_distanceFieldsWater",
    unit++,
    GL_TEXTURE_CUBE_MAP,
    GL_R16UI,
    GL_RED_INTEGER,
    GL_UNSIGNED_SHORT
  );

  texNormalheightmapsLand = Texture(
    "res/tex/earth/faces/normalheightmap21600_5",
    "u_normalheightmapsLand",
    unit++,
    GL_TEXTURE_CUBE_MAP
  );

  texWorldColors = Texture(
    "res/tex/earth/faces/worldColors21600",
    "u_worldColors",
    unit++,
    GL_TEXTURE_CUBE_MAP
  );

  texBorders = Texture(
    "res/tex/earth/faces/borders21600",
    "u_borders",
    unit++,
    GL_TEXTURE_CUBE_MAP
  );

  texNormalmapWave0 = Texture("res/tex/earth/normalmapWave0.png", "u_normalmapWave0", unit++);
  texNormalmapWave1 = Texture("res/tex/earth/normalmapWave1.png", "u_normalmapWave1", unit++);

  shader.setUniformTexture(texHeightmapsWater);
  shader.setUniformTexture(texDistanceFieldWater);
  shader.setUniformTexture(texNormalheightmapsLand);
  shader.setUniformTexture(texWorldColors);
  shader.setUniformTexture(texBorders);
  shader.setUniformTexture(texNormalmapWave0);
  shader.setUniformTexture(texNormalmapWave1);

  #endif
}

Earth::Earth(u32 resolution, u32 chunksPerFace, float radius)
  : resolution(resolution),
    radius(radius),
    atmosphereRadius(radius * 1.5f) {

  chunks = chunksPerFace;
  terrainFaces = new TerrainFace[6];

  build();
}

Earth::~Earth() {
  if (terrainFaces) {
    delete[] terrainFaces;
    terrainFaces = nullptr;
  }
}

const u32&   Earth::getResolution()       const { return resolution;       }
const float& Earth::getRadius()           const { return radius;           }
const float& Earth::getAtmosphereRadius() const { return atmosphereRadius; }
const float& Earth::getHeightmapScale()   const { return heightmapScale;   }

void Earth::rebuild() {
  rebuild(resolution, radius);
};

void Earth::rebuild(int resolution, float radius) {
  this->resolution = resolution;
  this->radius = radius;
  build();
}

void Earth::draw(const Camera* camera, const Shader& shader) const {
  static const GLint heightmapScaleLoc               = shader.getUniformLoc("u_heightmapScale");
  static const GLint seaLevelLoc                     = shader.getUniformLoc("u_seaLevel");
  static const GLint lightMultLoc                    = shader.getUniformLoc("u_lightMultiplier");
  static const GLint ambientLoc                      = shader.getUniformLoc("u_ambient");
  static const GLint specularLightLoc                = shader.getUniformLoc("u_specularLight");
  static const GLint tpBlendSharpnessLoc             = shader.getUniformLoc("u_triplanarBlendSharpness");
  static const GLint bordersColorLoc                 = shader.getUniformLoc("u_bordersColor");
  static const GLint waterShallowColorLoc            = shader.getUniformLoc("u_waterShallowColor");
  static const GLint waterDeepColorLoc               = shader.getUniformLoc("u_waterDeepColor");
  static const GLint waterDeepFactorLoc              = shader.getUniformLoc("u_waterDeepFactor");
  static const GLint waterDeepEdgeStartLoc           = shader.getUniformLoc("u_waterDeepEdgeStart");
  static const GLint waterDeepEdgeEndLoc             = shader.getUniformLoc("u_waterDeepEdgeEnd");
  static const GLint waterSpecSmoothnessLoc          = shader.getUniformLoc("u_waterSpecularSmoothness");
  static const GLint waterWaveFreqLoc                = shader.getUniformLoc("u_waterWaveFreq");
  static const GLint waterWaveResMultLoc             = shader.getUniformLoc("u_waterWaveResMult");
  static const GLint waterShoreWaveFreqLoc           = shader.getUniformLoc("u_waterShoreWaveFreq");
  static const GLint waterShoreWaveThresholdStartLoc = shader.getUniformLoc("u_waterShoreWaveThresholdStart");
  static const GLint waterShoreWaveThresholdEndLoc   = shader.getUniformLoc("u_waterShoreWaveThresholdEnd");
  static const GLint waterShoreWaveAmplitudeLoc      = shader.getUniformLoc("u_waterShoreWaveAmplitude");
  static const GLint waterShoreWaveNoiseScaleLoc     = shader.getUniformLoc("u_waterShoreWaveNoiseScale");
  static const GLint waterShoreWaveNoiseSpeedLoc     = shader.getUniformLoc("u_waterShoreWaveNoiseSpeed");
  static const GLint waterShoreWaveNoiseAmplitudeLoc = shader.getUniformLoc("u_waterShoreWaveNoiseAmplitude");

  shader.setUniform1f(seaLevelLoc, seaLevel);
  shader.setUniform1f(lightMultLoc, lightMultiplier);
  shader.setUniform1f(heightmapScaleLoc, heightmapScale);
  shader.setUniform1f(ambientLoc, ambient);
  shader.setUniform1f(specularLightLoc, specularLight);
  shader.setUniform1f(tpBlendSharpnessLoc, triplanarBlendSharpness);
  shader.setUniform1f(waterSpecSmoothnessLoc, waterSpecularSmoothness);
  shader.setUniform1f(waterDeepFactorLoc, waterDeepFactor);
  shader.setUniform1f(waterDeepEdgeStartLoc, waterDeepEdgeStart);
  shader.setUniform1f(waterDeepEdgeEndLoc, waterDeepEdgeEnd);
  shader.setUniform1f(waterWaveFreqLoc, waterWaveFreq);
  shader.setUniform1f(waterWaveResMultLoc, waterWaveResMult);
  shader.setUniform1f(waterShoreWaveFreqLoc, waterShoreWaveFreq);
  shader.setUniform1f(waterShoreWaveThresholdStartLoc, waterShoreWaveThresholdStart);
  shader.setUniform1f(waterShoreWaveThresholdEndLoc, waterShoreWaveThresholdEnd);
  shader.setUniform1f(waterShoreWaveAmplitudeLoc, waterShoreWaveAmplitude);
  shader.setUniform1f(waterShoreWaveNoiseScaleLoc, waterShoreWaveNoiseScale);
  shader.setUniform1f(waterShoreWaveNoiseSpeedLoc, waterShoreWaveNoiseSpeed);
  shader.setUniform1f(waterShoreWaveNoiseAmplitudeLoc, waterShoreWaveNoiseAmplitude);
  shader.setUniform3f(bordersColorLoc, bordersColor);
  shader.setUniform3f(waterShallowColorLoc, waterShallowColor);
  shader.setUniform3f(waterDeepColorLoc, waterDeepColor);

  texHeightmapsWater.bind();
  texDistanceFieldWater.bind();
  texNormalheightmapsLand.bind();
  texWorldColors.bind();
  texBorders.bind();
  texNormalmapWave0.bind();
  texNormalmapWave1.bind();

  for (u8 i = 0; i < 6; i++)
    terrainFaces[i].draw(camera, shader, debug_terrainFaceColors[i]);

  texHeightmapsWater.unbind();
  texDistanceFieldWater.unbind();
  texNormalheightmapsLand.unbind();
  texWorldColors.unbind();
  texBorders.unbind();
  texNormalmapWave0.unbind();
  texNormalmapWave1.unbind();
}

void Earth::build() {
  delete[] terrainFaces;
  terrainFaces = new TerrainFace[6] (
    TerrainFace(directions[0], this),
    TerrainFace(directions[1], this),
    TerrainFace(directions[2], this),
    TerrainFace(directions[3], this),
    TerrainFace(directions[4], this),
    TerrainFace(directions[5], this)
  );

  if (printBuildInfo)
    for (u32 i = 0; i < 6; i++)
      printf("TerrainFace #%i vertices: %i\n", i, terrainFaces[i].chunks.back().getVerticesSize());
}

