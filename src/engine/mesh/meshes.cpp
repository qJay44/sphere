#include "meshes.hpp"

#include <vector>

#include "../Camera.hpp"
#include "../frustum/Frustum.hpp"

namespace meshes {

Mesh line(vec3 p1, vec3 p2, vec3 color) {
  std::vector<VertexPC> vertices{
    {p1, color},
    {p2, color}
  };

  std::vector<GLuint> indices{
    0, 1
  };

  return Mesh(vertices, indices, GL_LINES);
}

Mesh axis() {
  std::vector<VertexPC> vertices{
    {{0.f, 0.f, 0.f}, global::red},
    {{1.f, 0.f, 0.f}, global::red},
    {{0.f, 0.f, 0.f}, global::green},
    {{0.f, 1.f, 0.f}, global::green},
    {{0.f, 0.f, 0.f}, global::blue},
    {{0.f, 0.f, 1.f}, global::blue},
  };

  std::vector<GLuint> indices{
    0, 1,
    2, 3,
    4, 5
  };

  return Mesh(vertices, indices, GL_LINES);
}

Mesh plane(vec3 color, GLenum mode) {
  std::vector<Vertex4> vertices{
    {{-1.f, -1.f, 0.f}, color, {0.f, 0.f}, {1.f, 0.f, 0.f}},
    {{-1.f,  1.f, 0.f}, color, {0.f, 1.f}, {1.f, 0.f, 0.f}},
    {{ 1.f,  1.f, 0.f}, color, {1.f, 1.f}, {1.f, 0.f, 0.f}},
    {{ 1.f, -1.f, 0.f}, color, {1.f, 0.f}, {1.f, 0.f, 0.f}},
  };

  std::vector<GLuint> indices;

  switch (mode) {
    case GL_TRIANGLES: {
      indices = {
        0, 1, 2,
        2, 3, 0
      };
      break;
    }
    case GL_PATCHES: {
      indices = {
        0, 1,
        2, 3,
      };
      break;
    }
    default:
      error("[meshes::plane] Unhandled mode [{}]", mode);
  }

  return Mesh(vertices, indices, mode);
}

Mesh plane(size_t resolution, GLenum mode, vec3 up) {
  size_t indicesPerQuad = 0;
  if      (mode == GL_TRIANGLES) indicesPerQuad = 6;
  else if (mode == GL_PATCHES)   indicesPerQuad = 4;
  else error("[meshes::plane] Unexpected mode [{}]", mode);

  std::vector<VertexPT> vertices(resolution * resolution);
  std::vector<GLuint> indices((resolution - 1) * (resolution - 1) * indicesPerQuad);

  vec3 axisA = vec3(up.y, up.z, up.x);
  vec3 axisB = cross(up, axisA);
  size_t triIndex = 0;

  for (size_t y = 0; y < resolution; y++) {
    float percentY = y / (resolution - 1.f);
    vec3 pY = (percentY - 0.5f) * 2.f * axisB;

    for (size_t x = 0; x < resolution; x++) {
      size_t idx = x + y * resolution;
      float percentX = x / (resolution - 1.f);
      vec3 pX = (percentX - 0.5f) * 2.f * axisA;

      VertexPT& vert = vertices[idx];
      vert.position = up + pX + pY;
      vert.texture = {percentX, percentY};

      switch (mode) {
        case GL_TRIANGLES: {
          if (x != resolution - 1 && y != resolution - 1) {
            indices[triIndex + 0] = idx + resolution;     // 2       0 -------- 1
            indices[triIndex + 1] = idx;                  // 0       |          |
            indices[triIndex + 2] = idx + 1;              // 1       |          |
            //                                                       |          |
            indices[triIndex + 3] = idx + 1;              // 1       |          |
            indices[triIndex + 4] = idx + resolution + 1; // 3       |          |
            indices[triIndex + 5] = idx + resolution;     // 2       2 -------- 3

            triIndex += 6;
          }
          break;
        }
        case GL_PATCHES: {
          if (x != resolution - 1 && y != resolution - 1) {
            indices[triIndex + 0] = idx;                  // 0
            indices[triIndex + 1] = idx + 1;              // 1
            indices[triIndex + 2] = idx + resolution + 1; // 3
            indices[triIndex + 3] = idx + resolution;     // 2

            triIndex += 4;
          }
          break;
        }
      }
    }
  }

  return Mesh(vertices, indices, mode);
}

Mesh cube(vec3 color) {
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
  std::vector<Vertex4> vertices{
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
    0, 1, 2, //  Front face
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

  return Mesh(vertices, indices, GL_TRIANGLES);
}

Mesh frustum(const Camera& cam, vec3 color) {
  frustum::Frustum frustum(cam);

  ivec2 winSize;
  glfwGetWindowSize(global::window, &winSize.x, &winSize.y);
  float aspect = static_cast<float>(winSize.x) / winSize.y;

  vec3 camCrossUp = normalize(cross(cam.getForward(), cam.getLeft()));
  float fovRad = glm::radians(cam.getFov());
  float fovSize = 0.f;
  if (fovRad >= PI_2) {
    fovSize = 57.28f;
    fovRad -= PI_2;
  }
  fovSize += tan(fovRad);

  // NOTE: Frustum's forward is pointing towards +z

  float farVSideHalf = cam.getFarPlane() * fovSize * 0.5f;
  float farHSideHalf = farVSideHalf * aspect;
  vec3 frontMultFar = cam.getForward() * cam.getFarPlane();
  vec3 farPos = cam.getPosition() + frontMultFar;
  vec3 farTR = farPos + cam.getLeft()  * farHSideHalf +  camCrossUp * farVSideHalf;
  vec3 farTL = farPos + cam.getRight() * farHSideHalf +  camCrossUp * farVSideHalf;
  vec3 farBL = farPos + cam.getRight() * farHSideHalf + -camCrossUp * farVSideHalf;
  vec3 farBR = farPos + cam.getLeft()  * farHSideHalf + -camCrossUp * farVSideHalf;

  float nearVSideHalf = cam.getNearPlane() * fovSize * 0.5f;
  float nearHSideHalf = nearVSideHalf * aspect;
  vec3 nearPos = cam.getPosition() + cam.getForward() * cam.getNearPlane();
  vec3 nearTR = nearPos + cam.getLeft()  * nearHSideHalf +  camCrossUp * nearVSideHalf;
  vec3 nearTL = nearPos + cam.getRight() * nearHSideHalf +  camCrossUp * nearVSideHalf;
  vec3 nearBL = nearPos + cam.getRight() * nearHSideHalf + -camCrossUp * nearVSideHalf;
  vec3 nearBR = nearPos + cam.getLeft()  * nearHSideHalf + -camCrossUp * nearVSideHalf;

  vec3 centerTR = (farTR + nearTR) * 0.5f;
  vec3 centerTL = (farTL + nearTL) * 0.5f;
  vec3 centerBR = (farBR + nearBR) * 0.5f;
  vec3 centerBL = (farBL + nearBL) * 0.5f;

  vec3 centerTop    = (centerTR + centerTL) * 0.5f;
  vec3 centerBottom = (centerBR + centerBL) * 0.5f;
  vec3 centerRight  = (centerTR + centerBR) * 0.5f;
  vec3 centerLeft   = (centerTL + centerBL) * 0.5f;

  std::vector<VertexPC> vertices {
    // Near plane
    {nearTR, color},
    {nearTL, color},
    {nearBL, color},
    {nearBR, color},
    // Far plane
    {farTR, color},
    {farTL, color},
    {farBL, color},
    {farBR, color},
    // Normals
    {centerTop, global::green},
    {centerTop + frustum.topFace.normal, global::green},
    {centerBottom, global::green},
    {centerBottom + frustum.bottomFace.normal, global::green},
    {centerRight, global::red},
    {centerRight + frustum.rightFace.normal, global::red},
    {centerLeft, global::red},
    {centerLeft + frustum.leftFace.normal, global::red},
    {nearPos, global::blue},
    {nearPos + frustum.nearFace.normal, global::blue},
    {farPos, global::blue},
    {farPos + frustum.farFace.normal, global::blue},
  };

  std::vector<GLuint> indices {
    // Near plane
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    // Far plane
    4, 5,
    5, 6,
    6, 7,
    7, 4,
    // Connect corners
    0, 4,
    1, 5,
    2, 6,
    3, 7,
    // Normals
    8, 9,
    10, 11,
    12, 13,
    14, 15,
    16, 17,
    18, 19
  };

  return Mesh(vertices, indices, GL_LINES);
}

} // namespace meshes

