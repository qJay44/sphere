#include "image2D.hpp"
#include <format>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

image2D::image2D() {}

image2D::image2D(const fspath& path) { load(path); }

image2D::~image2D() {
  if (stbiLoad) stbi_image_free(pixels);
  else if (pixels) error("image2D is not nullptr");
}

void image2D::load(const fspath& path) {
  int w, h, colorChannels;
  stbi_set_flip_vertically_on_load(true);
  pixels = stbi_load(path.string().c_str(), &w, &h, &colorChannels, 0);
  if (!pixels)
    error(std::format("stb didn't load the image: {}", path.string()));

  width = static_cast<u16>(w);
  height = static_cast<u16>(h);
  channels = static_cast<u16>(colorChannels);
  name = path.string();
  stbiLoad = true;
}

