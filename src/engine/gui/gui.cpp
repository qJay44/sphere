#include "gui.hpp"

#include <algorithm>
#include <cmath>

#include "AtmospherePlotter.hpp"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"

using namespace ImGui;

static bool collapsed = true;

Earth* gui::earthPtr = nullptr;
Camera* gui::camSpecatePtr = nullptr;
Airplane* gui::airplanePtr = nullptr;
Light* gui::lightPtr = nullptr;

u16 gui::fps = 1;

void gui::toggle() { collapsed = !collapsed; }

void gui::draw() {
  static RunOnce a([]() {
    SetNextWindowPos({0, 0});
  });
  SetNextWindowCollapsed(collapsed);

  Begin("Settings");

  ImGui::Text("FPS: %d / %f.5 ms", fps, global::dt);

  // ===== Planet ======================================================================================== //

  if (!earthPtr) error("The earth object is not linked to gui");
  if (CollapsingHeader("Planet")) {

    // +++++++++++++++ Resolution +++++++++++++++ //

    Text("Resolution"); SameLine();

    static u8 resRoot = sqrt(earthPtr->resolution);
    if (ArrowButton("##left", ImGuiDir_Left)) {
      resRoot--;
      resRoot = std::max(resRoot, (u8)1);
      earthPtr->resolution = resRoot * resRoot;
    }
    SameLine();
    if (ArrowButton("##right", ImGuiDir_Right)) {
      resRoot++;
      earthPtr->resolution = resRoot * resRoot;
    }
    SameLine();

    earthPtr->resolution = std::clamp(earthPtr->resolution, 2u, 2048u);
    Text("%d", earthPtr->resolution);

    // +++++++++++++++ Chunks +++++++++++++++++++ //

    Text("Chunks    "); SameLine();

    if (ArrowButton("##left##2", ImGuiDir_Left))   {earthPtr->chunks >>= 1;} SameLine();
    if (ArrowButton("##right##2", ImGuiDir_Right)) {earthPtr->chunks <<= 1;} SameLine();

    earthPtr->chunks = std::clamp(earthPtr->chunks, 2u, 1024u);    SameLine();
    Text("%d", earthPtr->chunks);

    // ++++++++++++++++++++++++++++++++++++++++++ //

    SliderFloat("Radius", &earthPtr->radius, 1.f, 500.f);
    SliderFloat("Heightmap scale", &earthPtr->heightmapScale, 0.01f, 100.f);
    SliderFloat("Triplanar blend sharpness", &earthPtr->triplanarBlendSharpness, 1.f, 10.f);
    ColorEdit3("Border color", glm::value_ptr(earthPtr->bordersColor));

    SeparatorText("Water");
    SliderFloat("Deep factor", &earthPtr->waterDeepFactor, -50.f, 50.f);
    SliderFloat("Deep edge start", &earthPtr->waterDeepEdgeStart, 0.f, 1.f);
    SliderFloat("Deep edge end", &earthPtr->waterDeepEdgeEnd, 0.f, 1.f);
    SliderFloat("Specular smoothness", &earthPtr->waterSpecularSmoothness, 0.f, 10.f);
    SliderFloat("Wave frequency", &earthPtr->waterWaveFreq, -0.5f, 0.5f);
    SliderFloat("Wave resolution multiplier", &earthPtr->waterWaveResMult, 0.001f, 50.f);
    SliderFloat("Shore wave frequency", &earthPtr->waterShoreWaveFreq, -10000.f, 10000.f);
    SliderFloat("Shore wave distance threshold start", &earthPtr->waterShoreWaveThresholdStart, 0.f, 1.f);
    SliderFloat("Shore wave distance threshold end", &earthPtr->waterShoreWaveThresholdEnd, 0.f, 1.f);
    SliderFloat("Shore wave amplitude scale", &earthPtr->waterShoreWaveAmplitude, -1.f, 1.f);
    SliderFloat("Shore wave noise scale", &earthPtr->waterShoreWaveNoiseScale, 0.f, 10.f);
    SliderFloat("Shore wave noise speed", &earthPtr->waterShoreWaveNoiseSpeed, 0.f, 10.f);
    SliderFloat("Shore wave noise amplitude", &earthPtr->waterShoreWaveNoiseAmplitude, 0.f, 10.f);
    ColorEdit3("Shallow color", glm::value_ptr(earthPtr->waterShallowColor));
    ColorEdit3("Deep color", glm::value_ptr(earthPtr->waterDeepColor));

    SeparatorText("Lightning");
    SliderFloat("Light multiplier", &earthPtr->lightMultiplier, 0.1f, 20.f);
    SliderFloat("Ambient", &earthPtr->ambient, 0.0f, 20.f);
    SliderFloat("Specular light", &earthPtr->specularLight, 0.0f, 20.f);

    if (Button("Rebuild"))
      earthPtr->rebuild();
  }

  // ===== Atmosphere ==================================================================================== //

  if (CollapsingHeader("Atmosphere")) {
    PlanetAtmosphere& atmosphere = earthPtr->atmosphere;
    static AtmospherePlotter atmospherePlotter(&earthPtr->atmosphere, 100); // Always valid pointer?

    SeparatorText("Atmosphere");
    SliderFloat("Radius##2", &atmosphere.radius, earthPtr->radius, 500.f);
    SliderInt("Scattering points", &atmosphere.scatteringPoints, 2, 50);
    SliderInt("Optical depth points", &atmosphere.opticalDepthPoints, 2, 50);
    SliderFloat("Density falloff", &atmosphere.densityFalloff, 0.f, 20.f);
    SliderFloat("Scattering strenth", &atmosphere.scatteringStrength, 0.f, 20.f);

    atmospherePlotter.renderDensity(1.f);
    atmospherePlotter.renderTransmittance(5.f);
  }

  // ===== Airplane ====================================================================================== //

  if (!airplanePtr) error("The airplane is not linked to gui");
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
    SliderFloat("Duration", &airplanePtr->trailDuration, 0.f, 30.f);
    SliderFloat("Alpha factor", &airplanePtr->trailAlphaFactor, 0.f, 1.f);
    ColorEdit3("Color", glm::value_ptr(airplanePtr->trailLeft.color));
    airplanePtr->trailRight.color = airplanePtr->trailLeft.color;

    SeparatorText("Flags");
    Checkbox("Show right", &airplanePtr->showRight);
    Checkbox("Show up", &airplanePtr->showUp);
    Checkbox("Show forward", &airplanePtr->showForward);
  }

  // ===== Airplane Camera =============================================================================== //

  Camera& camAirplane = airplanePtr->getCamera();

  if (CollapsingHeader("Airplane Camera")) {
    SliderFloat("Near",     &camAirplane.nearPlane, 0.01f, 1.f);
    SliderFloat("Far",      &camAirplane.farPlane,  10.f , 1000.f);
    SliderFloat("Distance", &airplanePtr->camDistance,  1.f  , 50.f);
    SliderFloat("FOV",      &camAirplane.fov,       45.f , 179.f);
  }

  // ===== Spectate camera =============================================================================== //

  if (!camSpecatePtr) error("The spectate camera is not linked to gui");
  if (CollapsingHeader("Spectate camera")) {
    SliderFloat("Near##2",  &camSpecatePtr->nearPlane, 0.01f, 1.f);
    SliderFloat("Far##2",   &camSpecatePtr->farPlane,  10.f , 1000.f);
    SliderFloat("Speed##2", &camSpecatePtr->speed,     1.f  , 50.f);
    SliderFloat("FOV##2",   &camSpecatePtr->fov,       45.f , 179.f);
    DragFloat("Yaw##2", &camSpecatePtr->yaw);
    DragFloat("Pitch##2", &camSpecatePtr->pitch);
    DragFloat3("Position", glm::value_ptr(camSpecatePtr->position));
  }

  // ===== Light ========================================================================================= //

  if (!lightPtr) error("The light is not linked to gui");
  if (CollapsingHeader("Light")) {
    DragFloat3("Position", glm::value_ptr(lightPtr->position));
    DragFloat("Radius", &lightPtr->radius, 1.f, 0.f);
    ColorEdit3("Color", glm::value_ptr(lightPtr->color));
  };

  // ===== Other ========================================================================================= //

  if (CollapsingHeader("Other")) {
    Checkbox("Show global axis", &global::drawGlobalAxis);
    Checkbox("Planet print build info", &earthPtr->printBuildInfo);
  }

  End();
}

