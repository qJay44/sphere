#include "gui.hpp"

#include <cstdio>

#include "imgui.h"

using namespace ImGui;

struct RunOnce {
  template<typename T> RunOnce(T&& f) { f(); }
};

namespace gui {

struct PlanetGUI {
  Planet* ptr;
  int res;
  float radius;
  float heighmapScale = 2.105f;
};

PlanetGUI planetGUI;

void link(Planet* p) { planetGUI = {p, p->getResolution(), p->getRadius(), 0.105f}; }

void draw() {
  static RunOnce a([]() {
    ImGui::SetNextWindowSize({400, 100});
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowCollapsed(true);
  });

  Begin("Settings");

  SliderInt("Resolution", &planetGUI.res, 2, 1'000);
  SliderFloat("Radius", &planetGUI.radius, 1.f, 10.f);

  if (SliderFloat("HeighmapScale", &planetGUI.heighmapScale, 0.01f, 1.f))
    planetGUI.ptr->setHeightmapScale(planetGUI.heighmapScale);

  if (Button("Apply")) {
    if (!planetGUI.ptr) puts("The planet is not linked to gui");
    else planetGUI.ptr->rebuild(static_cast<u16>(planetGUI.res), static_cast<float>(planetGUI.radius));
  }

  End();
}
} // namespace gui
