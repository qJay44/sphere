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

static bool collapsed = true;

struct PlanetGUI {
  Planet* ptr;
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

void gui::link(Planet* ptr)         { planetGUI.ptr     = ptr; }
void gui::link(AirplaneCamera* ptr) { cameraGUI.arcball = ptr; }
void gui::link(Camera* ptr)         { cameraGUI.free    = ptr; }
void gui::link(Airplane* ptr)       { airplaneGUI.ptr   = ptr; }

void gui::toggle() { collapsed = !collapsed; }

void gui::draw() {
  static RunOnce a([]() {
    SetNextWindowSize({400, 500});
    SetNextWindowPos({0, 0});
  });
  SetNextWindowCollapsed(collapsed);

  Begin("Settings");

  ImDrawList* drawList = GetWindowDrawList();

  // ================== Planet ==================

  SeparatorText("Planet");

  if (!planetGUI.ptr) error("The planet is not linked to gui");

  SliderInt("Resolution", &planetGUI.ptr->resolution, 2, 1000);
  SliderFloat("Radius", &planetGUI.ptr->radius, 1.f, 100.f);
  SliderFloat("Heightmap scale", &planetGUI.ptr->heightmapScale, 0.01f, 10.f);
  SliderFloat("Sea level", &planetGUI.ptr->seaLevel, -10.f, 10.f);

  if (Button("Rebuild"))
    planetGUI.ptr->rebuild();

  // ================== Airplane =======================

  SeparatorText("Airplane");
  if (!airplaneGUI.ptr) error("The airplane is not linked to gui");

  static float prevScale = airplaneGUI.scale;
  if (SliderFloat("Scale", &airplaneGUI.scale, 0.01f, 10.f)) {
    float scaleFactor = airplaneGUI.scale / prevScale;
    airplaneGUI.ptr->scale(scaleFactor);
    prevScale = airplaneGUI.scale;
  }

  SliderFloat("Speed (radians)", &airplaneGUI.ptr->speedRad, 0.01f, 10.f);
  SliderFloat("Fly height", &airplaneGUI.ptr->flyHeight, 1.f, 10.f);

  // ================== Airplane Camera ================

  SeparatorText("Airplane Camera");
  if (!cameraGUI.arcball) error("The arcball camera is not linked to gui");

  SliderFloat("Distance", &cameraGUI.arcball->distance, 1.f, 10.f);

  // ================== Free Camera ====================

  SeparatorText("Free Camera");
  if (!cameraGUI.free) error("The free camera is not linked to gui");

  End();
}

