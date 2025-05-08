#include "gui.hpp"

#include "imgui.h"

#define IM_RED   IM_COL32(255u, 0u  , 0u  , 255u)
#define IM_GREEN IM_COL32(0u ,  255u, 0u  , 255u)
#define IM_BLUE  IM_COL32(0u ,  0u  , 255u, 255u)
#define IM_WHITE IM_COL32(255u, 255u, 255u, 255u)

#define IM_CIRCLE_RADIUS 30.f

constexpr ImU32 IM_U32_2 = 2;
constexpr ImU32 IM_U32_1024 = 2 << 9;

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

struct LightGUI {
  Light* ptr;
};

PlanetGUI planetGUI;
CameraGUI cameraGUI;
AirplaneGUI airplaneGUI;
LightGUI lightGUI;

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
void gui::link(Light* ptr)          { lightGUI.ptr      = ptr; }

void gui::toggle() { collapsed = !collapsed; }

void gui::draw() {
  static RunOnce a([]() {
    SetNextWindowPos({0, 0});
  });
  SetNextWindowCollapsed(collapsed);

  Begin("Settings");

  ImDrawList* drawList = GetWindowDrawList();

  // ================== Planet =========================

  SeparatorText("Planet");

  if (!planetGUI.ptr) error("The planet is not linked to gui");
  {
    Text("Resolution");
    static int rbResolution = planetGUI.ptr->resolution;
    for (u32 i = 0; i < 9; i++) {
      int num = 4 << i;
      RadioButton(std::to_string(num).c_str(), &rbResolution, num);
      if (i != 8) SameLine();
    }
    planetGUI.ptr->resolution = rbResolution;

    Text("Chunks");
    static int rbChunks = planetGUI.ptr->chunks;
    for (u32 i = 0; i < 9; i++) {
      int num = 4 << i;
      RadioButton((std::to_string(num) + "##2").c_str(), &rbChunks, num);
      if (i != 8) SameLine();
    }
    planetGUI.ptr->chunks = rbChunks;

    SliderFloat("Radius", &planetGUI.ptr->radius, 1.f, 100.f);
    SliderFloat("Heightmap scale", &planetGUI.ptr->heightmapScale, 0.01f, 10.f);
    SliderFloat("Sea level", &planetGUI.ptr->seaLevel, -10.f, 10.f);

    static int rbChunksColoring = planetGUI.ptr->colorChunksInsteadOfFaces;
    RadioButton("Color only faces", &rbChunksColoring, 0); SameLine();
    RadioButton("Color only chunks per face", &rbChunksColoring, 1);
    planetGUI.ptr->colorChunksInsteadOfFaces = rbChunksColoring;

    if (Button("Rebuild"))
      planetGUI.ptr->rebuild();
  }

  // ================== Airplane =======================

  SeparatorText("Airplane");
  if (!airplaneGUI.ptr) error("The airplane is not linked to gui");
  {
    static float prevScale = airplaneGUI.scale;
    if (SliderFloat("Scale", &airplaneGUI.scale, 0.01f, 10.f)) {
      float scaleFactor = airplaneGUI.scale / prevScale;
      airplaneGUI.ptr->scale(scaleFactor);
      prevScale = airplaneGUI.scale;
    }

    SliderFloat("Speed (radians)", &airplaneGUI.ptr->speedRad, 0.f, 10.f);
    SliderFloat("Fly height", &airplaneGUI.ptr->flyHeight, 1.f, 10.f);
    SliderFloat("Turn speed (radians)", &airplaneGUI.ptr->turnSpeedRad, 0.f, 10.f);
  }

  // ================== Airplane Camera ================

  SeparatorText("Airplane Camera");
  if (!cameraGUI.arcball) error("The arcball camera is not linked to gui");
  {
    SliderFloat("Near",     &cameraGUI.arcball->nearPlane, 0.01f, 1.f);
    SliderFloat("Far",      &cameraGUI.arcball->farPlane,  10.f , 100.f);
    SliderFloat("Distance", &cameraGUI.arcball->distance,  1.f  , 10.f);
    SliderFloat("FOV",      &cameraGUI.arcball->fov,       45.f , 179.f);
  }

  // ================== Free Camera ====================

  SeparatorText("Free Camera");
  if (!cameraGUI.free) error("The free camera is not linked to gui");
  {
    SliderFloat("Near##2",  &cameraGUI.free->nearPlane, 0.01f, 1.f);
    SliderFloat("Far##2",   &cameraGUI.free->farPlane,  10.f , 100.f);
    SliderFloat("Speed##2", &cameraGUI.free->speed,     1.f  , 50.f);
    SliderFloat("FOV##2",   &cameraGUI.free->fov,       45.f , 179.f);
  }

  // ================== Light ==========================

  SeparatorText("Light");
  if (!lightGUI.ptr) error("The light is not linked to gui");
  {
    const vec3& pos = lightGUI.ptr->position;
    static vec3 posGui = pos;

    DragFloat("x", &posGui.x, 0.1f);
    DragFloat("y", &posGui.y, 0.1f);
    DragFloat("z", &posGui.z, 0.1f);

    lightGUI.ptr->translate(posGui - pos * glm::sign(posGui));
    lightGUI.ptr->position = posGui;

    ColorEdit3("Color", (float*)&lightGUI.ptr->color);
  };

  End();
}

