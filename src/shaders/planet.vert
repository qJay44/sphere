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

uniform sampler2DArray normalheightmaps;
uniform sampler2DArray worldColors;

uniform float heightmapScale;

#define PI 3.141592265359f

// #define _COLOR_TERRAIN_CHUNKS

void main() {
  float lon = 0.5f - atan(in_normal.z, in_normal.x) / (2.f * PI);
  float lat = 0.5f + asin(in_normal.y) / PI;
  float idx = round(lon);
  vec2 texCoord = vec2(lon, lat);
  texCoord.x = (texCoord.x - 0.5f) * 2.f * idx + texCoord.x * 2.f * (1.f - idx);

  vec4 normalheightmap = texture(normalheightmaps, vec3(texCoord, idx));
  vec3 vertPos = vec3(model * vec4(in_pos, 1.f));
  normal = normalheightmap.rgb;

  #ifdef _COLOR_TERRAIN_CHUNKS
    color = in_col;
  #else
    vertPos += in_normal * normalheightmap.a * heightmapScale;
    color = texture(worldColors, vec3(texCoord, idx)).rgb;
  #endif

  gl_Position = cam * vec4(vertPos, 1.f);
}

