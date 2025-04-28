#pragma once

#include "../engine/mesh/Mesh.hpp"

#include "Planet.hpp"

#define AIRPLANE_FLAG_DRAW_FORWARD 1
#define AIRPLANE_FLAG_DRAW_UP      1 << 1

class Airplane : public Mesh {
public:
  Airplane(const Planet& planet, vec3 position, float speedRad, float flyHeight, float meshSize = 1.f);

  const vec3& getPosition()    const;
  const vec3& getForward()     const;
  const vec3& getUp()          const;

  void update();
  void draw(const Camera* camera, const Shader& shader, int flags = 0);

private:
  const Planet& planet;

  vec3 position;
  vec3 forward;
  vec3 up;

  float speedRad;
  float flyHeight;
};

