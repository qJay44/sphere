#pragma once

#include "../engine/mesh/Mesh.hpp"

#include "Planet.hpp"

#define AIRPLANE_FLAG_DRAW_FORWARD 1
#define AIRPLANE_FLAG_DRAW_UP      1 << 2
#define AIRPLANE_FLAG_DRAW_RIGHT   1 << 3

class Airplane : public Mesh {
public:
  Airplane(const Planet& planet, vec3 position, float speedRad, float flyHeight, float meshSize = 1.f);

  const vec3&  getPosition()       const;
  const vec3&  getForward()        const;
  const vec3&  getUp()             const;
  const vec3&  getRight()          const;
  const float& getSpeed()          const;
  const glm::quat& getQuaternion() const;

  vec3 getBack() const;
  vec3 getDown() const;
  vec3 getLeft() const;

  void update();
  void draw(const Camera* camera, const Shader& shader, u32 flags = 0);

private:
  const Planet& planet;

  vec3 position;
  vec3 forward;
  vec3 up;
  vec3 right;
  vec3 rotAxis{-1.f, 0.f, 0.f};
  glm::quat quat;

  float speedRad;
  float flyHeight;
};

