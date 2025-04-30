#pragma once

#include "Shader.hpp"

class Camera {
public:
  Camera(vec3 pos, vec3 orientation, float sensitivity);

  const float& getFov()        const;
  const vec3& getOrientation() const;
  const vec3& getPosition()    const;
  const vec3& getUp()          const;
  const mat4& getMatrix()      const;

  vec3 getBack()    const;
  vec3 getLeft()    const;
  vec3 getRight()   const;
  vec3 getForward() const;
  vec3 getBottom()  const;

  void setIncreasedSpeed();
  void setNormalSpeed();

  void update();
  void draw(Camera& camToDraw, const Shader& shader) const;

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

  mat4 mat  = mat4(1.f);
  mat4 view = mat4(1.f);

protected:
  virtual void calcView();
  void calcOrientation(const float& radRotX, const float& radRotY);
};

