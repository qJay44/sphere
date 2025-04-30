#pragma once

#include "glm/ext/matrix_transform.hpp"

class Camera {
public:
  Camera(vec3 pos, vec3 orientation, float sensitivity);

  const vec3& getOrientation() const;
  const vec3& getPosition()    const;
  const mat4& getMatrix()      const;
  const vec3& getUp()          const;

  vec3 getBack()  const;
  vec3 getRight() const;

  void setIncreasedSpeed();
  void setNormalSpeed();

  void update();

  virtual void moveForward();
  virtual void moveBack();
  virtual void moveLeft();
  virtual void moveRight();
  virtual void moveUp();
  virtual void moveDown();

  virtual void moveByMouse(const double& x, const double& y);

protected:
  vec3 position, orientation;
  float sensitivity;

  vec3 up{0.f, 1.f, 0.f};
  float speed = 3.f;
  float fov = 45.f;

  mat4 mat = identity<mat4>();
  mat4 view = identity<mat4>();

protected:
  virtual void calcView();
  void calcOrientation(const float& radRotX, const float& radRotY);
};

