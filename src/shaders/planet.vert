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

uniform sampler2D normalheightmap0;
uniform sampler2D normalheightmap1;

uniform float heightmapScale;

#define PI 3.141592265359f

vec4 getHeightmapValue() {
  float lon = 0.5f - atan(in_normal.z, in_normal.x) / (2.f * PI);
  float lat = 0.5f + asin(in_normal.y) / PI;
  float idx = round(lon);
  vec2 coord = vec2(lon, lat);
  coord.x = (coord.x - 0.5f) * 2.f * idx + coord.x * 2.f * (1.f - idx);

  vec4 nhms[2] = vec4[2](
    texture(normalheightmap0, coord),
    texture(normalheightmap1, coord)
  );

  return nhms[uint(idx)];
}

// #define _COLOR_TERRAIN_CHUNKS

void main() {
  vec4 normalheightmap = getHeightmapValue();
  vec3 vertPos = vec3(model * vec4(in_pos, 1.f));
  normal = normalheightmap.xyz;

  #ifdef _COLOR_TERRAIN_CHUNKS
    color = in_col;
  #else
    vertPos += in_normal * normalheightmap.w * heightmapScale;
    color = vec3(normalheightmap.w);
  #endif

  texCoord = in_tex;
  gl_Position = cam * vec4(vertPos, 1.f);
}

