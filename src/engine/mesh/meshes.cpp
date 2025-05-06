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

  Mesh m = Mesh(vertices, indices);
  m.translate(pos);
  m.scale(size);

  return m;
}

Mesh frustum(const Camera& cam, vec3 color) {
  vec2 winSize = getWinSize();
  float aspect = winSize.x / winSize.y;

  vec3 camCrossUp = normalize(cross(cam.getForward(), cam.getLeft()));
  float fovRad = glm::radians(cam.getFov());
  float fovSize = 0.f;
  if (fovRad >= PI_2) {
    fovSize = 57.28f;
    fovRad -= PI_2;
  }
  fovSize += tan(fovRad);

  float farVSideHalf = cam.getFarPlane() * fovSize * 0.5f;
  float farHSideHalf = farVSideHalf * aspect;
  vec3 frontMultFar = cam.getForward() * cam.getFarPlane();
  vec3 farPos = cam.getPosition() + frontMultFar;
  vec3 farTL = farPos + cam.getLeft()  * farHSideHalf +  camCrossUp * farVSideHalf;
  vec3 farTR = farPos + cam.getRight() * farHSideHalf +  camCrossUp * farVSideHalf;
  vec3 farBR = farPos + cam.getRight() * farHSideHalf + -camCrossUp * farVSideHalf;
  vec3 farBL = farPos + cam.getLeft()  * farHSideHalf + -camCrossUp * farVSideHalf;

  float nearVSideHalf = cam.getNearPlane() * fovSize * 0.5f;
  float nearHSideHalf = nearVSideHalf * aspect;
  vec3 nearPos = cam.getPosition() + cam.getForward() * cam.getNearPlane();
  vec3 nearTL = nearPos + cam.getLeft()  * nearHSideHalf +  camCrossUp * nearVSideHalf;
  vec3 nearTR = nearPos + cam.getRight() * nearHSideHalf +  camCrossUp * nearVSideHalf;
  vec3 nearBR = nearPos + cam.getRight() * nearHSideHalf + -camCrossUp * nearVSideHalf;
  vec3 nearBL = nearPos + cam.getLeft()  * nearHSideHalf + -camCrossUp * nearVSideHalf;

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

} // namespace meshes

