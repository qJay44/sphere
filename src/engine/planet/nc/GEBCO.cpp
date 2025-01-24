#include "GEBCO.hpp"

#include "netcdf.h"

GEBCO::GEBCO(const fspath& path, bool print)
  : File(path, print),
    longitudes(getDimensionSize(0)),
    latitudes(getDimensionSize(1)),
    stepLon((2.f * PI) / longitudes),
    stepLat(PI / latitudes) {}

const u32& GEBCO::getLongitudes() const { return longitudes; }
const u32& GEBCO::getLatitudes() const { return latitudes; }

short GEBCO::elevation(float lat, float lon) const {
  size_t col = static_cast<size_t>((lat + PI_2) / stepLat) - 1;
  size_t row = static_cast<size_t>((lon + PI) / stepLon) - 1;
  size_t idx[2] = {col, row};
  short val;

  if ((status = nc_get_var1(ncid, 3, idx, &val))) {
    const char* msg = "lon: %f, lat: %f\ncol: %d, row: %d\nlons: %d, lats: %d\n";
    printf(msg, lon, lat, col, row, longitudes, latitudes);
    error(status);
  }

  return val;
}
