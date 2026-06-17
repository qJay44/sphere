#include "Earth.hpp"

#include "TerrainFace.hpp"
#include "TileManager.hpp"

static constexpr auto caps32k = TextureVirtual::Capabilities({
  128,
  {1024, 1024},
  {32768 / 1024, 16384 / 1024}
});

Earth::Earth(int resolution, int chunksPerSide, float radius)
  : resolution(resolution),
    chunksPerSide(chunksPerSide),
    radius(radius),
    tileManager(caps32k),
    atmosphere(radius * 1.55f)
{
  build();
  createTextures();

  tileManager.addTexture(&texVirt32kColors);
  tileManager.addTexture(&texVirt32kHeightmapLand);
  tileManager.addTexture(&texVirt32kNormalmapLand);
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

  for (int i = 0; i < 6; i++)
    terrainFaces[i].build(directions[i], chunksPerSide, resolution, radius);

  bakeOpticalDepth();
}

void Earth::createTextures() {
  texIndir32k = TextureVirtual::generateIndirection({}, caps32k);

  texVirt32kColors = TextureVirtual("res/tex/earth/32768x16384/colormap.tif", {
    .internalFormat = GL_SRGB8
  }, caps32k, &texIndir32k);

  texVirt32kHeightmapLand = TextureVirtual("res/tex/earth/32768x16384/heightmapLand.tif", {
    .internalFormat = GL_R16,
    .format = GL_RED
  }, caps32k, &texIndir32k);

  texVirt32kNormalmapLand = TextureVirtual("res/tex/earth/32768x16384/normalmapLand.tif", {
  }, caps32k, &texIndir32k);

  texBathymetry = Texture2D(
    "res/tex/earth/Bathymetry.png",
    {
      .internalFormat = GL_R8,
      .format         = GL_RED,
    }
  );

  texShoreSDF = Texture2D(
    "res/tex/earth/shoreSDF.png",
    {
      .internalFormat = GL_R8,
      .format         = GL_RED,
    }
  );

  texBorders = Texture2D(
    "res/tex/earth/borders.png",
    {
      .internalFormat = GL_R8,
      .format         = GL_RED,
    }
  );

  texNormalmapWave0 = Texture2D(
    "res/tex/earth/normalmapWave0.png",
    {
      .wrapS = GL_REPEAT,
      .wrapT = GL_REPEAT,
    }
  );

  texNormalmapWave1 = Texture2D(
    "res/tex/earth/normalmapWave1.png",
    {
      .wrapS = GL_REPEAT,
      .wrapT = GL_REPEAT,
    }
  );

  texNoise = Texture2D(
    "res/tex/earth/noise.png",
    {
      .internalFormat = GL_R8,
      .format = GL_RED,
      .wrapS = GL_REPEAT,
      .wrapT = GL_REPEAT,
    }
  );

  texBakedOpticalDepth = Texture2D(ivec2(256), {
    .internalFormat = GL_R32F,
  });
}

void Earth::update() {
  atmosphere.update();
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
  shader.setUniform1f("u_seaLevel", seaLevel);
  shader.setUniform1f("u_lightMultiplier", lightMultiplier);
  shader.setUniform1f("u_triplanarBlendSharpness", triplanarBlendSharpness);
  shader.setUniform1f("u_waterDeepFactor", waterDeepFactor);
  shader.setUniform1f("u_waterWaveFreq", waterWaveFreq);
  shader.setUniform1f("u_waterWaveResScale", waterWaveResScale);
  shader.setUniform1f("u_maskTerrainFaceColor", useTerrainFaceColors);
  shader.setUniform1f("u_maskTerrainFaceChunkColor", useTerrainFaceChunkColors);
  shader.setUniform1f("u_waterShoreFreq", waterShoreFreq);
  shader.setUniform1f("u_waterShoreScale", waterShoreScale);
  shader.setUniform1f("u_waterShoreNoiseFreq", waterShoreNoiseFreq);
  shader.setUniform1f("u_waterShoreNoiseScale", waterShoreNoiseScale);
  shader.setUniform1f("u_waterShoreNoiseStrength", waterShoreNoiseStrength);
  shader.setUniform1f("u_waterShoreMaskBlend", waterShoreMaskBlend);
  shader.setUniform1f("u_waterShoreWidth", waterShoreWidth);
  shader.setUniform1f("u_waterShoreEdgeBlend", waterShoreEdgeBlend);
  shader.setUniform2f("u_virtualDims", caps32k.virtualDims);
  shader.setUniform3f("u_bordersColor", bordersColor);
  shader.setUniform3f("u_waterShallowColor", waterShallowColor);
  shader.setUniform3f("u_waterDeepColor", waterDeepColor);

  texIndir32k.bind(0);
  texVirt32kColors.bind(1);
  texVirt32kHeightmapLand.bind(2);
  texVirt32kNormalmapLand.bind(3);
  texBathymetry.bind(4);
  texShoreSDF.bind(5);
  texBorders.bind(6);
  texNormalmapWave0.bind(7);
  texNormalmapWave1.bind(8);
  texNoise.bind(9);

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

  texBakedOpticalDepth.bind(2);
  Mesh::drawScreen(camera, shader);
  texBakedOpticalDepth.unbind();
}

