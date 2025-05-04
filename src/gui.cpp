#include "gui.hpp"

#include "imgui.h"

#define IM_RED   IM_COL32(255, 0  , 0  , 255)
#define IM_GREEN IM_COL32(0  , 255, 0  , 255)
#define IM_BLUE  IM_COL32(0  , 0  , 255, 255)
#define IM_WHITE IM_COL32(255, 255, 255, 255)

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
  float arcballDistance;
};

struct AirplaneGUI {
  Airplane* ptr;
  float scale = 1.f;
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

void drawDirection(ImDrawList* drawList, const vec2& dir, const ImU32& color, float offsetX) {
  ImVec2 pos = GetCursorScreenPos();
  pos.x += IM_CIRCLE_RADIUS + offsetX;
  pos.y += IM_CIRCLE_RADIUS;
  ImVec2 p1{pos.x, pos.y};
  ImVec2 p2{pos.x + dir.x * IM_CIRCLE_RADIUS, pos.y + dir.y * IM_CIRCLE_RADIUS};

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

  if (SliderFloat("Heightmap scale", &planetGUI.heighmapScale, 0.01f, 10.f))
    planetGUI.ptr->setHeightmapScale(planetGUI.heighmapScale);

  if (SliderFloat("Sea level", &planetGUI.seaLevel, -10.f, 10.f)) //
    planetGUI.ptr->setSeaLevel(planetGUI.seaLevel);

  if (Button("Apply")) {
    if (!planetGUI.ptr) error("The planet is not linked to gui");
    else planetGUI.ptr->rebuild(static_cast<u16>(planetGUI.res), static_cast<float>(planetGUI.radius));
  }

  // ================== Airplane =======================

  SeparatorText("Airplane");
  if (!airplaneGUI.ptr) error("The airplane is not linked to gui");

  if (SliderFloat("Scale", &airplaneGUI.scale, 0.01f, 10.f))
    airplaneGUI.ptr->setScale(glm::scale(mat4(1.f), vec3(airplaneGUI.scale)));

  // ================== Airplane Camera ================

  SeparatorText("Airplane Camera");
  if (!cameraGUI.arcball) error("The arcball camera is not linked to gui");

  cameraGUI.arcballDistance = cameraGUI.arcball->getDistance();
  if (SliderFloat("Distance", &cameraGUI.arcballDistance, 1.f, 10.f))
    cameraGUI.arcball->setDistance(cameraGUI.arcballDistance);

  // ================== Free Camera ====================

  SeparatorText("Free Camera");
  if (!cameraGUI.free) error("The free camera is not linked to gui");

  End();
}

} // namespace gui
