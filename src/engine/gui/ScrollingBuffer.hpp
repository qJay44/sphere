#pragma once

#include <vector>

struct ScrollingBuffer {
  size_t maxSize;
  int offset = 0;
  std::vector<vec2> data;

  ScrollingBuffer(size_t maxSize = 2000) : maxSize(maxSize) {
    data.reserve(maxSize);
  }

  void add(float x, float y) {
    if (data.size() < maxSize)
      data.push_back({x, y});
    else {
      data[offset] = {x, y};
      offset = (offset + 1) % maxSize;
    }
  }

  void clear() {
    data.clear();
    offset = 0;
  }
};

