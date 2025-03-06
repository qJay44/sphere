#include "gui.hpp"

#include <cstdio>

#include "imgui.h"

using namespace ImGui;

struct RunOnce {
  template<typename T> RunOnce(T&& f) { f(); }
};

namespace gui {

static bool collapsed = true;

struct PlanetGUI {
  Planet* ptr;
  int res;
  float radius;
  float heighmapScale;
  float seaLevel;
};

struct CameraGUI {
  Camera* ptr;
  vec2 lookCoordsDeg;
};

PlanetGUI planetGUI;
CameraGUI cameraGUI;

void link(Planet* p) {
  planetGUI = {
    p, //
    p->getResolution(),
    p->getRadius(),
    p->getHeightmapScale(),
    p->getSeaLevel(),
  };
}

void link(Camera* c) {
  cameraGUI.ptr = c;
  cameraGUI.lookCoordsDeg = degrees(c->getLookCoords());
}

void toggle() { collapsed = !collapsed; }

void draw() {
  static RunOnce a([]() {
    SetNextWindowSize({400, 280});
    SetNextWindowPos({0, 0});
  });
  SetNextWindowCollapsed(collapsed);

  Begin("Settings");

  // ================== Planet ==================

  SeparatorText("Planet");
  SliderInt("Resolution", &planetGUI.res, 2, 1'000);
  SliderFloat("Radius", &planetGUI.radius, 1.f, 10.f);

  if (SliderFloat("Heighmap scale", &planetGUI.heighmapScale, 0.01f, 1.f))
    planetGUI.ptr->setHeightmapScale(planetGUI.heighmapScale);

  if (SliderFloat("Sea level", &planetGUI.seaLevel, -10.f, 10.f)) //
    planetGUI.ptr->setSeaLevel(planetGUI.seaLevel);

  if (Button("Apply")) {
    if (!planetGUI.ptr) error("The planet is not linked to gui");
    else planetGUI.ptr->rebuild(static_cast<u16>(planetGUI.res), static_cast<float>(planetGUI.radius));
  }

  // ================== Camera ==================

  SeparatorText("Camera");
  if (!cameraGUI.ptr) error("The camera is not linked to gui");

  cameraGUI.lookCoordsDeg = degrees(cameraGUI.ptr->getLookCoords());
  bool lonChanged = SliderFloat("Longitude", &cameraGUI.lookCoordsDeg.x, -180.f, 180.f);
  bool latChanged = SliderFloat("Latitude", &cameraGUI.lookCoordsDeg.y, -90.f, 90.f);

  if (lonChanged || latChanged) //
    cameraGUI.ptr->setLookCoords(radians(cameraGUI.lookCoordsDeg));

  const vec3& pos = cameraGUI.ptr->getPosition();
  Text("x: %.2f\ny: %.2f\nz: %.2f", pos.x, pos.y, pos.z);

  End();
}
} // namespace gui
