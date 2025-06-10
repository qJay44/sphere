#version 460 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_tex;

out DATA {
  vec3 defaultNormal;
  vec2 texCoord;
} data_out;

uniform mat4 u_cam;
uniform mat4 u_model;
uniform vec3 u_camPos;
uniform vec3 u_lightPos;
uniform samplerCube u_normalheightmapsLand;
uniform float u_heightmapScale;
uniform float u_seaLevel;

#define PI 3.141592265359f

void main() {
  vec3 normal = normalize(in_pos);

  float height = texture(u_normalheightmapsLand, normal).a;
  vec3 vertPos = in_pos + normal * height * u_heightmapScale;

  data_out.defaultNormal = normal;
  data_out.texCoord = in_tex;

  gl_Position = u_model * vec4(vertPos, 1.f);
}

