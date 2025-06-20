#pragma once

#include "../engine/mesh/Mesh.hpp"

#include "Earth.hpp"
#include "Trail.hpp"

#define AIRPLANE_FLAG_DRAW_RIGHT    1
#define AIRPLANE_FLAG_DRAW_UP       1 << 2
#define AIRPLANE_FLAG_DRAW_FORWARD  1 << 3

#define AIRPLANE_FLAG_DRAW_DIRECTIONS (AIRPLANE_FLAG_DRAW_RIGHT | AIRPLANE_FLAG_DRAW_UP | AIRPLANE_FLAG_DRAW_FORWARD)

class Airplane : public Mesh<Vertex4> {
public:
  static void loadTextures();

  Airplane(const Earth& planet, vec3 position, float speedRad, float flyHeight, float turnSpeedRad, float meshScale = 1.f);
  ~Airplane();

  const vec3&  getPosition() const;
  const vec3&  getForward()  const;
  const vec3&  getUp()       const;
  const vec3&  getRight()    const;
  const float& getSpeed()    const;

  const glm::quat& getTurnQuat() const;
  const glm::quat& getRotateQuat() const;

  vec3 getBack() const;
  vec3 getDown() const;
  vec3 getLeft() const;

  void turn(float dir);
  void update();
  void draw(const Camera* camera, const Shader& shader, u32 flags = 0) const;
  void drawTrail(const Camera* camera, const Shader& shader) const;

private:
  friend struct gui;

  static Texture* texDiffuse;

  const Earth& planet;
  vec3 position;
  float speedRad;
  float flyHeight;
  float turnSpeedRad;
  float meshScale;

  vec3 forward;
  vec3 up;
  vec3 right;

  float turnMomentumDecreaseFactor = 0.45f;
  float tiltMomentumDecreaseFactor = 0.91f;
  float tiltRecoverMomentumDecreaseFactor = 0.11f;

  float turnMomentumRad = 0.f;
  float tiltMomentumRad = 0.f;
  float tiltRecoverMomentumRad = 0.f;

  glm::quat turnQuat;
  glm::quat rotateQuat;

  Trail trailLeft;
  Trail trailRight;
  vec3 trailOffset = vec3(540.f, 190.f, -50.f);

  float trailDuration = 2.f;
  float trailAlphaFactor = 0.3f;
};

