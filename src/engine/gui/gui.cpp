#include "gui.hpp"

#include "AtmospherePlotter.hpp"
#include "TilePlotter.hpp"
#include "glm/gtc/type_ptr.hpp"

#undef IM_NEW
#undef IM_FREE

#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "global.hpp"

using namespace ImGui;

static bool configCollapsed = true;
static bool infoCollapsed = true;

Earth* gui::earthPtr = nullptr;
Camera* gui::camSpecatePtr = nullptr;
Airplane* gui::airplanePtr = nullptr;
Light* gui::lightPtr = nullptr;

float gui::_sliderf0 = 0.f;
float gui::_sliderf1 = 0.f;
float gui::_sliderf2 = 0.f;
int gui::_slideri0 = 0.f;

u16 gui::fps = 1;

void gui::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void gui::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void gui::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}

void gui::init() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplGlfw_InitForOpenGL(global::window, true);
  ImGui_ImplOpenGL3_Init();
}

void gui::toggleConfig() { configCollapsed = !configCollapsed; }
void gui::toggleInfo() { infoCollapsed = !infoCollapsed; }

void gui::draw() {
  auto _task = global::profiler->startScopedTask("gui::draw");

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  SetNextWindowPos({0, 0});
  SetNextWindowCollapsed(configCollapsed);

  // ::::: Config window ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  Begin("Config");

  // ===== Planet ======================================================================================== //

  bool rebakeOpticalDepth = false;

  assert(earthPtr);
  if (CollapsingHeader("Planet")) {
    bool rebuild = SliderFloat("Radius", &earthPtr->radius, 1.f, 500.f);
    rebakeOpticalDepth |= rebuild;

    Checkbox("Frustum culling", &earthPtr->useFrustum);

    static int rbFaceChunk = 0;
    if (RadioButton("Default"     , rbFaceChunk == 0)) rbFaceChunk = 0;
    if (RadioButton("Color faces" , rbFaceChunk == 1)) rbFaceChunk = 1;
    if (RadioButton("Color chunks", rbFaceChunk == 2)) rbFaceChunk = 2;

    earthPtr->useTerrainFaceColors = rbFaceChunk == 1;
    earthPtr->useTerrainFaceChunkColors = rbFaceChunk == 2;

    rebuild |= SliderInt("Chunks per side", &earthPtr->chunksPerSide, 1, 20);
    rebuild |= SliderInt("Resolution", &earthPtr->resolution, 2, 100);

    if (rebuild)
      earthPtr->build();

    Spacing();
    SliderFloat("TESC divisions scale", &earthPtr->tessDivs, 0.f, 100.f);
    SliderFloat("Heightmap scale", &earthPtr->heightmapScale, 0.01f, 100.f);
    SliderFloat("Triplanar blend sharpness", &earthPtr->triplanarBlendSharpness, 1.f, 10.f);
    SliderFloat("Sea level", &earthPtr->seaLevel, -1.f, 1.f);
    ColorEdit3("Border color", glm::value_ptr(earthPtr->bordersColor));

    SeparatorText("Water");
    SliderFloat("Deep factor", &earthPtr->waterDeepFactor, -1.f, 1.f);
    SliderFloat("Wave frequency", &earthPtr->waterWaveFreq, -0.5f, 0.5f);
    SliderFloat("Wave resolution scale", &earthPtr->waterWaveResScale, 0.1f, 1.f);
    SliderFloat("Shore frequency", &earthPtr->waterShoreFreq, 0.f, 300.f);
    SliderFloat("Shore scale", &earthPtr->waterShoreScale, 0.f, 300.f);
    SliderFloat("Shore noise frequency", &earthPtr->waterShoreNoiseFreq, 0.f, 1.f);
    SliderFloat("Shore noise scale", &earthPtr->waterShoreNoiseScale, 0.f, 1.f);
    SliderFloat("Shore noise strength", &earthPtr->waterShoreNoiseStrength, 0.f, 1000.f);
    SliderFloat("Shore mask blend", &earthPtr->waterShoreMaskBlend, 0.f, 1.f);
    SliderFloat("Shore width", &earthPtr->waterShoreWidth, 0.f, 1.f);
    SliderFloat("Shore edge blend", &earthPtr->waterShoreEdgeBlend, 0.f, 1.f);
    ColorEdit3("Shallow color", glm::value_ptr(earthPtr->waterShallowColor));
    ColorEdit3("Deep color", glm::value_ptr(earthPtr->waterDeepColor));

    SeparatorText("Lightning");
    SliderFloat("Multiplier", &earthPtr->lightMultiplier, 0.1f, 20.f);
    SliderFloat("Distance dim scale", &earthPtr->lightDimScale, 0.1f, 20.f);
    SliderFloat("Ambient", &earthPtr->ambient, 0.0f, 20.f);
  }

  // ===== Atmosphere ==================================================================================== //

  if (CollapsingHeader("Atmosphere")) {
    PlanetAtmosphere& atmosphere = earthPtr->atmosphere;
    static AtmospherePlotter atmospherePlotter(&earthPtr->atmosphere, 100); // Always valid pointer?

    SeparatorText("Atmosphere");
    rebakeOpticalDepth |= SliderFloat("Radius##2", &atmosphere.radius, earthPtr->radius, 1000.f);
    rebakeOpticalDepth |= SliderInt("Scattering points", &atmosphere.scatteringPoints, 2, 50);
    rebakeOpticalDepth |= SliderInt("Optical depth points", &atmosphere.opticalDepthPoints, 2, 50);
    rebakeOpticalDepth |= SliderFloat("Sun intensity", &atmosphere.sunIntensity, 0.f, 10.f);
    rebakeOpticalDepth |= SliderFloat("Density falloff", &atmosphere.densityFalloff, 0.f, 20.f);
    rebakeOpticalDepth |= SliderFloat("Scattering strength", &atmosphere.scatteringStrength, 0.f, 1.f);

    Checkbox("Gamma correction", &atmosphere.useGammaCorrection);
    Checkbox("Apply", &atmosphere.apply);

    atmospherePlotter.renderDensity(1.f);
    atmospherePlotter.renderTransmittance(5.f);

    if (rebakeOpticalDepth)
      earthPtr->bakeOpticalDepth();
  }

  // ===== Airplane ====================================================================================== //

  assert(airplanePtr);
  if (CollapsingHeader("Airplane")) {
    SliderFloat("Speed", &airplanePtr->speedDefault, 0.f, PI * 2.f);
    SetItemTooltip("Radians");

    SliderFloat("Fly height", &airplanePtr->flyHeight, 1.f, 100.f);
    SliderFloat("Turn speed", &airplanePtr->turnSpeedRad, 0.f, 10.f);
    SetItemTooltip("Radians");

    SliderFloat("Turn momementum decrease factor", &airplanePtr->turnMomentumDecreaseFactor, 0.1f, 0.99f);
    SetItemTooltip("How long the momementum is decreasing");
    SliderFloat("Tilt momementum decrease factor", &airplanePtr->tiltMomentumDecreaseFactor, 0.1f, 0.99f);
    SetItemTooltip("How long the momementum is decreasing");
    SliderFloat("Tilt recover momementum decrease factor", &airplanePtr->tiltRecoverMomentumDecreaseFactor, 0.1f, 0.99f);
    SetItemTooltip("How long the momementum is decreasing");

    if (SliderFloat("Scale", &airplanePtr->meshScale, 0.01f, 10.f))
      airplanePtr->setScale(airplanePtr->meshScale);

    SeparatorText("Trail");
    SliderFloat3("Trails offset", glm::value_ptr(airplanePtr->trailOffset), -10000.f, 10000.f);
    SliderFloat("Width", &airplanePtr->trailLeft.width, 0.f, 0.5f);
    SliderFloat("Duration", &airplanePtr->trailLeft.duration, 0.f, 10.f);

    if (TreeNode("Flags")) {
      CheckboxFlags("Right", &airplanePtr->flags, AirplaneFlags_DrawRight);
      CheckboxFlags("Up", &airplanePtr->flags, AirplaneFlags_DrawUp);
      CheckboxFlags("Forward", &airplanePtr->flags, AirplaneFlags_DrawForward);
      CheckboxFlags("Frustum", &airplanePtr->flags, AirplaneFlags_DrawFrustum);

      TreePop();
    }
  }

  // ===== Airplane Camera =============================================================================== //

  Camera& camAirplane = airplanePtr->getCamera();

  if (CollapsingHeader("Airplane Camera")) {
    SliderFloat("Near", &camAirplane.nearPlane, 0.01f, 1.f);
    SliderFloat("Far", &camAirplane.farPlane, 10.f, 1000.f);
    SliderFloat("Distance", &airplanePtr->camDistance, 1.f, 50.f);
    SliderFloat("FOV", &camAirplane.fov, 45.f, 179.f);

    if (TreeNode("Flags")) {
      CheckboxFlags("Right", &camAirplane.flags, CameraFlags_DrawRight);
      CheckboxFlags("Up", &camAirplane.flags, CameraFlags_DrawUp);
      CheckboxFlags("Forward", &camAirplane.flags, CameraFlags_DrawForward);

      TreePop();
    }
  }

  // ===== Spectate camera =============================================================================== //

  assert(camSpecatePtr);
  if (CollapsingHeader("Spectate camera")) {
    SliderFloat("Near##2", &camSpecatePtr->nearPlane, 0.01f, 1.f);
    SliderFloat("Far##2", &camSpecatePtr->farPlane,  10.f, 1000.f);
    SliderFloat("Speed##2", &camSpecatePtr->speedDefault, 1.f, 50.f);
    SliderFloat("FOV##2", &camSpecatePtr->fov, 45.f, 179.f);
    DragFloat("Yaw##2", &camSpecatePtr->yaw);
    DragFloat("Pitch##2", &camSpecatePtr->pitch);
    DragFloat3("Position", glm::value_ptr(camSpecatePtr->position));

    if (TreeNode("Flags")) {
      CheckboxFlags("Right", &camSpecatePtr->flags, CameraFlags_DrawRight);
      CheckboxFlags("Up", &camSpecatePtr->flags, CameraFlags_DrawUp);
      CheckboxFlags("Forward", &camSpecatePtr->flags, CameraFlags_DrawForward);
      CheckboxFlags("Frustum", &camSpecatePtr->flags, CameraFlags_DrawFrustum);

      TreePop();
    }
  }

  // ===== Light ========================================================================================= //

  assert(lightPtr);
  if (CollapsingHeader("Light")) {
    DragFloat3("Position", glm::value_ptr(lightPtr->position));
    DragFloat("Radius", &lightPtr->radius, 1.f, 0.f);
    ColorEdit3("Color", glm::value_ptr(lightPtr->color));
  };

  // ===== Other ========================================================================================= //

  if (CollapsingHeader("Other")) {
    Checkbox("Show global axis", &global::drawGlobalAxis);
    Checkbox("Planet print build info", &earthPtr->printBuildInfo);

    DragFloat("_sliderf0", &_sliderf0);
    DragFloat("_sliderf1", &_sliderf1);
    DragFloat("_sliderf2", &_sliderf2);
    DragInt("_slideri0", &_slideri0);
  }

  End();

  _task.end();

  // ::::: Info window ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  const ImGuiViewport* viewport = GetMainViewport();
  ImVec2 posBR = viewport->WorkPos + viewport->WorkSize;

  SetNextWindowPos(posBR, ImGuiCond_Always, {1.f, 1.f});
  SetNextWindowCollapsed(infoCollapsed);

  Begin("Info");

  ImGui::Text("FPS: %d / %f.5 ms", fps, global::dt);

  static TilePlotter tilePlotter(earthPtr->chunksDrawn, earthPtr->tileManager.tilesLoaded, earthPtr->tileManager.caps.tileSlots);
  tilePlotter.render();

  assert(global::profiler);
  global::profiler->renderTasks(400, 200, 200, 0);

  End();

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void gui::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
}

