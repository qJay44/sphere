#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 vertPos;
out vec3 vertPosOriginal;
out vec3 defaultNormal;
out vec3 lightPos;
out vec3 camPos;
out vec2 texCoord;

in DATA {
  vec3 defaultNormal;
  vec2 texCoord;
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
  vec3 N;
  mat3 TBN;
  vec4 vertPos4;

  vertPos4 = gl_in[0].gl_Position;
  gl_Position = u_cam * vertPos4;
  vertPos = vertPos4.xyz;
  vertPosOriginal = vertPos;
  defaultNormal = data_in[0].defaultNormal;
  N = normalize(vec3(u_model * vec4(defaultNormal, 0.f)));
  TBN = transpose(mat3(T, B, N));
  vertPos = TBN * vertPos;
  lightPos = TBN * u_lightPos;
  camPos = TBN * u_camPos;
  EmitVertex();

  vertPos4 = gl_in[1].gl_Position;
  gl_Position = u_cam * vertPos4;
  vertPos = vertPos4.xyz;
  vertPosOriginal = vertPos;
  defaultNormal = data_in[1].defaultNormal;
  N = normalize(vec3(u_model * vec4(defaultNormal, 0.f)));
  TBN = transpose(mat3(T, B, N));
  vertPos = TBN * vertPos;
  lightPos = TBN * u_lightPos;
  camPos = TBN * u_camPos;
  EmitVertex();

  vertPos4 = gl_in[2].gl_Position;
  gl_Position = u_cam * vertPos4;
  vertPos = vertPos4.xyz;
  vertPosOriginal = vertPos;
  defaultNormal = data_in[2].defaultNormal;
  N = normalize(vec3(u_model * vec4(defaultNormal, 0.f)));
  TBN = transpose(mat3(T, B, N));
  vertPos = TBN * vertPos;
  lightPos = TBN * u_lightPos;
  camPos = TBN * u_camPos;
  EmitVertex();

  EndPrimitive();
}

