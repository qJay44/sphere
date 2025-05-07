#pragma once

#include "objects/Planet.hpp"
#include "objects/AirplaneCamera.hpp"
#include "objects/Light.hpp"

struct gui {
  static void link(Planet* ptr);
  static void link(AirplaneCamera* ptr);
  static void link(Camera* ptr);
  static void link(Airplane* ptr);
  static void link(Light* ptr);
  static void toggle();
  static void draw();
};

