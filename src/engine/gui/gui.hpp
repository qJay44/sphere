#pragma once

#include "../objects/Airplane.hpp"
#include "../objects/Earth.hpp"
#include "../objects/Light.hpp"

struct gui {
  static Earth* earthPtr;
  static Camera* camSpecatePtr;
  static Airplane* airplanePtr;
  static Light* lightPtr;
  static u16 fps;

  static void toggle();
  static void draw();
};

