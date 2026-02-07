#include "Earth.hpp"

#include "TerrainFace.hpp"

//#define USE_DEGUG_LOAD

constexpr vec3 terrainFaceColors[6] {
  {0.f,    0.992f, 1.f   },
  {1.f,    0.149f, 0.f   },
  {1.f,    0.251f, 0.988f},
  {0.f,    0.976f, 0.173f},
  {0.024f, 0.204f, 0.988f},
  {0.996f, 0.984f, 0.169f},
};

void Earth::loadTextures() {
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
      .minFilter      = GL_NEAREST, // NOTE: Required for isampler
      .magFilter      = GL_NEAREST, // NOTE: Required for isampler
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
      .minFilter      = GL_NEAREST, // NOTE: Required for usampler
      .magFilter      = GL_NEAREST, // NOTE: Required for usampler
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

Earth::Earth(int resolution, int chunksPerSide, float radius)
  : resolution(resolution),
    chunksPerSide(chunksPerSide),
    radius(radius),
    atmosphere(radius * 1.55f) {
  build();
}

const int&   Earth::getResolution()     const { return resolution;       }
const float& Earth::getRadius()         const { return radius;           }
const float& Earth::getHeightmapScale() const { return heightmapScale;   }

void Earth::rebuild() {
  rebuild(resolution, radius);
};

void Earth::rebuild(int resolution, float radius) {
  this->resolution = resolution;
  this->radius = radius;
  build();
}

void Earth::update(const Light& light) {
  texHeightmapsWater.update();
  texDistanceFieldWater.update();
  texNormalheightmapsLand.update();
  texWorldColors.update();
  texBorders.update();
  texNormalmapWave0.update();
  texNormalmapWave1.update();
  atmosphere.update(light);
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
  shader.setUniform1f("u_radius", radius);
  shader.setUniform1f("u_maskTerrainFaceColor", useTerrainFaceColors);
  shader.setUniform1f("u_maskTerrainFaceChunkColor", useTerrainFaceChunkColors);
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

  for (u8 i = 0; i < 6; i++) {
    shader.setUniform3f("u_terrainFaceColor", terrainFaceColors[i]);
    terrainFaces[i].draw(camera, shader, frustum);
  }

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
  shader.setUniform1f("u_planetRadius", radius);
  shader.setUniformMatrix4f("u_camInv", camera->getProjViewInv());
  atmosphere.setUniforms(shader);

  Mesh::screenDraw(camera, shader);
}

void Earth::build() {
  for (int i = 0; i < 6; i++)
    terrainFaces[i] = TerrainFace(i, chunksPerSide, resolution, radius);
}

