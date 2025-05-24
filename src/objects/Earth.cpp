#include "Earth.hpp"

#include "TerrainFace.hpp"

constexpr vec3 directions[6]{
  {1.f,  0.f,  0.f }, // Right
  {-1.f, 0.f,  0.f }, // Left
  {0.f,  1.f,  0.f }, // Top
  {0.f,  -1.f, 0.f }, // Bottom
  {0.f,  0.f,  -1.f}, // Back
  {0.f,  0.f,  1.f }, // Front
};

constexpr vec3 palette[6]{
  {0.f,    0.992f, 1.f   },
  {1.f,    0.149f, 0.f   },
  {1.f,    0.251f, 0.988f},
  {0.f,    0.976f, 0.173f},
  {0.024f, 0.204f, 0.988f},
  {0.996f, 0.984f, 0.169f},
};

Texture* Earth::texNormalheightmaps = nullptr;
Texture* Earth::texWorldColors = nullptr;
Texture* Earth::texBorders = nullptr;

void Earth::loadTextures() {
  Earth::texNormalheightmaps = new Texture(
    "res/tex/earth/normalheightmap21600_50_0.png",
    "res/tex/earth/normalheightmap21600_50_1.png",
    "u_normalheightmaps",
    0,
    GL_TEXTURE_2D_ARRAY,
    GL_RGBA8,
    GL_RGBA,
    GL_UNSIGNED_BYTE
  );

  Earth::texWorldColors = new Texture(
    "res/tex/earth/worldColors0.jpg",
    "res/tex/earth/worldColors1.jpg",
    "u_worldColors",
    1,
    GL_TEXTURE_2D_ARRAY,
    GL_RGB8,
    GL_RGB,
    GL_UNSIGNED_BYTE
  );

  Earth::texBorders = new Texture(
    "res/tex/earth/borders21600_0.png",
    "res/tex/earth/borders21600_1.png",
    "u_borders",
    2,
    GL_TEXTURE_2D_ARRAY,
    GL_R8,
    GL_RED,
    GL_UNSIGNED_BYTE
  );
}

Earth::Earth(u32 resolution, u32 chunksPerFace, float radius)
  : resolution(resolution),
    radius(radius) {

  chunks = chunksPerFace;
  terrainFaces = new TerrainFace[6];
  heightmapScale = radius * 0.065f;

  build();
}

Earth::~Earth() {
  if (terrainFaces) {
    delete[] terrainFaces;
    terrainFaces = nullptr;
  }

  delete texNormalheightmaps; texNormalheightmaps = nullptr;
  delete texWorldColors; texWorldColors = nullptr;
  delete texBorders; texBorders = nullptr;
}

const u32&   Earth::getResolution()     const { return resolution;     }
const float& Earth::getRadius()         const { return radius;         }
const float& Earth::getHeightmapScale() const { return heightmapScale; }

void Earth::setHeightmapScale(const float& n) { heightmapScale = n; }
void Earth::setCountriesBorders(const Mesh<Vertex1>& mesh) { countriesBorders = mesh; }

void Earth::rebuild() {
  rebuild(resolution, radius);
};

void Earth::rebuild(int resolution, float radius) {
  this->resolution = resolution;
  this->radius = radius;
  build();
}

void Earth::draw(const Camera* camera, const Shader& shader) const {
  static const GLint heightmapScaleLoc = shader.getUniformLoc("u_heightmapScale");
  static const GLint nhmsLoc = shader.getUniformLoc("u_normalheightmaps");
  static const GLint wcLoc = shader.getUniformLoc("u_worldColors");
  static const GLint bordersLoc = shader.getUniformLoc("u_borders");
  static const GLint lightMultLoc = shader.getUniformLoc("u_lightMultiplier");
  static const GLint ambientLoc = shader.getUniformLoc("u_ambient");
  static const GLint specularLightLoc = shader.getUniformLoc("u_specularLight");
  static const GLint borderColorLoc = shader.getUniformLoc("u_borderColor");

  shader.setUniform1f(lightMultLoc, lightMultiplier);
  shader.setUniform1f(heightmapScaleLoc, heightmapScale);
  shader.setUniform1f(ambientLoc, ambient);
  shader.setUniform1f(specularLightLoc, specularLight);
  shader.setUniform3f(borderColorLoc, borderColor);
  shader.setUniformTexture(nhmsLoc, Earth::texNormalheightmaps->getUnit());
  shader.setUniformTexture(wcLoc, Earth::texWorldColors->getUnit());
  shader.setUniformTexture(bordersLoc, Earth::texBorders->getUnit());

  Earth::texNormalheightmaps->bind();
  Earth::texWorldColors->bind();
  Earth::texBorders->bind();

  for (u8 i = 0; i < 6; i++)
    terrainFaces[i].draw(camera, shader);

  Earth::texNormalheightmaps->unbind();
  Earth::texWorldColors->unbind();
  Earth::texBorders->unbind();
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

