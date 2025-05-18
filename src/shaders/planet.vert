#version 460 core

layout(location = 0) in vec3 in_pos;

out DATA {
  vec2 texCoord;
  float idx;
  float u0;
  float u1;
} data_out;

uniform mat4 u_cam;
uniform mat4 u_model;
uniform vec3 u_camPos;
uniform vec3 u_lightPos;
uniform sampler2DArray u_normalheightmaps;
uniform float u_heightmapScale;

#define PI 3.141592265359f

void main() {
  vec3 normal = normalize(in_pos);
  vec2 texCoord;

  texCoord.x = 0.5f - atan(normal.z, normal.x) / (2.f * PI); // Longitude
  texCoord.y = 0.5f + asin(normal.y) / PI; // Latitude

  float idx = round(texCoord.x);
  texCoord.x = (texCoord.x - 0.5f) * 2.f * idx + texCoord.x * 2.f * (1.f - idx);

  float height = texture(u_normalheightmaps, vec3(texCoord, idx)).a;
  vec3 vertPos = in_pos + normal * height * u_heightmapScale;

  data_out.texCoord = texCoord;
  data_out.idx = idx;
  data_out.u0 = fract(texCoord.x);
  data_out.u1 = fract(texCoord.x + 0.5f) - 0.5f;

  gl_Position = u_model * vec4(vertPos, 1.f);
}

