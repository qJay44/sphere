#version 460 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_col;
layout(location = 2) in vec2 in_tex;
layout(location = 3) in vec3 in_normal;

out vec3 color;
out vec2 texCoord;
out vec3 normal;

uniform mat4 model;
uniform mat4 cam;

uniform sampler2D heightmap0;
uniform sampler2D heightmap1;

uniform float heightmapScale;

#define PI 3.141592265359f

float heightmapValue() {
  float lon = 0.5f - atan(in_normal.z, in_normal.x) / (2.f * PI);
  float lat = 0.5f + asin(in_normal.y) / PI;
  float idx = round(lon);
  vec2 coord = vec2(lon, lat);
  coord.x = (coord.x - 0.5f) * 2.f * idx + coord.x * 2.f * (1.f - idx);

  float hms[2] = float[2](        //
    texture(heightmap0, coord).r, //
    texture(heightmap1, coord).r
  );

  return hms[uint(idx)];
}

void main() {
  float hmVal = heightmapValue();
  vec3 vertPos = vec3(model * vec4(in_pos, 1.f));

  vertPos += in_normal * hmVal * heightmapScale;
  color = vec3(hmVal);
  texCoord = in_tex;
  normal = in_normal;
  gl_Position = cam * vec4(vertPos, 1.f);
}
