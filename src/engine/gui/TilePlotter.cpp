#include "TilePlotter.hpp"

#undef IM_NEW
#undef IM_FREE

#include "implot.h"
#include "global.hpp"

constexpr ImPlotAxisFlags flagsPlot =
  ImPlotFlags_NoInputs    |
  ImPlotFlags_NoMenus     |
  ImPlotFlags_NoBoxSelect |
  ImPlotFlags_NoFrame;

constexpr ImPlotAxisFlags flagsAxis =
  ImPlotAxisFlags_NoTickMarks  |
  ImPlotAxisFlags_NoMenus      |
  ImPlotAxisFlags_NoSideSwitch |
  ImPlotAxisFlags_Lock         |
  ImPlotAxisFlags_NoTickLabels;

TilePlotter::TilePlotter(const int& chunksDrawn, const int& tilesLoaded,  const int& slotsLimit)
  : chunksDrawn(chunksDrawn),
    tilesLoaded(tilesLoaded),
    slotsLimit(slotsLimit)
{
}

void TilePlotter::render(vec2 size) {
  if (ImPlot::BeginPlot("Tile load and chunks drawn", size, flagsPlot)) {
    update();

    float t = global::time;

    ImPlot::SetupAxisLimits(ImAxis_X1, t - 5.f, t, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0.f, slotsLimit * 1.5f);
    ImPlot::SetupAxes("Time", "Count", flagsAxis, flagsAxis);
    ImPlot::SetupLegend(ImPlotLocation_NorthEast);

    ImPlot::SetNextLineStyle({0.f, 1.f, 0.f, 1.f}, 2.f);
    ImPlot::PlotLine("Chunks", &sDataChunks.data[0].x, &sDataChunks.data[0].y, sDataChunks.data.size(), 0, sDataChunks.offset, sizeof(vec2));

    ImPlot::SetNextLineStyle({0.f, 1.f, 1.f, 1.f}, 2.f);
    ImPlot::PlotLine("Tiles", &sDataTiles.data[0].x, &sDataTiles.data[0].y, sDataTiles.data.size(), 0, sDataTiles.offset, sizeof(vec2));

    ImPlot::SetNextLineStyle({1.f, 0.f, 0.f, 1.f}, 2.f);
    ImPlot::PlotLine("Slots", &sDataSlots.data[0].x, &sDataSlots.data[0].y, sDataSlots.data.size(), 0, sDataSlots.offset, sizeof(vec2));

    ImPlot::EndPlot();
  }
}

void TilePlotter::update() {
  sDataChunks.add(global::time, chunksDrawn);
  sDataTiles.add(global::time, tilesLoaded);
  sDataSlots.add(global::time, slotsLimit);
}

