#pragma once

#include "../engine/Camera.hpp"

#include "Airplane.hpp"

class AirplaneCamera : public Camera {
public:
  AirplaneCamera(const Airplane& airplane, float distance, float sensitivity);

  void moveForward() override;
  void moveBack()    override;
  void moveLeft()    override;
  void moveRight()   override;
  void moveUp()      override;
  void moveDown()    override;

  void zoom(const float& dir);

private:
  const Airplane& airplane;
  float distance;

  vec3 rotAxisHorizontal;
  vec3 rotAxisVertical;

private:
  void moveByMouse(const dvec2& mousePos) override;
  void calcView() override;
};

