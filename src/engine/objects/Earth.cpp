#include "Earth.hpp"

#include "TerrainFace.hpp"

//#define USE_DEGUG_LOAD

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
      .wrapS          = GL_CLAMP_TO_EDGE,
      .wrapT          = GL_CLAMP_TO_EDGE,
      .wrapR          = GL_CLAMP_TO_EDGE,
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
      .wrapS          = GL_CLAMP_TO_EDGE,
      .wrapT          = GL_CLAMP_TO_EDGE,
      .wrapR          = GL_CLAMP_TO_EDGE,
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
      .wrapS          = GL_CLAMP_TO_EDGE,
      .wrapT          = GL_CLAMP_TO_EDGE,
      .wrapR          = GL_CLAMP_TO_EDGE,
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
      .wrapS          = GL_CLAMP_TO_EDGE,
      .wrapT          = GL_CLAMP_TO_EDGE,
      .wrapR          = GL_CLAMP_TO_EDGE,
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
      .wrapS          = GL_CLAMP_TO_EDGE,
      .wrapT          = GL_CLAMP_TO_EDGE,
      .wrapR          = GL_CLAMP_TO_EDGE,
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
      .wrapS          = GL_CLAMP_TO_EDGE,
      .wrapT          = GL_CLAMP_TO_EDGE,
      .wrapR          = GL_CLAMP_TO_EDGE,
      .genMipMap      = false,
    }
  );

  texNormalmapWave0 = Texture("res/tex/earth/normalmapWave0.png", {"u_normalmapWave0", 5});
  texNormalmapWave1 = Texture("res/tex/earth/normalmapWave1.png", {"u_normalmapWave1", 6});

  texBakedOpticalDepth = Texture(ivec2(256), {
    .uniformName    = "u_bakedOpticalDepth",
    .unit           = 2,
    .internalFormat = GL_R32F,
    .minFilter      = GL_LINEAR,
    .magFilter      = GL_LINEAR,
    .wrapS          = GL_CLAMP_TO_EDGE,
    .wrapT          = GL_CLAMP_TO_EDGE,
    .genMipMap      = false
  });

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

void Earth::update(const Light& light) {
  texHeightmapsWater.update();
  texDistanceFieldWater.update();
  texNormalheightmapsLand.update();
  texWorldColors.update();
  texBorders.update();
  texNormalmapWave0.update();
  texNormalmapWave1.update();
  texBakedOpticalDepth.update();

  atmosphere.update(light);
}

void Earth::build() {
  for (int i = 0; i < 6; i++)
    terrainFaces[i] = TerrainFace(i, chunksPerSide, resolution, radius);
  bakeOpticalDepth();
}

void Earth::bakeOpticalDepth() {
  static Shader shader("bakeOpticalDepth.comp");
  constexpr uvec2 localSize(16); // NOTE: Must match in the shader

  const uvec2 size{256, 256};
  const uvec2 numGroups = (size + localSize - 1u) / localSize;

  shader.use();
  shader.setUniform1f("u_planetRadius", radius);
  shader.setUniform1f("u_atmosphereRadius", atmosphere.radius);
  shader.setUniform1f("u_densityFalloffR", atmosphere.densityFalloffR);
  shader.setUniform1f("u_densityFalloffM", atmosphere.densityFalloffM);
  shader.setUniform1i("u_opticalDepthPoints", atmosphere.opticalDepthPoints);
  glBindImageTexture(0, texBakedOpticalDepth.getId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
  glDispatchCompute(numGroups.x, numGroups.y, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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

  for (const TerrainFace& tf : terrainFaces)
    tf.draw(camera, shader, frustum);

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

  texBakedOpticalDepth.bind();
  Mesh::screenDraw(camera, shader);
  texBakedOpticalDepth.unbind();
}

