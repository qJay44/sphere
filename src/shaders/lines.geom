#version 460 core

layout(triangles) in;
layout(line_strip, max_vertices = 4) out;

out vec3 normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 cam;

in DATA {
  vec3 color;
  vec2 texCoord;
  vec3 normal;
}
data_in[];

void main() {
  gl_Position = cam * gl_in[0].gl_Position;
  normal = data_in[0].normal;
  color = data_in[0].color;
  texCoord = data_in[0].texCoord;
  EmitVertex();

  gl_Position = cam * gl_in[1].gl_Position;
  normal = data_in[1].normal;
  color = data_in[1].color;
  texCoord = data_in[1].texCoord;
  EmitVertex();

  gl_Position = cam * gl_in[2].gl_Position;
  normal = data_in[2].normal;
  color = data_in[2].color;
  texCoord = data_in[2].texCoord;
  EmitVertex();

  gl_Position = cam * gl_in[0].gl_Position;
  normal = data_in[0].normal;
  color = data_in[0].color;
  texCoord = data_in[0].texCoord;
  EmitVertex();

  EndPrimitive();
}
