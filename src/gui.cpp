#include "gui.hpp"

#include <algorithm>
#include <cmath>

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

#define IM_RED   IM_COL32(255u, 0u  , 0u  , 255u)
#define IM_GREEN IM_COL32(0u ,  255u, 0u  , 255u)
#define IM_BLUE  IM_COL32(0u ,  0u  , 255u, 255u)
#define IM_WHITE IM_COL32(255u, 255u, 255u, 255u)

#define IM_CIRCLE_RADIUS 30.f

constexpr ImU32 IM_U32_2 = 2;
constexpr ImU32 IM_U32_1024 = 2 << 9;

using namespace ImGui;

static bool collapsed = true;

struct EarthGUI {
  Earth* ptr;
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

EarthGUI earthGUI;
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

void gui::link(Earth* ptr)          { earthGUI.ptr     = ptr; }
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

  if (!earthGUI.ptr) error("The earth object is not linked to gui");
  if (TreeNode("Planet")) {

    // +++++++++++++++ Resolution +++++++++++++++ //
    //
    Text("Resolution"); SameLine();

    static u8 resRoot = sqrt(earthGUI.ptr->resolution);
    if (ArrowButton("##left", ImGuiDir_Left)) {
      resRoot--;
      resRoot = std::max(resRoot, (u8)1);
      earthGUI.ptr->resolution = resRoot * resRoot;
    }
    SameLine();
    if (ArrowButton("##right", ImGuiDir_Right)) {
      resRoot++;
      earthGUI.ptr->resolution = resRoot * resRoot;
    }
    SameLine();

    earthGUI.ptr->resolution = std::clamp(earthGUI.ptr->resolution, 2u, 2048u);
    Text("%d", earthGUI.ptr->resolution);

    // +++++++++++++++ Chunks +++++++++++++++++++ //

    Text("Chunks    "); SameLine();

    if (ArrowButton("##left##2", ImGuiDir_Left))   earthGUI.ptr->chunks >>= 1; SameLine();
    if (ArrowButton("##right##2", ImGuiDir_Right)) earthGUI.ptr->chunks <<= 1; SameLine();

    earthGUI.ptr->chunks = std::clamp(earthGUI.ptr->chunks, 2u, 1024u);    SameLine();
    Text("%d", earthGUI.ptr->chunks);

    // ++++++++++++++++++++++++++++++++++++++++++ //

    SliderFloat("Radius", &earthGUI.ptr->radius, 1.f, 100.f);
    SliderFloat("Heightmap scale", &earthGUI.ptr->heightmapScale, 0.01f, 100.f);
    SliderFloat("Light multiplier", &earthGUI.ptr->lightMultiplier, 0.1f, 20.f);
    ColorEdit3("Border color", glm::value_ptr(earthGUI.ptr->borderColor));

    Separator();
    if (Button("Rebuild"))
      earthGUI.ptr->rebuild();

    TreePop();
  }

  // ================== Airplane =======================

  if (!airplaneGUI.ptr) error("The airplane is not linked to gui");
  if (TreeNode("Airplane")) {
    SliderFloat("Speed", &airplaneGUI.ptr->speedRad, 0.f, PI * 2.f);
    SetItemTooltip("Radians");

    SliderFloat("Fly height", &airplaneGUI.ptr->flyHeight, 1.f, 100.f);
    SliderFloat("Turn speed", &airplaneGUI.ptr->turnSpeedRad, 0.f, 10.f);
    SetItemTooltip("Radians");

    SliderFloat("Turn momementum decrease factor", &airplaneGUI.ptr->turnMomentumDecreaseFactor, 0.1f, 0.99f);
    SetItemTooltip("How long the momementum is decreasing");
    SliderFloat("Tilt momementum decrease factor", &airplaneGUI.ptr->tiltMomentumDecreaseFactor, 0.1f, 0.99f);
    SetItemTooltip("How long the momementum is decreasing");
    SliderFloat("Tilt recover momementum decrease factor", &airplaneGUI.ptr->tiltRecoverMomentumDecreaseFactor, 0.1f, 0.99f);
    SetItemTooltip("How long the momementum is decreasing");

    static float prevScale = airplaneGUI.scale;
    if (SliderFloat("Scale", &airplaneGUI.scale, 0.01f, 10.f)) {
      float scaleFactor = airplaneGUI.scale / prevScale;
      airplaneGUI.ptr->scale(scaleFactor);
      prevScale = airplaneGUI.scale;
    }

    TreePop();
  }

  // ================== Airplane Camera ================

  if (!cameraGUI.arcball) error("The arcball camera is not linked to gui");
  if (TreeNode("Airplane Camera")) {
    SliderFloat("Near",     &cameraGUI.arcball->nearPlane, 0.01f, 1.f);
    SliderFloat("Far",      &cameraGUI.arcball->farPlane,  10.f , 100.f);
    SliderFloat("Distance", &cameraGUI.arcball->distance,  1.f  , 50.f);
    SliderFloat("FOV",      &cameraGUI.arcball->fov,       45.f , 179.f);

    TreePop();
  }

  // ================== Free Camera ====================

  if (!cameraGUI.free) error("The free camera is not linked to gui");
  if (TreeNode("Free camera")) {
    SliderFloat("Near##2",  &cameraGUI.free->nearPlane, 0.01f, 1.f);
    SliderFloat("Far##2",   &cameraGUI.free->farPlane,  10.f , 100.f);
    SliderFloat("Speed##2", &cameraGUI.free->speed,     1.f  , 50.f);
    SliderFloat("FOV##2",   &cameraGUI.free->fov,       45.f , 179.f);

    TreePop();
  }

  // ================== Light ==========================

  if (!lightGUI.ptr) error("The light is not linked to gui");
  if (TreeNode("Light")) {
    bool update = false;
    update += DragFloat("x", &lightGUI.ptr->position.x, 0.1f);
    update += DragFloat("y", &lightGUI.ptr->position.y, 0.1f);
    update += DragFloat("z", &lightGUI.ptr->position.z, 0.1f);

    if (update)
      lightGUI.ptr->translation = glm::translate(mat4(1.f), lightGUI.ptr->position);

    ColorEdit3("Color", glm::value_ptr(lightGUI.ptr->color));

    TreePop();
  };

  // ================== Other ==========================

  if (TreeNode("Other")) {
    Checkbox("Show global axis", &global::drawGlobalAxis);
    Checkbox("Planet print build info", &earthGUI.ptr->printBuildInfo);

    TreePop();
  }

  End();
}

