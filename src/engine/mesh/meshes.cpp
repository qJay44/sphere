#include "meshes.hpp"

#include <vector>

#include "../Camera.hpp"

namespace meshes {

Mesh line(vec3 p1, vec3 p2, vec3 color) {
  std::vector<Vertex> vertices{
    {p1, color},
    {p2, color}
  };

  return Mesh(vertices, GL_LINES);
}

Mesh axis(float size) {
  std::vector<Vertex> vertices{
    {{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}},
    {{size, 0.f, 0.f}, {1.f, 0.f, 0.f}},
    {{0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}},
    {{0.f, size, 0.f}, {0.f, 1.f, 0.f}},
    {{0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}},
    {{0.f, 0.f, size}, {0.f, 0.f, 1.f}},
  };

  return Mesh(vertices, GL_LINES);
}

Mesh plane(vec3 pos, vec2 size, vec3 color) {
  std::vector<Vertex> vertices{
    {{-0.1f, -0.1f, pos.z}, color, {0.f, 0.f}},
    {{-0.1f,  0.1f, pos.z}, color, {0.f, 1.f}},
    {{ 0.1f,  0.1f, pos.z}, color, {1.f, 1.f}},
    {{ 0.1f, -0.1f, pos.z}, color, {1.f, 0.f}},
  };

  std::vector<GLuint> indices{
    0, 1, 2,
    2, 3, 0
  };


  Mesh m = Mesh(vertices, indices);
  m.translate(pos);
  m.scale(size);

  return m;
}

Mesh cube(vec3 pos, float size, vec3 color) {
  //        5--------6
  //       /|       /|
  //      1--------2 |
  //      | |      | |
  //      | 4------|-7
  //      |/       |/
  //      0--------3
  //
  //      x: - to left, + to right
  //      y: - to down, + to up
  //      z: - to rear, + to front

  vec2 tex{0.f, 0.f};
  std::vector<Vertex> vertices{
    {{-1.f, -1.f,  1.f}, color, tex, {-1.f, -1.f,  1.f}},
    {{-1.f,  1.f,  1.f}, color, tex, {-1.f,  1.f,  1.f}},
    {{ 1.f,  1.f,  1.f}, color, tex, { 1.f,  1.f,  1.f}},
    {{ 1.f, -1.f,  1.f}, color, tex, { 1.f, -1.f,  1.f}},
    {{-1.f, -1.f, -1.f}, color, tex, {-1.f, -1.f, -1.f}},
    {{-1.f,  1.f, -1.f}, color, tex, {-1.f,  1.f, -1.f}},
    {{ 1.f,  1.f, -1.f}, color, tex, { 1.f,  1.f, -1.f}},
    {{ 1.f, -1.f, -1.f}, color, tex, { 1.f, -1.f, -1.f}},
  };

  std::vector<GLuint> indices{
    0, 1, 2, //  Font face
    2, 3, 0, //
    4, 5, 6, //  Rear face
    6, 7, 4, //
    0, 1, 5, //  Left face
    5, 4, 0, //
    3, 2, 6, //  Right face
    6, 7, 3, //
    1, 5, 6, //  Top face
    6, 2, 1, //
    0, 4, 7, //  Bottom face
    7, 3, 0, //
  };

  Mesh m = Mesh(vertices, indices);
  m.translate(pos);
  m.scale(size);

  return m;
}

Mesh frustum(const Camera& cam, vec3 color) {
  float fovRad = glm::radians(cam.getFov());

  float nearWidth = cos(fovRad) * global::nearPlane;
  float nearHeight = sin(fovRad) * global::nearPlane;
  float nearWidth2 = nearWidth * 0.5f;
  float nearHeight2 = nearHeight * 0.5f;

  vec3 nearPos = cam.getPosition() + cam.getForward() * global::nearPlane;
  vec3 nearTL = nearPos + cam.getLeft()  * nearWidth2 + cam.getUp() * nearHeight2;
  vec3 nearTR = nearPos + cam.getRight() * nearWidth2 + cam.getUp() * nearHeight2;
  vec3 nearBR = nearPos + cam.getRight() * nearWidth2 + cam.getBottom() * nearHeight2;
  vec3 nearBL = nearPos + cam.getLeft()  * nearWidth2 + cam.getBottom() * nearHeight2;

  float farWidth = cos(fovRad) * global::farPlane;
  float farHeight = sin(fovRad) * global::farPlane;
  float farWidth2 = farWidth * 0.5f;
  float farHeight2 = farHeight * 0.5f;

  vec3 farPos = cam.getPosition()  + cam.getForward() * global::farPlane;
  vec3 farTL = farPos + cam.getLeft()  * farWidth2 + cam.getUp() * farHeight2;
  vec3 farTR = farPos + cam.getRight() * farWidth2 + cam.getUp() * farHeight2;
  vec3 farBR = farPos + cam.getRight() * farWidth2 + cam.getBottom() * farHeight2;
  vec3 farBL = farPos + cam.getLeft()  * farWidth2 + cam.getBottom() * farHeight2;

  std::vector<Vertex> vertices {
    // Near plane
    {nearTL, color},
    {nearTR, color},
    {nearTR, color},
    {nearBR, color},
    {nearBR, color},
    {nearBL, color},
    {nearBL, color},
    {nearTL, color},
    // Far plane
    {farTL, color},
    {farTR, color},
    {farTR, color},
    {farBR, color},
    {farBR, color},
    {farBL, color},
    {farBL, color},
    {farTL, color},
    // Connect planes corners
    {nearTL, color},
    {farTL, color},
    {nearTR, color},
    {farTR, color},
    {nearBR, color},
    {farBR, color},
    {nearBL, color},
    {farBL, color},
  };

  return Mesh(vertices, GL_LINES);
}

// FIXME: Lacking one segment
Mesh sphere(float radius, u16 resolution, vec3 color) {
  std::vector<Vertex> vertices(resolution * resolution);
  std::vector<GLuint> indices((resolution - 1) * (resolution - 1) * 2 * 3);

  float stepLon = (2.f * PI) / resolution; // From -PI to PI, total length = 2PI
  float stepLat = PI / resolution;         // From -PI_2 to PI_2, total length = 2PI_2 -> PI
  u32 triIndex = 0;

  for (u32 i = 0; i < resolution; i++) {
    float lon = i * stepLon;
    for (u32 j = 0; j < resolution; j++) {
      float lat = j * stepLat;
      u32 idx = i + j * resolution;
      vec3 point{
        radius * sinf(lat) * cosf(lon),
        radius * sinf(lat) * sinf(lon),
        radius * cosf(lat),
      };

      vertices[idx] = {point, color, {}, point / radius};

      if (i != resolution - 1 && j != resolution - 1) {
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

  return Mesh(vertices, indices);
}
} // namespace meshes
