#pragma once
#include "../mesh/Mesh.hpp"

#include "Earth.hpp"
#include "Trail.hpp"
#include "Light.hpp"
#include "glm/gtc/quaternion.hpp"

enum AirplaneFlags : u32 {
  AirplaneFlags_None        = 0,
  AirplaneFlags_DrawRight   = 1,
  AirplaneFlags_DrawUp      = 1 << 1,
  AirplaneFlags_DrawForward = 1 << 2,
  AirplaneFlags_DrawDirections = AirplaneFlags_DrawRight | AirplaneFlags_DrawUp | AirplaneFlags_DrawForward,
};

class Airplane : public Moveable, public Mesh {
public:
  Airplane(vec3 position, float flyHeight, const fspath& model, float meshScale = 1.f);

  void moveForward() override;
  void moveBack()    override;
  void moveLeft()    override;
  void moveRight()   override;
  void moveUp()      override;
  void moveDown()    override;

  void onMouseMove(dvec2 mousePos) override;
  void onMouseScroll(dvec2 offset) override;

  Camera& getCamera();

  void setTurnSpeed(float speedRad);
  void setFlags(u32 f);

  void turn(float dir);
  void update(const Earth& earth);
  void draw(const Camera* camera, Shader& shader) const;
  void drawTrails(const Camera* camera, Shader& shader) const;
  void drawLights(const Camera* camera, Shader& shader) const;
  void drawDirections(const Camera* camera, Shader& shader) const;

private:
  friend struct gui;

  static Texture2D texDiffuse;

  Camera camera;
  float flyHeight;
  float turnSpeedRad;
  float meshScale;
  Light lightLeft;
  Light lightRight;

  Trail trailLeft;
  Trail trailRight;
  vec3 trailOffset = vec3(540.f, 190.f, -50.f);

  float camDistance = 10.f;

  float turnMomentumRad = 0.f;
  float tiltMomentumRad = 0.f;

  float turnMomentumDamping = 0.45f;
  float tiltMomentumDamping = 0.95f;

  vec3 localRight{1.f, 0.f, 0.f};
  vec3 localUp{0.f, 0.f, 1.f};
  vec3 localOrientation{0.f, 1.f, 0.f};

  glm::quat localRotationQuat = glm::identity<glm::quat>();
  glm::quat orientationQuat   = glm::identity<glm::quat>();
  glm::quat yawQuat           = glm::identity<glm::quat>();
  glm::quat pitchQuat         = glm::identity<glm::quat>();
  glm::quat rollQuat          = glm::identity<glm::quat>();

  u32 flags = 0;

private:
  void updateCamera();
  void updateTrails();
};

