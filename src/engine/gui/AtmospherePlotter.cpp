#include "AtmospherePlotter.hpp"

#include "implot.h"

constexpr ImPlotAxisFlags flagsPlot =
  ImPlotFlags_NoInputs    |
  ImPlotFlags_NoMenus     |
  ImPlotFlags_NoBoxSelect |
  ImPlotFlags_NoFrame     |
  ImPlotFlags_NoLegend;

constexpr ImPlotAxisFlags flagsAxis =
  ImPlotAxisFlags_NoTickMarks  |
  ImPlotAxisFlags_NoMenus      |
  ImPlotAxisFlags_NoSideSwitch |
  ImPlotAxisFlags_Lock;

AtmospherePlotter::AtmospherePlotter(const PlanetAtmosphere* atmosphere, int points)
  : atmosphere(atmosphere), points(points)
{
  x_density.resize(points);
  y_density.resize(points);

  x_transmittance.resize(points);
  y1_transmittance.resize(points);
  y2_transmittance.resize(points);
  y3_transmittance.resize(points);
}

void AtmospherePlotter::renderDensity(float xmax, ImVec2 size) {
  if (ImPlot::BeginPlot("Density", size, flagsPlot)) {
    updateDensity(xmax);

    ImPlot::SetupAxes("Density falloff", "##EmptyTitle", flagsAxis, flagsAxis);
    ImPlot::SetupLegend(ImPlotLocation_NorthEast);

    ImPlot::SetNextLineStyle({1.f, 0.f, 0.f, 1.f}, 2.f);
    ImPlot::PlotLine("Red", x_density.data(), y_density.data(), points);

    ImPlot::EndPlot();
  }
}

void AtmospherePlotter::renderTransmittance(float xmax, ImVec2 size) {
  if (ImPlot::BeginPlot("Transmittance", size, flagsPlot)) {
    updateTranmittance(xmax);

    ImPlot::SetupAxes("Optical depth", "##EmptyTitle", flagsAxis, flagsAxis);
    ImPlot::SetupLegend(ImPlotLocation_NorthEast);

    ImPlot::SetNextLineStyle({1.f, 0.f, 0.f, 1.f}, 2.f);
    ImPlot::PlotLine("Red", x_transmittance.data(), y1_transmittance.data(), points);

    ImPlot::SetNextLineStyle({0.f, 1.f, 0.f, 1.f}, 2.f);
    ImPlot::PlotLine("Green", x_transmittance.data(), y2_transmittance.data(), points);

    ImPlot::SetNextLineStyle({0.f, 0.f, 1.f, 1.f}, 2.f);
    ImPlot::PlotLine("Blue", x_transmittance.data(), y3_transmittance.data(), points);

    ImPlot::EndPlot();
  }
}

void AtmospherePlotter::updateDensity(float xmax) {
  static float prev = 0xFFFFFF;

  const float& a = atmosphere->densityFalloff;

  if (prev == a)
    return;

  float step = xmax / (points - 1);

  for (int i = 0; i < points; i++) {
    float x = i * step;
    x_density[i] = x;
    y_density[i] = atmosphere->density(x, a);
  }

  prev = a;
}

void AtmospherePlotter::updateTranmittance(float xmax) {
  static float prev = 0xFFFFFF;

  const float& a = atmosphere->scatteringStrength;

  if (prev == a)
    return;

  float step = xmax / (points - 1);
  const vec3& b = atmosphere->scatteringCoefficients;

  for (int i = 0; i < points; i++) {
    float x = i * step;
    x_transmittance[i] = x;
    y1_transmittance[i] = atmosphere->transmittance(x, a, b.r);
    y2_transmittance[i] = atmosphere->transmittance(x, a, b.g);
    y3_transmittance[i] = atmosphere->transmittance(x, a, b.b);
  }

  prev = a;
}

