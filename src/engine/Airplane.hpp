#pragma once

#include "mesh/Mesh.hpp"

class Airplane : public Mesh {
public:
  Airplane(vec3 position, vec3 forward, float speedRad, float flyHeight, float meshSize = 1.f);

  void update();

private:
  vec3 position;
  vec3 forward;
  float speedRad;
  float flyHeight;
};

