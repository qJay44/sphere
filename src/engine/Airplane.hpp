#pragma once

#include "mesh/Mesh.hpp"

#define AIRPLANE_FLAG_DRAW_FORWARD 1
#define AIRPLANE_FLAG_DRAW_UP      1 << 1

class Airplane : public Mesh {
public:
  Airplane(vec3 position, vec3 forward, float speedRad, float flyHeight, float meshSize = 1.f);

  const vec3& getPosition() const;
  const vec3& getUp() const;

  void update();
  void draw(const Camera* camera, const Shader& shader, int flags = 0);

private:
  vec3 position;
  vec3 forward;
  vec3 up;
  float speedRad;
  float flyHeight;
};

