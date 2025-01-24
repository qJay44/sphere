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

void link(Planet* p) { planet = p; }

void draw() {
  static RunOnce a([]() {
    ImGui::SetNextWindowSize({400, 100});
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowCollapsed(true);
  });

  Begin("Settings");

  SliderInt("Resolution", &_gState.resolution, 2, 60);

  if (Button("Apply")) {
    if (!planet) puts("The planet is not linked to gui");
    else planet->rebuild();
  }

  End();
}
} // namespace gui
