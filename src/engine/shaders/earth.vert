#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTex;

out DATA {
  vec3 defaultNormal;
  vec2 texCoord;
} data_out;

layout(binding = 2) uniform samplerCube u_normalheightmapsLand;

uniform mat4 u_model;
uniform float u_heightmapScale;

void main() {
  vec3 normal = normalize(inPos);

  float height = texture(u_normalheightmapsLand, normal).a;
  vec3 vertPos = inPos + normal * height * exp(u_heightmapScale);

  data_out.defaultNormal = normal;
  data_out.texCoord = inTex;

  gl_Position = u_model * vec4(vertPos, 1.f);
}

