#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 vertPos;
out vec2 texCoord;
out vec3 lightPos;
out vec3 camPos;
out float u0;
out float u1;
out float idx;

in DATA {
  vec2 texCoord;
  float idx;
  float u0;
  float u1;
} data_in[];

uniform mat4 u_model;
uniform mat4 u_cam;
uniform vec3 u_camPos;
uniform vec3 u_lightPos;

void main() {
  // Edges of the triangle
  vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

  // Lengths of UV differences
  vec2 deltaUV0 = data_in[1].texCoord - data_in[0].texCoord;
  vec2 deltaUV1 = data_in[2].texCoord - data_in[0].texCoord;

  // one over the determinant
  float invDet = 1.f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

  vec3 tangent   = invDet * ( deltaUV1.y * edge0 - deltaUV0.y * edge1);
  vec3 bitangent = invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1);

  vec3 T = normalize(vec3(u_model * vec4(tangent, 0.f)));
  vec3 B = normalize(vec3(u_model * vec4(bitangent, 0.f)));
  vec3 N = normalize(vec3(u_model * vec4(cross(edge0, edge1), 0.f)));

  mat3 TBN = mat3(T, B, N);
  TBN = transpose(TBN); // TBN is an orthogonal matrix and so its inverse is equal to its transpose
  lightPos = TBN * u_lightPos;
  camPos = TBN * u_camPos;

  gl_Position = u_cam * gl_in[0].gl_Position;
  vertPos = TBN * gl_in[0].gl_Position.xyz;
  texCoord = data_in[0].texCoord;
  idx = data_in[0].idx;
  u0 = data_in[0].u0;
  u1 = data_in[0].u1;
  EmitVertex();

  gl_Position = u_cam * gl_in[1].gl_Position;
  vertPos = TBN * gl_in[1].gl_Position.xyz;
  texCoord = data_in[1].texCoord;
  idx = data_in[1].idx;
  u0 = data_in[1].u0;
  u1 = data_in[1].u1;
  EmitVertex();

  gl_Position = u_cam * gl_in[2].gl_Position;
  vertPos = TBN * gl_in[2].gl_Position.xyz;
  texCoord = data_in[2].texCoord;
  idx = data_in[2].idx;
  u0 = data_in[2].u0;
  u1 = data_in[2].u1;
  EmitVertex();

  EndPrimitive();
}

