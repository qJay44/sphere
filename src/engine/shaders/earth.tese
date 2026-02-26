#version 460 core

#define PI 3.141592265359f
// #define HEIGHT_MIN -10894.f
// #define HEIGHT_MAX 7891.f

layout (quads, equal_spacing, cw) in;

in vec2 uvsCoord[];

out DATA {
  vec4 worldPos;
  float maskWater;
} dataOut;

layout(binding = 0) uniform usampler2D u_texIndirection32k;
layout(binding = 2) uniform sampler2DArray u_texVirt32kHeightmapLand;

uniform vec2 u_virtualDims;
uniform mat4 u_camPV;
uniform float u_heightmapScale;
uniform float u_seaLevel; // Kinda useless

vec4 textureVirtual(sampler2DArray texVirtual, usampler2D texIndirection, vec3 normal) {
  vec2 globalUV = vec2(
    atan(normal.z, normal.x) / (2.f * PI) + 0.5f,
    asin(normal.y) / PI + 0.5f
  );

  // Flip to avoid using vips flipping operations
  globalUV = 1.f - globalUV;

  ivec2 tileCoord = ivec2(globalUV * u_virtualDims);
  tileCoord = min(tileCoord, ivec2(u_virtualDims) - 1);

  uint physicalSlot = texelFetch(texIndirection, tileCoord, 0).r;

  if (physicalSlot == 255u)
    return vec4(0.f, 1.f, 0.f, 0.f);

  vec2 localUV = fract(globalUV * u_virtualDims);

  return texture(texVirtual, vec3(localUV, physicalSlot));
}

void main() {
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  vec4 p0 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, u);
  vec4 p1 = mix(gl_in[3].gl_Position, gl_in[2].gl_Position, u);
  vec4 worldPos = mix(p0, p1, v);
  vec3 normal = normalize(worldPos.xyz);

  float height = textureVirtual(u_texVirt32kHeightmapLand, u_texIndirection32k, normal).r + u_seaLevel;
  worldPos.xyz += normal * height * u_heightmapScale;

  dataOut.worldPos = worldPos;
  dataOut.maskWater = float(height <= 0.f);

  gl_Position = u_camPV * worldPos;
}

