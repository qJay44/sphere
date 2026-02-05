#pragma once

#include "../engine/mesh/Mesh.hpp"

#include "Earth.hpp"
#include "Trail.hpp"
#include "glm/gtc/quaternion.hpp"

class Airplane : public Moveable, public Mesh {
public:
  Airplane(vec3 position, float flyHeight, float meshScale = 1.f);
  ~Airplane();

  void onMouseMove(dvec2 mousePos) override;

  Camera& getCamera();

  void setTurnSpeed(float speedRad);

  void turn(float dir);
  void update(const Earth& earth);
  void draw(const Camera* cam, Shader& shader) const;
  void drawTrail(const Camera* cam, Shader& shader) const;
  void drawDirections(const Camera* cam, Shader& shader) const;

private:
  friend struct gui;

  static Texture texDiffuse;

  Camera camera;
  float flyHeight;
  float turnSpeedRad;
  float meshScale;
  float camDistance = 10.f;

  float turnMomentumDecreaseFactor = 0.45f;
  float tiltMomentumDecreaseFactor = 0.91f;
  float tiltRecoverMomentumDecreaseFactor = 0.11f;

  float turnMomentumRad = 0.f;
  float tiltMomentumRad = 0.f;
  float tiltRecoverMomentumRad = 0.f;

  glm::quat turnQuat   = glm::identity<glm::quat>();
  glm::quat rotateQuat = glm::identity<glm::quat>();

  Trail trailLeft;
  Trail trailRight;
  vec3 trailOffset = vec3(540.f, 190.f, -50.f);

  float trailDuration = 2.f;
  float trailAlphaFactor = 0.3f;

  bool showRight = false;
  bool showUp = false;
  bool showForward = false;

private:
  void updateCamera();
};

