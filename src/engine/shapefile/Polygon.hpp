#pragma once

#include <bit>
#include <vector>

#include "Point.hpp"
#include "shapefile.hpp"
#include "utils/utils.hpp"

namespace shapefile {

struct Polygon {
  struct BoundingBox {
    double Xmin;
    double Ymin;
    double Xmax;
    double Ymax;
  } box;

  s32 numParts;
  s32 numPoints;
  std::vector<s32> parts;
  std::vector<Point> points;

  [[nodiscard]]
  static std::vector<Polygon> getPolygons(const ShapefileReader& shapefile) {
    shapefile_t shapeType = shapefile.getType();

    if (shapeType != shapefile::shapefile_t::Polygon)
      error("[shapefile::Polygon::getPolygon] The shapefile type [{}] is not Polygon", (s32)shapeType);

    s32 recordsSize = shapefile.getRecordsSizeInBytes();
    s32 recoredOffset = 0;
    std::vector<Polygon> polygons;

    while (recoredOffset < recordsSize) {
      const char* recordHeaderPtr = shapefile.getFirstRecordPtr() + recoredOffset;
      const char* recordContentPtr = recordHeaderPtr + 8;

      // s32 recordNumber = shapefile.toInteger(recordHeaderPtr, std::endian::big);
      s32 contentLength = shapefile.toInteger(recordHeaderPtr + 4, std::endian::big) * 2; // 16-bit word -> byte

      auto recordShapeType = (shapefile_t)shapefile.toInteger(recordContentPtr, std::endian::little);

      if (shapeType != recordShapeType)
        error("[shapefile::Polygon::getPolygon] The shapefile type in record [{}] differs from the expected [{}]", (s32)recordShapeType, (s32)shapeType);

      Polygon polygon;
      polygon.box.Xmin  = shapefile.toDouble (recordContentPtr + 4 , std::endian::little);
      polygon.box.Ymin  = shapefile.toDouble (recordContentPtr + 12, std::endian::little);
      polygon.box.Xmax  = shapefile.toDouble (recordContentPtr + 20, std::endian::little);
      polygon.box.Ymax  = shapefile.toDouble (recordContentPtr + 28, std::endian::little);
      polygon.numParts  = shapefile.toInteger(recordContentPtr + 36, std::endian::little);
      polygon.numPoints = shapefile.toInteger(recordContentPtr + 40, std::endian::little);

      const s32& numParts = polygon.numParts;
      const s32& numPoints = polygon.numPoints;

      polygon.parts.resize(numParts);
      polygon.points.resize(numPoints);

      const char* partsPtr = recordContentPtr + 44;
      const char* pointsPtr = partsPtr + numParts * 4;

      for (int i = 0; i < numParts; i++)
        polygon.parts[i] = shapefile.toInteger(partsPtr + i * 4, std::endian::little);

      for (s32 i = 0; i < numParts; i++) {
        s32 startPointIdx = polygon.parts[i];
        s32 endPointIdx = (i + 1 < numParts ? polygon.parts[i + 1] : numPoints);
        s32 currPartLength = endPointIdx - startPointIdx;

        for (s32 j = 0; j < currPartLength; j++) {
          s32 absolutePointIdx = startPointIdx + j;
          s32 pointOffset = absolutePointIdx * 16;

          Point& p = polygon.points[absolutePointIdx];
          p.x = shapefile.toDouble(pointsPtr + 0 + pointOffset, std::endian::little);
          p.y = shapefile.toDouble(pointsPtr + 8 + pointOffset, std::endian::little);
        }
      }

      polygons.push_back(std::move(polygon));
      recoredOffset += 8 + contentLength;
    }

    return polygons;
  }
};

} // namespace shapefile

