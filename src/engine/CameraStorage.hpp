#pragma once

#include "../objects/AirplaneCamera.hpp"

struct CameraStorage {
  static Camera* cameraFreePtr;
  static AirplaneCamera* cameraAirplanePtr;
};

