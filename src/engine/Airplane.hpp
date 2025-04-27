#pragma once

#include "mesh/Mesh.hpp"

class Airplane : public Mesh {
public:
  Airplane(vec3 position, vec3 forward, float speedRad, float flyHeight, float meshSize = 1.f);

  void update();
  void drawForward(const Camera* camera, const Shader& shader, float size = 1.f, vec3 color = {1.f, 0.f, 0.f});

private:
  vec3 position;
  vec3 forward;
  float speedRad;
  float flyHeight;
};

