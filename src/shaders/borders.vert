#version 460 core

layout (location = 0) in vec3 in_pos;

uniform mat4 u_model;
uniform mat4 u_cam;
uniform float u_planetRadius;
uniform float u_borderDataScale;

void main() {
  vec3 coord = in_pos;
  float r = cos(coord.y);
  vec3 vertPos = vec3(
    u_planetRadius * sin(coord.x) * r,
    u_planetRadius * sin(coord.y),
    u_planetRadius * cos(coord.x) * r
  );

  vertPos *= u_borderDataScale;

	gl_Position = u_cam * u_model * vec4(vertPos, 1.0f);
}

