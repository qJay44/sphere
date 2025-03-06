#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_col;
layout (location = 2) in vec2 in_tex;
layout (location = 3) in vec3 in_normal;

out DATA {
  vec3 color;
  vec2 texCoord;
  vec3 normal;
} data_out;

uniform mat4 model;

void main() {
  vec3 vertPos = vec3(model * vec4(in_pos, 1.f));
  data_out.color = vec3(0.5f);
  data_out.texCoord = in_tex;
  data_out.normal = in_normal;
  gl_Position = vec4(vertPos, 1.f);
}

