#pragma once

struct image2D {
  image2D();
  image2D(const fspath& path);
  ~image2D();

  u16 width, height, channels;
  byte* pixels = nullptr;
  std::string name;

  void load(const fspath& path);

private:
  bool stbiLoad = false;
};
