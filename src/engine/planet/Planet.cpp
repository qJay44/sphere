#include "Planet.hpp"

#include "netcdf.h"

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(1);}

Planet::Planet(u32 resolution) {
  static const vec3 directions[6]{
    {0.f,  1.f,  0.f }, // Up
    {0.f,  -1.f, 0.f }, // Down
    {-1.f, 0.f,  0.f }, // Left
    {1.f,  0.f,  0.f }, // Right
    {0.f,  0.f,  1.f }, // Front
    {0.f,  0.f,  -1.f}, // Rear
  };
  static const vec3 palette[6]{
    {0.004f, 0.745f, 0.996f},
    {1.f,    0.867f, 0.f   },
    {1.f,    0.49f,  0.f   },
    {1.f,    0.f,    0.427f},
    {0.678f, 1.f,    0.008f},
    {0.561f, 0.f,    1.f   },
  };

  for (int i = 0; i < 6; i++)
    terrainFaces[i] = TerrainFace(resolution, directions[i], palette[i]);
}

void Planet::readGeoData(const std::string& path) const {
  int ncid, varid;
  int dataIn[5][5];
  int x, y, retval;

  if ((retval = nc_open(path.c_str(), NC_NOWRITE, &ncid))) ERR(retval);

  if ((retval = nc_close(ncid))) ERR(retval);
}

void Planet::draw(const Camera& camera, const Shader& shader) const {
  for (const TerrainFace& face : terrainFaces) {
    face.draw(camera, shader);
  }
}
