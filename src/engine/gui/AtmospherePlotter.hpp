#pragma once

#include <vector>

#include "../objects/PlanetAtmosphere.hpp"
#include "imgui.h"

class AtmospherePlotter {
public:
  AtmospherePlotter(const PlanetAtmosphere* atmosphere, int points);

  void renderDensity(float xmax = 5.f, ImVec2 size = {400.f, 200.f});
  void renderTransmittance(float xmax = 5.f, ImVec2 size = {400.f, 200.f});

private:
  const PlanetAtmosphere* atmosphere;
  int points;

  std::vector<float> x_density;
  std::vector<float> y_density;

  std::vector<float> x_transmittance;
  std::vector<float> y1_transmittance;
  std::vector<float> y2_transmittance;
  std::vector<float> y3_transmittance;

private:
  void updateDensity(float xmax);
  void updateTranmittance(float xmax);
};

