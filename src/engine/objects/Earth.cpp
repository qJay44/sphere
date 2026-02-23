#include "Earth.hpp"

#include <vips/vips8>

#include "TerrainFace.hpp"
#include "TileManager.hpp"
#include "../gui/gui.hpp"

//#define USE_DEGUG_LOAD

constexpr int tileSlots = 128;
constexpr ivec2 tileSize{1024, 1024};
constexpr ivec2 virtualDims{32768 / tileSize.x, 16384 / tileSize.y};

Earth::Earth(int resolution, int chunksPerSide, float radius)
  : resolution(resolution),
    chunksPerSide(chunksPerSide),
    radius(radius),
    tileManager(tileSlots, tileSize, virtualDims),
    atmosphere(radius * 1.55f)
{
  build();
  createTextures();

  tileManager.addTexture(&texVirtColors);
  tileManager.addTexture(&texVirtHeightmapLand);
  tileManager.addTexture(&texVirtNormalmapLand);
}

const int&   Earth::getResolution()     const { return resolution;     }
const float& Earth::getRadius()         const { return radius;         }
const float& Earth::getHeightmapScale() const { return heightmapScale; }

void Earth::build() {
  constexpr vec3 directions[6] {
    {1.f,  0.f,  0.f},  // Right
    {-1.f, 0.f,  0.f},  // Left
    {0.f,  1.f,  0.f},  // Top
    {0.f,  -1.f, 0.f},  // Bottom
    {0.f,  0.f,  1.f},  // Back
    {0.f,  0.f,  -1.f}, // Front
  };

  constexpr vec3 colors[6] {
    {0.f,    0.992f, 1.f   },
    {1.f,    0.149f, 0.f   },
    {1.f,    0.251f, 0.988f},
    {0.f,    0.976f, 0.173f},
    {0.024f, 0.204f, 0.988f},
    {0.996f, 0.984f, 0.169f},
  };

  for (int i = 0; i < 6; i++) {
    terrainFaces[i].build(directions[i], chunksPerSide, resolution, radius);
    terrainFaces[i].color = colors[i];
  }

  bakeOpticalDepth();
}

void Earth::createTextures() {
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

  texVirtColors = TextureVirtual({
    "Colors",
    "res/tex/earth/32768x16384/colormap.jpg",
    {0, 1},
    tileSlots,
    tileSize,
    virtualDims,
  });

  texVirtHeightmapLand = TextureVirtual({
    "HeightmapLand",
    "res/tex/earth/32768x16384/heightmapLand.png",
    {2, 3},
    tileSlots,
    tileSize,
    virtualDims,
    GL_R16,
    GL_RED
  });

  texVirtNormalmapLand = TextureVirtual({
    "NormalmapLand",
    "res/tex/earth/32768x16384/normalmapLand.jpg",
    {4, 5},
    tileSlots,
    tileSize,
    virtualDims,
  });

  texBathymetry = Texture2D(
    "res/tex/earth/Bathymetry.png",
    {
      .uniformName    = "u_texBathymetry",
      .unit           = 6,
      .internalFormat = GL_R8,
      .format         = GL_RED,
      .minFilter      = GL_NEAREST,
      .magFilter      = GL_NEAREST,
      .wrapS          = GL_CLAMP_TO_EDGE,
      .wrapT          = GL_CLAMP_TO_EDGE,
      .genMipMap      = false,
    }
  );

  texLandSDF = Texture2D(
    "res/tex/earth/landSDF.png",
    {
      .uniformName    = "u_texLandSDF",
      .unit           = 7,
      .internalFormat = GL_R8,
      .format         = GL_RED,
      .minFilter      = GL_NEAREST,
      .magFilter      = GL_NEAREST,
      .wrapS          = GL_CLAMP_TO_EDGE,
      .wrapT          = GL_CLAMP_TO_EDGE,
      .genMipMap      = false,
    }
  );

  texBorders = Texture2D(
    "res/tex/earth/borders.png",
    {
      .uniformName    = "u_texBorders",
      .unit           = 8,
      .internalFormat = GL_R8,
      .format         = GL_RED,
      .minFilter      = GL_LINEAR,
      .magFilter      = GL_LINEAR,
      .wrapS          = GL_CLAMP_TO_EDGE,
      .wrapT          = GL_CLAMP_TO_EDGE,
      .genMipMap      = false,
    }
  );

  texNormalmapWave0 = Texture2D("res/tex/earth/normalmapWave0.png", {"u_texNormalmapWave0", 9});
  texNormalmapWave1 = Texture2D("res/tex/earth/normalmapWave1.png", {"u_texNormalmapWave1", 10});

  texBakedOpticalDepth = Texture2D(ivec2(256), {
    .uniformName    = "u_texBakedOpticalDepth",
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

void Earth::update(const Light& light) {
  atmosphere.update(light);
}

void Earth::bakeOpticalDepth() {
  static Shader shader("bakeOpticalDepth.comp");
  constexpr uvec2 localSize(16); // NOTE: Must match in the shader

  const uvec2 size{256, 256};
  const uvec2 numGroups = (size + localSize - 1u) / localSize;

  shader.use();
  shader.setUniform1f("u_planetRadius", radius);
  shader.setUniform1f("u_atmosphereRadius", atmosphere.radius);
  shader.setUniform1f("u_densityFalloff", atmosphere.densityFalloff);
  shader.setUniform1i("u_opticalDepthPoints", atmosphere.opticalDepthPoints);
  glBindImageTexture(0, texBakedOpticalDepth.getId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
  glDispatchCompute(numGroups.x, numGroups.y, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Earth::draw(const Camera* camera, const frustum::Frustum& frustum, Shader& shader) {
  shader.setUniform1f("u_div", tessDivs);
  shader.setUniform1f("u_heightmapScale", heightmapScale);
  shader.setUniform1f("u_radius", radius);
  shader.setUniform1f("u_ambient", ambient);
  shader.setUniform1f("u_specularStrength", specularStrength);
  shader.setUniform1f("u_seaLevel", seaLevel);
  shader.setUniform1f("u_lightMultiplier", lightMultiplier);
  shader.setUniform1f("u_lightDimScale", lightDimScale);
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
  shader.setUniform1f("u_maskTerrainFaceColor", useTerrainFaceColors);
  shader.setUniform1f("u_maskTerrainFaceChunkColor", useTerrainFaceChunkColors);
  shader.setUniform2f("u_virtualDims", virtualDims);
  shader.setUniform3f("u_bordersColor", bordersColor);
  shader.setUniform3f("u_waterShallowColor", waterShallowColor);
  shader.setUniform3f("u_waterDeepColor", waterDeepColor);

  shader.setUniform1f("u_temp", gui::_sliderf0);

  texVirtColors.bind();
  texVirtHeightmapLand.bind();
  texVirtNormalmapLand.bind();
  texBathymetry.bind();
  texLandSDF.bind();
  texBorders.bind();
  texNormalmapWave0.bind();
  texNormalmapWave1.bind();

  chunksDrawn = 0;

  if (useFrustum)
    for (const TerrainFace& tf : terrainFaces)
      chunksDrawn += tf.draw(camera, shader, frustum, tileManager);
  else
    for (const TerrainFace& tf : terrainFaces)
      tf.draw(camera, shader);

  tileManager.processRequests();
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

