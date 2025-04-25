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
  ArcballCamera* ptr;
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

void link(ArcballCamera* c) {
  cameraGUI.ptr = c;
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

  cameraGUI.lonDeg = degrees(cameraGUI.ptr->getLongitude());
  cameraGUI.latDeg = degrees(cameraGUI.ptr->getLatitude());

  if (SliderFloat("Longitude", &cameraGUI.lonDeg, -180.f, 180.f))
    cameraGUI.ptr->setLongitude(cameraGUI.lonDeg);

  if (SliderFloat("Latitude", &cameraGUI.latDeg, -90.f, 90.f))
    cameraGUI.ptr->setLatitude(cameraGUI.latDeg);

  End();
}
} // namespace gui
