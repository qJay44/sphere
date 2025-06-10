#include "Earth.hpp"

#include "TerrainFace.hpp"

constexpr vec3 directions[6]{
  {1.f,  0.f,  0.f},  // Right
  {-1.f, 0.f,  0.f},  // Left
  {0.f,  1.f,  0.f},  // Top
  {0.f,  -1.f, 0.f},  // Bottom
  {0.f,  0.f,  1.f},  // Front
  {0.f,  0.f,  -1.f}, // Back
};

constexpr vec3 debug_terrainFaceColors[6] {
  {0.f,    0.992f, 1.f   },
  {1.f,    0.149f, 0.f   },
  {1.f,    0.251f, 0.988f},
  {0.f,    0.976f, 0.173f},
  {0.024f, 0.204f, 0.988f},
  {0.996f, 0.984f, 0.169f},
};

Texture* Earth::texNormalheightmapsLand = nullptr;
Texture* Earth::texHeightmapsWater = nullptr;
Texture* Earth::texWorldColors = nullptr;
Texture* Earth::texBorders = nullptr;
Texture* Earth::texNormalmapWave0 = nullptr;
Texture* Earth::texNormalmapWave1 = nullptr;

void Earth::loadTextures() {
  size_t unit = 0;

  Earth::texHeightmapsWater = new Texture(
    "res/tex/earth/faces/water",
    "u_heightmapsWater",
    unit++,
    GL_TEXTURE_CUBE_MAP,
    GL_R16I,
    GL_RED_INTEGER,
    GL_SHORT
  );

  Earth::texNormalheightmapsLand = new Texture(
    "res/tex/earth/faces/land/normalheightmap21600_50",
    "u_normalheightmapsLand",
    unit++,
    GL_TEXTURE_CUBE_MAP
  );

  Earth::texWorldColors = new Texture(
    "res/tex/earth/faces/worldColors21600",
    "u_worldColors",
    unit++,
    GL_TEXTURE_CUBE_MAP
  );

  Earth::texBorders = new Texture(
    "res/tex/earth/faces/borders21600",
    "u_borders",
    unit++,
    GL_TEXTURE_CUBE_MAP
  );

  Earth::texNormalmapWave0 = new Texture("res/tex/earth/normalmapWave0.png", "u_normalmapWave0", unit++);
  Earth::texNormalmapWave1 = new Texture("res/tex/earth/normalmapWave1.png", "u_normalmapWave1", unit++);
}

Earth::Earth(u32 resolution, u32 chunksPerFace, float radius)
  : resolution(resolution),
    radius(radius) {

  chunks = chunksPerFace;
  terrainFaces = new TerrainFace[6];

  build();
}

Earth::~Earth() {
  if (terrainFaces) {
    delete[] terrainFaces;
    terrainFaces = nullptr;
  }

  delete texNormalheightmapsLand; texNormalheightmapsLand = nullptr;
  delete texHeightmapsWater; texHeightmapsWater = nullptr;
  delete texWorldColors; texWorldColors = nullptr;
  delete texBorders; texBorders = nullptr;
  delete texNormalmapWave0; texNormalmapWave0 = nullptr;
  delete texNormalmapWave1; texNormalmapWave1 = nullptr;
}

const u32&   Earth::getResolution()     const { return resolution;     }
const float& Earth::getRadius()         const { return radius;         }
const float& Earth::getHeightmapScale() const { return heightmapScale; }

void Earth::rebuild() {
  rebuild(resolution, radius);
};

void Earth::rebuild(int resolution, float radius) {
  this->resolution = resolution;
  this->radius = radius;
  build();
}

