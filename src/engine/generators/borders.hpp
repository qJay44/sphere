#pragma once

#include "../texture/Texture2D.hpp"
#include "../shapefile/Polygon.hpp"
#include "../mesh/BufferObject.hpp"
#include "../Shader.hpp"

namespace generators {

[[nodiscard]]
static Texture2D generateTextureBorders(const shapefile::ShapefileReader& shapefile, ivec2 resolution = ivec2(2048, 1024)) {
  static Shader shader("generators/borders.comp");

  // ----- Allocate buffers ---------------------------------------------------------------------------------------- //

  auto countriesPolygons = shapefile::Polygon::getPolygons(shapefile);

  struct alignas(16) Metadata {
    vec4 bounds; // Xmin, Ymin, Xmax, Ymax
    int pointBufferOffset;
    int numPoints;
    vec2 _pad;
  };

  struct {
    BufferObject meta{GL_SHADER_STORAGE_BUFFER};
    BufferObject points{GL_SHADER_STORAGE_BUFFER};
  } ssbo;

  std::vector<Metadata> dataMeta;
  std::vector<vec2> dataPoints;
  int globalPointOffset = 0;

  for (const auto& country : countriesPolygons) {
    const auto& box = country.box;
    vec4 bounds{
      box.Xmin,
      box.Ymin,
      box.Xmax,
      box.Ymax
    };

    s32 numParts = country.parts.size();
    for (s32 i = 0; i < numParts; i++) {
      s32 startIdx = country.parts[i];
      s32 endIdx = i + 1 < numParts ? country.parts[i + 1] : country.points.size();
      s32 partLength = endIdx - startIdx;

      Metadata meta{};
      meta.bounds = bounds;
      meta.pointBufferOffset = globalPointOffset;
      meta.numPoints = partLength;

      for (s32 j = startIdx; j < endIdx; j++)
        dataPoints.push_back({
          country.points[j].x,
          country.points[j].y
        });

      dataMeta.push_back(meta);
      globalPointOffset += partLength;
    }
  }

  ssbo.meta.storage(dataMeta.data(), dataMeta.size() * sizeof(Metadata), 0);
  ssbo.points.storage(dataPoints.data(), dataPoints.size() * sizeof(vec2), 0);

  // ----- Allocate texture ---------------------------------------------------------------------------------------- //

  TextureDescriptor desc{};
  desc.internalFormat = GL_R16F;
  desc.format = GL_RED;

  Texture2D texture(resolution, desc);
  uvec2 numWorkGruops{(resolution + 7) / 8};

  ssbo.meta.bindBase(0);
  ssbo.points.bindBase(1);

  shader.use();
  glBindImageTexture(0, texture.getId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16F);
  glDispatchCompute(numWorkGruops.x, numWorkGruops.y, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  texture.bind(0);

  return texture;
}

} // namespace generators

