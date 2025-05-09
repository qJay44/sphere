#pragma once

struct image2D {
  static void write(const std::string& path, uvec2 size, u8 channels, byte* buf);

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
