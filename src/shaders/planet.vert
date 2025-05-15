#version 460 core

// layout(location = 1) in vec3 in_col; // Color of a chunk or terrain face
// layout(location = 2) in vec2 in_tex; // Texture coordinates of terrain face

layout(location = 0) in vec3 in_pos;
layout(location = 3) in vec3 in_normal;

out DATA {
  vec2 texCoord;
  float idx;
  float u0;
  float u1;
} data_out;

uniform mat4 u_model;
uniform sampler2DArray u_normalheightmaps;
uniform float u_heightmapScale;

#define PI 3.141592265359f

void main() {
  vec2 texCoord;
  texCoord.x = 0.5f - atan(in_normal.z, in_normal.x) / (2.f * PI); // Longitude
  texCoord.y = 0.5f + asin(in_normal.y) / PI; // Latitude

  float idx = round(texCoord.x);
  texCoord.x = (texCoord.x - 0.5f) * 2.f * idx + texCoord.x * 2.f * (1.f - idx);

  float height = texture(u_normalheightmaps, vec3(texCoord, idx)).a;
  vec3 vertPos = vec3(u_model * vec4(in_pos, 1.f));
  vertPos += in_normal * height * u_heightmapScale;

  data_out.texCoord = texCoord;
  data_out.idx = idx;
  data_out.u0 = fract(texCoord.x);
  data_out.u1 = fract(texCoord.x + 0.5f) - 0.5f;

  gl_Position = vec4(vertPos, 1.f);
}

