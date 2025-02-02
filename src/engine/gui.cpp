#include "gui.hpp"

#include <cstdio>

#include "imgui.h"
#include "planet/Planet.hpp"

using namespace ImGui;

struct RunOnce {
  template<typename T> RunOnce(T&& f) { f(); }
};

namespace gui {

Planet* planet;
int planetResolution;
float planetRadius;

void link(Planet* p) {
  planet = p;
  planetResolution = p->getResolutino();
  planetRadius = p->getRadius();
}

void draw() {
  static RunOnce a([]() {
    ImGui::SetNextWindowSize({400, 100});
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowCollapsed(true);
  });

  Begin("Settings");

  SliderInt("Resolution", &planetResolution, 2, 1000);
  SliderFloat("Radius", &planetRadius, 1.f, 10.f);

  if (Button("Apply")) {
    if (!planet) puts("The planet is not linked to gui");
    else planet->rebuild(
      static_cast<u16>(planetResolution),
      static_cast<float>(planetRadius)
    );
  }

  End();
}
} // namespace gui
