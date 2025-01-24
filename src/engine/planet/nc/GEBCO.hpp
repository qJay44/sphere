#pragma once

#include "File.hpp"

class GEBCO : public nc::File {
public:
  GEBCO(const fspath& path, bool print = false);

  const u32& getLongitudes() const;
  const u32& getLatitudes() const;

  short elevation(float lon, float lat) const;

private:
  const u32 longitudes;
  const u32 latitudes;
  const float stepLon;
  const float stepLat;

private:
};
