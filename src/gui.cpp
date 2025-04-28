#include "gui.hpp"

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
  AirplaneCamera* arcball;
  Camera* free;
  float lonDeg, latDeg;
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

void link(AirplaneCamera* c) { cameraGUI.arcball = c; }
void link(Camera* c) { cameraGUI.free = c; }

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

  // ================== Arball Camera ==================

  SeparatorText("Airplane Camera");
  if (!cameraGUI.arcball) error("The arcball camera is not linked to gui");

  // ================== Free Camera ====================

  SeparatorText("Free Camera");
  if (!cameraGUI.arcball) error("The arcball camera is not linked to gui");

  if (!cameraGUI.free) error("The free camera is not linked to gui");
  const vec3& pos = cameraGUI.free->getPosition();
  Text("x: %.2f\ty: %.2f\tz: %.2f\t", pos.x, pos.y, pos.z);

  const vec3& orientation = cameraGUI.free->getOrientation();
  Text("x: %.2f\ty: %.2f\tz: %.2f\t", orientation.x, orientation.y, orientation.z);

  End();
}
} // namespace gui
