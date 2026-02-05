#include "Earth.hpp"

#include "TerrainFace.hpp"
#include "glm/exponential.hpp"

//#define USE_DEGUG_LOAD

constexpr vec3 directions[6] {
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
  #ifdef USE_DEGUG_LOAD

  texWorldColors = Texture(
    "res/tex/earth/faces/debug",
    {
      .uniformName    = "u_worldColors",
      .unit           = 0,
      .target         = GL_TEXTURE_CUBE_MAP,
      .internalFormat = GL_RGBA,
      .format         = GL_RGBA,
    }
  );

  #else

  texHeightmapsWater = Texture(
    "res/tex/earth/faces/bathymetry21600", // TODO: Check this image (noise around country borders)
    {
      .uniformName    = "u_heightmapsWater",
      .unit           = 0,
      .target         = GL_TEXTURE_CUBE_MAP,
      .internalFormat = GL_R16I,
      .format         = GL_RED_INTEGER,
      .type           = GL_SHORT,
      .minFilter      = GL_NEAREST,
      .magFilter      = GL_NEAREST,
      .genMipMap      = false,
    }
  );

  texDistanceFieldWater = Texture(
    "res/tex/earth/faces/distanceFieldWater21600",
    {
      .uniformName    = "u_distanceFieldsWater",
      .unit           = 1,
      .target         = GL_TEXTURE_CUBE_MAP,
      .internalFormat = GL_R16UI,
      .format         = GL_RED_INTEGER,
      .type           = GL_UNSIGNED_SHORT,
      .minFilter      = GL_NEAREST,
      .magFilter      = GL_NEAREST,
      .genMipMap      = false,
    }
  );

  texNormalheightmapsLand = Texture(
    "res/tex/earth/faces/normalheightmap21600_5",
    {
      .uniformName    = "u_normalheightmapsLand",
      .unit           = 2,
      .target         = GL_TEXTURE_CUBE_MAP,
      .internalFormat = GL_RGBA,
      .format         = GL_RGBA,
      .minFilter      = GL_LINEAR,
      .magFilter      = GL_LINEAR,
      .genMipMap      = false,
    }
  );

  texWorldColors = Texture(
    "res/tex/earth/faces/worldColors21600",
    {
      .uniformName    = "u_worldColors",
      .unit           = 3,
      .target         = GL_TEXTURE_CUBE_MAP,
      .minFilter      = GL_LINEAR,
      .magFilter      = GL_LINEAR,
      .genMipMap      = false,
    }
  );

  texBorders = Texture(
    "res/tex/earth/faces/borders21600",
    {
      .uniformName    = "u_borders",
      .unit           = 4,
      .target         = GL_TEXTURE_CUBE_MAP,
      .internalFormat = GL_RED,
      .format         = GL_RED,
      .minFilter      = GL_LINEAR,
      .magFilter      = GL_LINEAR,
      .genMipMap      = false,
    }
  );

  texNormalmapWave0 = Texture("res/tex/earth/normalmapWave0.png", {"u_normalmapWave0", 5});
  texNormalmapWave1 = Texture("res/tex/earth/normalmapWave1.png", {"u_normalmapWave1", 6});

  #endif
}

Earth::Earth(u32 resolution, u32 chunksPerFace, float radius, float atmosphereRadius, const Light* light)
  : resolution(resolution),
    chunks(chunksPerFace),
    radius(radius),
    atmosphereRadius(atmosphereRadius),
    light(light) {

  terrainFaces = new TerrainFace[6];
  updateScatteringCoefficients();

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

void Earth::updateScatteringCoefficients() {
  atmosphereScatteringCoefficients = glm::pow(400.f / light->getWaveLengths(), vec3(4.f)) * atmosphereScatteringStrength;
}

void Earth::rebuild() {
  rebuild(resolution, radius);
};

void Earth::rebuild(int resolution, float radius) {
  this->resolution = resolution;
  this->radius = radius;
  build();
}

void Earth::draw(const Camera* camera, const frustum::Frustum& frustum, Shader& shader) const {
  shader.setUniform1f("u_lightMultiplier", lightMultiplier);
  shader.setUniform1f("u_heightmapScale", heightmapScale);
  shader.setUniform1f("u_ambient", ambient);
  shader.setUniform1f("u_specularLight", specularLight);
  shader.setUniform1f("u_triplanarBlendSharpness", triplanarBlendSharpness);
  shader.setUniform1f("u_waterSpecularSmoothness", waterSpecularSmoothness);
  shader.setUniform1f("u_waterDeepFactor", waterDeepFactor);
  shader.setUniform1f("u_waterDeepEdgeStart", waterDeepEdgeStart);
  shader.setUniform1f("u_waterDeepEdgeEnd", waterDeepEdgeEnd);
  shader.setUniform1f("u_waterWaveFreq", waterWaveFreq);
  shader.setUniform1f("u_waterWaveResMult", waterWaveResMult);
  shader.setUniform1f("u_waterShoreWaveFreq", waterShoreWaveFreq);
  shader.setUniform1f("u_waterShoreWaveThresholdStart", waterShoreWaveThresholdStart);
  shader.setUniform1f("u_waterShoreWaveThresholdEnd", waterShoreWaveThresholdEnd);
  shader.setUniform1f("u_waterShoreWaveAmplitude", waterShoreWaveAmplitude);
  shader.setUniform1f("u_waterShoreWaveNoiseScale", waterShoreWaveNoiseScale);
  shader.setUniform1f("u_waterShoreWaveNoiseSpeed", waterShoreWaveNoiseSpeed);
  shader.setUniform1f("u_waterShoreWaveNoiseAmplitude", waterShoreWaveNoiseAmplitude);
  shader.setUniform3f("u_bordersColor", bordersColor);
  shader.setUniform3f("u_waterShallowColor", waterShallowColor);
  shader.setUniform3f("u_waterDeepColor", waterDeepColor);

  texHeightmapsWater.bind();
  texDistanceFieldWater.bind();
  texNormalheightmapsLand.bind();
  texWorldColors.bind();
  texBorders.bind();
  texNormalmapWave0.bind();
  texNormalmapWave1.bind();

  for (u8 i = 0; i < 6; i++)
    terrainFaces[i].draw(camera, shader, frustum, debug_terrainFaceColors[i]);

  texHeightmapsWater.unbind();
  texDistanceFieldWater.unbind();
  texNormalheightmapsLand.unbind();
  texWorldColors.unbind();
  texBorders.unbind();
  texNormalmapWave0.unbind();
  texNormalmapWave1.unbind();
}

void Earth::drawAtmosphere(const Camera* camera, Shader& shader) const {
  shader.setUniform3f("u_planetCenter", vec3(0.f));
  shader.setUniform3f("u_scatteringCoefficients", atmosphereScatteringCoefficients);
  shader.setUniform1i("u_scatteringPoints", atmosphereScatteringPoints);
  shader.setUniform1i("u_opticalDepthPoints", atmosphereOpticalDepthPoints);
  shader.setUniform1f("u_planetRadius", radius);
  shader.setUniform1f("u_atmosphereRadius", atmosphereRadius);
  shader.setUniform1f("u_densityFalloff", atmosphereDensityFalloff);
  shader.setUniformMatrix4f("u_camInv", camera->getProjViewInv());

  Mesh::screenDraw(camera, shader);
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
}

