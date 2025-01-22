#include "GEBCO.hpp"

#include "glm/trigonometric.hpp"
#include "netcdf.h"

GEBCO::GEBCO(const fspath& path, bool print)
  : File(path, print),
    longitudes(getDimensionSize(0)),
    latitudes(getDimensionSize(1)),
    stepLon(360.f / longitudes),
    stepLat(180.f / latitudes) {}

const u32& GEBCO::getLongitudes() const { return longitudes; }
const u32& GEBCO::getLatitudes() const { return latitudes; }

short GEBCO::elevation(float lat, float lon, bool radians) const {
  if (radians) {
    lon = degrees(lon);
    lat = degrees(lat);
  }

  size_t col = static_cast<size_t>((lat + 90.f) / stepLat);
  size_t row = static_cast<size_t>((lon + 180.f) / stepLon);
  size_t idx[2] = {col, row};
  short val;

  if ((status = nc_get_var1(ncid, 3, idx, &val))) {
    const char* msg = "lon: %f, lat: %f\ndcol: %d, row: %d\nlons: %d, lats: %d\n";
    printf(msg, lon, lat, col, row, longitudes, latitudes);
    error(status);
  }

  return val;
}
