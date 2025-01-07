#include <cmath>
class Sphere {
public:
  Sphere(float radius, u16 resolution) : radius(radius), resolution(resolution) {
    float stepLon = PI / resolution;
    float stepLat = PI_2 / resolution;

    for (int i = 0; i < resolution; i++) {
      float lon = i * stepLon - PI;
      for (int j = 0; j < resolution; j++) {
        float lat = j * stepLat - PI_2;
        float x = radius * sinf(lon) * cosf(lat);
        float y = radius * sinf(lon) * sinf(lat);
        float z = radius * cos(lon);
      }
    }
  };

private:
  float radius;
  u16 resolution;
};
