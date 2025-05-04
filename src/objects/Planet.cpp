#include "Planet.hpp"

#include "../engine/mesh/texture/image2D.hpp"

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

Planet::Planet(int resolution, float radius, const fspath& texturePath) //
  : resolution(resolution),
    radius(radius) {
  image2D img(texturePath);
  image2D img_w2 = img;
  img_w2.width /= 2;

  glPixelStorei(GL_UNPACK_ROW_LENGTH, img.width);
  textures[0] = Texture(img_w2, GL_TEXTURE_2D, "heightmap0", 0, 1);
  img_w2.pixels += img_w2.width * img_w2.channels;
  textures[1] = Texture(img_w2, GL_TEXTURE_2D, "heightmap1", 1, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  img_w2.pixels = nullptr;

  build();
}

const int& Planet::getResolution() const { return resolution; }
const float& Planet::getRadius() const { return radius; };
const float& Planet::getHeightmapScale() const { return heightmapScale; }
const float& Planet::getSeaLevel() const { return seaLevel; }

void Planet::setHeightmapScale(const float& n) { heightmapScale = n; }
void Planet::setSeaLevel(const float& n) { seaLevel = n; }

void Planet::rebuild() {
  rebuild(resolution, radius);
};

void Planet::rebuild(int resolution, float radius) {
  this->resolution = resolution;
  this->radius = radius;
  build();
}

void Planet::draw(const Camera* camera, const Shader& shader) const {
  static const GLint heightmapScaleUniLoc = shader.getUniformLoc("heightmapScale");
  static const GLint seaLevelUniLoc = shader.getUniformLoc("seaLevel");
  shader.setUniform1f(heightmapScaleUniLoc, heightmapScale * radius);
  shader.setUniform1f(seaLevelUniLoc, seaLevel);

  for (u8 i = 0; i < 6; i++)
    terrainFaces[i].draw(camera, shader);
}

void Planet::build() {
  for (u8 i = 0; i < 6; i++) {
    std::vector<Vertex> vertices(resolution * resolution);
    std::vector<GLuint> indices((resolution - 1) * (resolution) * 2 * 3);
    vec3 localUp = directions[i];
    vec3 axisA = vec3(localUp.y, localUp.z, localUp.x);
    vec3 axisB = cross(localUp, axisA);
    u32 triIndex = 0;
    float percentStep = 1.f / (resolution - 1);

    for (int y = 0; y < resolution; y++) {
      float percentY = y * percentStep;
      vec3 pY = (percentY - 0.5f) * 2.f * axisB;

      for (int x = 0; x < resolution; x++) {
        u32 idx = x + y * resolution;
        float percentX = x * percentStep;
        vec3 pX = (percentX - 0.5f) * 2.f * axisA;
        vec3 pointOnPlane = localUp + pX + pY;
        Vertex& vertex = vertices[idx];

        vertex = {pointOnSphereFancy(pointOnPlane), palette[i]};
        vertex.normal = vertex.position;
        vertex.texture = {percentX, percentY};
        vertex.position *= radius;

        if (x != resolution - 1 && y != resolution - 1) {
          indices[triIndex + 0] = idx;
          indices[triIndex + 1] = idx + resolution + 1;
          indices[triIndex + 2] = idx + resolution;

          indices[triIndex + 3] = idx;
          indices[triIndex + 4] = idx + 1;
          indices[triIndex + 5] = idx + resolution + 1;

          triIndex += 6;
        }
      }
    }
    terrainFaces[i] = Mesh(vertices, indices);
    terrainFaces[i].add(&textures[0]);
    terrainFaces[i].add(&textures[1]);
  }
}

vec3 Planet::pointOnSphereDefault(const vec3& v) const { return v / length(v); }

vec3 Planet::pointOnSphereFancy(const vec3& v) const {
  float x2 = v.x * v.x;
  float y2 = v.y * v.y;
  float z2 = v.z * v.z;
  float x = v.x * sqrtf(1.f - (y2 + z2) * 0.5f + (y2 * z2) / 3.f);
  float y = v.y * sqrtf(1.f - (z2 + x2) * 0.5f + (z2 * x2) / 3.f);
  float z = v.z * sqrtf(1.f - (x2 + y2) * 0.5f + (x2 * y2) / 3.f);

  return vec3(x, y, z);
}

