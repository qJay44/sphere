#pragma once

#include "ScrollingBuffer.hpp"

class TilePlotter {
public:
  TilePlotter(const int& chunksDrawn, const int& tilesLoaded, const int& slotsLimit);

  void render(vec2 size = {400.f, 200.f});

private:
  const int& chunksDrawn;
  const int& tilesLoaded;
  const int& slotsLimit;
  int points;

  ScrollingBuffer sDataChunks;
  ScrollingBuffer sDataTiles;
  ScrollingBuffer sDataSlots;

private:
  void update();
};

