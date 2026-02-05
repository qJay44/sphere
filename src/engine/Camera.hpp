#pragma once

#include "Moveable.hpp"
#include "Shader.hpp"

#define CAMERA_FLAG_DRAW_RIGHT    1
#define CAMERA_FLAG_DRAW_UP       1 << 1
#define CAMERA_FLAG_DRAW_FORWARD  1 << 2
#define CAMERA_FLAG_DRAW_MESH     1 << 3
#define CAMERA_FLAG_DRAW_FRUSTUM  1 << 4

#define CAMERA_FLAG_DRAW_DIRECTIONS (CAMERA_FLAG_DRAW_RIGHT | CAMERA_FLAG_DRAW_UP | CAMERA_FLAG_DRAW_FORWARD)

class Camera : public Moveable {
public:
  Camera() = delete;
  Camera(Camera&) = delete;
  Camera(Camera&&) = delete;
  Camera(vec3 pos, float yaw = PI_2, float pitch = 0.f);

  const float& getNearPlane() const;
  const float& getFarPlane()  const;
  const float& getFov()       const;
  const mat4&  getProj()      const;
  const mat4&  getView()      const;
  const mat4&  getProjView()  const;

  mat4 getProjViewInv() const;

  void setNearPlane(float p);
  void setFarPlane(float p);

  void update();
  void draw(Camera& camToDraw, Shader& shader, u32 flags = 0) const;

protected:
  friend struct gui;
  friend struct InputsHandler;

  float nearPlane = 0.1f;
  float farPlane = 100.f;
  float fov = 45.f;
  float aspectRatio = 1.f;

  mat4 proj = mat4(1.f);
  mat4 view = mat4(1.f);
  mat4 pv   = mat4(1.f);
};

