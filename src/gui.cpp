#include "gui.hpp"

#include <algorithm>

#include "imgui.h"

#define IM_RED    IM_COL32(255, 0  , 0  , 255)
#define IM_GREEN  IM_COL32(0  , 255, 0  , 255)
#define IM_BLUE   IM_COL32(0  , 0  , 255, 255)
#define IM_WHITE  IM_COL32(255, 255, 255, 255)

#define IM_CIRCLE_RADIUS 30.f

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

struct AirplaneGUI {
  Airplane* ptr;
};

PlanetGUI planetGUI;
CameraGUI cameraGUI;
AirplaneGUI airplaneGUI;

void link(Planet* ptr) {
  planetGUI = {
    ptr,
    ptr->getResolution(),
    ptr->getRadius(),
    ptr->getHeightmapScale(),
    ptr->getSeaLevel(),
  };
}

void link(AirplaneCamera* ptr) { cameraGUI.arcball = ptr; }
void link(Camera* ptr)         { cameraGUI.free    = ptr; }
void link(Airplane* ptr)       { airplaneGUI.ptr   = ptr; }

void toggle() { collapsed = !collapsed; }

void drawDirection(ImDrawList* drawList, vec3 dir, const ImU32& color, float offsetX) {
  ImVec2 pos = GetCursorScreenPos();
  pos.x += IM_CIRCLE_RADIUS + offsetX;
  pos.y += IM_CIRCLE_RADIUS;
  dir.z = std::max(dir.z, 0.1f);
  ImVec2 p1{pos.x, pos.y};
  ImVec2 p2{
    p1.x + std::clamp(dir.x / abs(dir.z), -1.f, 1.f) * IM_CIRCLE_RADIUS,
    p1.y - std::clamp(dir.y / abs(dir.z), -1.f, 1.f) * IM_CIRCLE_RADIUS
  };

  drawList->AddCircleFilled(pos, IM_CIRCLE_RADIUS, IM_COL32(30, 30, 30, 255));
  drawList->AddCircle(pos, IM_CIRCLE_RADIUS, IM_WHITE);
  drawList->AddLine(p1, p2, color);
}


void draw() {
  static RunOnce a([]() {
    SetNextWindowSize({400, 500});
    SetNextWindowPos({0, 0});
  });
  SetNextWindowCollapsed(collapsed);

  Begin("Settings");

  ImDrawList* drawList = GetWindowDrawList();

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

  // ================== Airplane Camera ================

  SeparatorText("Airplane Camera");
  if (!cameraGUI.arcball) error("The arcball camera is not linked to gui");

  // Show directions
  {
    static bool check = true;
    Checkbox("Show direcitons##2", &check);

    if (check) {
      const vec3& right = cameraGUI.arcball->getRight();
      const vec3& up = cameraGUI.arcball->getUp();

      drawDirection(drawList, right, IM_RED, 0.f);
      drawDirection(drawList, up, IM_GREEN, (IM_CIRCLE_RADIUS + 5.f) * 2.f);
      drawDirection(drawList, normalize(cross(right, up)), IM_BLUE, (IM_CIRCLE_RADIUS + 5.f) * 4.f);
      Dummy(ImVec2(0.f, IM_CIRCLE_RADIUS * 2.f));
    }
  }

  // ================== Free Camera ====================

  SeparatorText("Free Camera");
  if (!cameraGUI.free) error("The free camera is not linked to gui");

  // Show directions
  {
    static bool check = true;
    Checkbox("Show direcitons##3", &check);

    if (check) {
      const vec3& right = cameraGUI.free->getRight();
      const vec3& up = cameraGUI.free->getUp();

      drawDirection(drawList, right, IM_RED, 0.f);
      drawDirection(drawList, up, IM_GREEN, (IM_CIRCLE_RADIUS + 5.f) * 2.f);
      drawDirection(drawList, normalize(cross(right, up)), IM_BLUE, (IM_CIRCLE_RADIUS + 5.f) * 4.f);
      Dummy(ImVec2(0.f, IM_CIRCLE_RADIUS * 2.f));
    }
  }

  End();
}

} // namespace gui
