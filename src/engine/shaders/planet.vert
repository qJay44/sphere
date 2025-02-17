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

void main() {
  vec3 vertPos = vec3(model * vec4(in_pos, 1.f));
  color = in_col;
  texCoord = in_tex;
  normal = in_normal;
  gl_Position = cam * vec4(vertPos, 1.f);
}
