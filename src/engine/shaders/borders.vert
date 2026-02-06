#version 460 core

layout (location = 0) in vec3 in_pos;

layout(binding = 0) uniform sampler2DArray u_normalheightmaps;

uniform mat4 u_model;
uniform mat4 u_camPV;
uniform float u_planetRadius;
uniform float u_borderDataScale;
uniform float u_heightMultiplier;

void main() {
  vec2 coord = in_pos.xy;
  float r = cos(coord.y);
  vec3 vertPos = vec3(
    sin(coord.x) * r,
    sin(coord.y),
    cos(coord.x) * r
  );
  vertPos *= u_planetRadius;

  coord = coord * 0.5f + 0.5f; // to [0, 1]
  float idx = round(coord.x);
  coord.x = (coord.x - 0.5f) * 2.f * idx + coord.x * 2.f * (1.f - idx);

  vec3 normal = normalize(vertPos);
  float height = texture(u_normalheightmaps, vec3(coord, idx)).a;

  vertPos += normal * height * u_heightMultiplier;
  vertPos += normal * u_borderDataScale;

	gl_Position = u_camPV * u_model * vec4(vertPos, 1.0f);
}

