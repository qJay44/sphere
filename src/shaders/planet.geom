#version 460 core

layout(triangles) in;
layout(triangles, max_vertices = 3) out;

in DATA {
  vec2 texCoord;
  float idx;
} data_in[];

uniform mat4 cam;

void main() {
  gl_Position = cam * gl_in[0].gl_Position;
  EmitVertex();
  gl_Position = cam * (gl_in[0].gl_Position + vec4(data_in[0].normal, 0.f) * scale);
  EmitVertex();
  EndPrimitive();

  gl_Position = cam * gl_in[1].gl_Position;
  EmitVertex();
  gl_Position = cam * (gl_in[1].gl_Position + vec4(data_in[1].normal, 0.f) * scale);
  EmitVertex();
  EndPrimitive();

  gl_Position = cam * gl_in[2].gl_Position;
  EmitVertex();
  gl_Position = cam * (gl_in[2].gl_Position + vec4(data_in[2].normal, 0.f) * scale);
  EmitVertex();
  EndPrimitive();
}