void Earth::draw(const Camera* camera, const Shader& shader) const {
  static const GLint heightmapsWaterLoc = shader.getUniformLoc(Earth::texHeightmapsWater->getUniformName());
  static const GLint nheightmapsLandLoc = shader.getUniformLoc(Earth::texNormalheightmapsLand->getUniformName());
  static const GLint worldColorsLoc     = shader.getUniformLoc(Earth::texWorldColors->getUniformName());
  static const GLint bordersLoc         = shader.getUniformLoc(Earth::texBorders->getUniformName());
  static const GLint normalmapWave0Loc  = shader.getUniformLoc(Earth::texNormalmapWave0->getUniformName());
  static const GLint normalmapWave1Loc  = shader.getUniformLoc(Earth::texNormalmapWave1->getUniformName());

  static const GLint heightmapScaleLoc      = shader.getUniformLoc("u_heightmapScale");
  static const GLint seaLevelLoc            = shader.getUniformLoc("u_seaLevel");
  static const GLint lightMultLoc           = shader.getUniformLoc("u_lightMultiplier");
  static const GLint ambientLoc             = shader.getUniformLoc("u_ambient");
  static const GLint specularLightLoc       = shader.getUniformLoc("u_specularLight");
  static const GLint tpBlendSharpnessLoc    = shader.getUniformLoc("u_triplanarBlendSharpness");
  static const GLint bordersColorLoc        = shader.getUniformLoc("u_bordersColor");
  static const GLint waterShallowColorLoc   = shader.getUniformLoc("u_waterShallowColor");
  static const GLint waterDeepColorLoc      = shader.getUniformLoc("u_waterDeepColor");
  static const GLint waterDeepFactorLoc     = shader.getUniformLoc("u_waterDeepFactor");
  static const GLint waterSpecSmoothnessLoc = shader.getUniformLoc("u_waterSpecularSmoothness");
  static const GLint waterWaveFreqLoc       = shader.getUniformLoc("u_waterWaveFreq");
  static const GLint waterWaveResMultLoc    = shader.getUniformLoc("u_waterWaveResMult");

  shader.setUniformTexture(heightmapsWaterLoc, Earth::texHeightmapsWater->getUnit());
  shader.setUniformTexture(nheightmapsLandLoc, Earth::texNormalheightmapsLand->getUnit());
  shader.setUniformTexture(worldColorsLoc,     Earth::texWorldColors->getUnit());
  shader.setUniformTexture(bordersLoc,         Earth::texBorders->getUnit());
  shader.setUniformTexture(normalmapWave0Loc,  Earth::texNormalmapWave0->getUnit());
  shader.setUniformTexture(normalmapWave1Loc,  Earth::texNormalmapWave1->getUnit());

  shader.setUniform1f(seaLevelLoc, seaLevel);
  shader.setUniform1f(lightMultLoc, lightMultiplier);
  shader.setUniform1f(heightmapScaleLoc, heightmapScale);
  shader.setUniform1f(ambientLoc, ambient);
  shader.setUniform1f(specularLightLoc, specularLight);
  shader.setUniform1f(tpBlendSharpnessLoc, triplanarBlendSharpness);
  shader.setUniform1f(waterSpecSmoothnessLoc, waterSpecularSmoothness);
  shader.setUniform1f(waterDeepFactorLoc, waterDeepFactor);
  shader.setUniform1f(waterWaveFreqLoc, waterWaveFreq);
  shader.setUniform1f(waterWaveResMultLoc, waterWaveResMult);
  shader.setUniform3f(bordersColorLoc, bordersColor);
  shader.setUniform3f(waterShallowColorLoc, waterShallowColor);
  shader.setUniform3f(waterDeepColorLoc, waterDeepColor);

  Earth::texNormalheightmapsLand->bind();
  Earth::texHeightmapsWater->bind();
  Earth::texWorldColors->bind();
  Earth::texBorders->bind();
  Earth::texNormalmapWave0->bind();
  Earth::texNormalmapWave1->bind();

  for (u8 i = 0; i < 6; i++)
    terrainFaces[i].draw(camera, shader, debug_terrainFaceColors[i]);

  Earth::texNormalheightmapsLand->unbind();
  Earth::texHeightmapsWater->unbind();
  Earth::texWorldColors->unbind();
  Earth::texBorders->unbind();
  Earth::texNormalmapWave0->unbind();
  Earth::texNormalmapWave1->unbind();
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

