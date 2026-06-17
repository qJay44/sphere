#version 460 core

#define PI 3.141592265359f

#define saturate(x) clamp(x, 0.f, 1.f)

out vec4 FragColor;
in DATA {
  vec4 worldPos;
  float maskWater;
} dataIn;

layout(binding = 0) uniform usampler2D u_texIndirection32k;
layout(binding = 1) uniform sampler2DArray u_texVirt32kColors;
layout(binding = 3) uniform sampler2DArray u_texVirt32kNormalmapLand;

layout(binding = 4) uniform sampler2D u_texBathymetry;
layout(binding = 5) uniform sampler2D u_texShoreSDF;
layout(binding = 6) uniform sampler2D u_texBorders;
layout(binding = 7) uniform sampler2D u_texNormalmapWave0;
layout(binding = 8) uniform sampler2D u_texNormalmapWave1;
layout(binding = 9) uniform sampler2D u_texNoise;

uniform vec2 u_virtualDims;
uniform vec3 u_camPos;
uniform vec3 u_sunDir;
uniform vec3 u_sunColor;
uniform vec3 u_bordersColor;
uniform vec3 u_waterDeepColor;
uniform vec3 u_waterShallowColor;
uniform vec3 u_terrainFaceColor;
uniform vec3 u_terrainFaceChunkColor;
uniform float u_time;
uniform float u_radius;
uniform float u_ambient;
uniform float u_seaLevel;
uniform float u_lightMultiplier;
uniform float u_maskTerrainFaceColor;
uniform float u_maskTerrainFaceChunkColor;
uniform float u_waterDeepFactor;
uniform float u_waterWaveFreq;
uniform float u_waterWaveResScale;
uniform float u_waterShoreScale;
uniform float u_waterShoreFreq;
uniform float u_waterShoreNoiseFreq;
uniform float u_waterShoreNoiseScale;
uniform float u_waterShoreNoiseStrength;
uniform float u_waterShoreMaskBlend;
uniform float u_waterShoreWidth;
uniform float u_waterShoreEdgeBlend;
uniform float u_triplanarBlendSharpness;

uniform float u_1f0;

vec3 sphereNormal = normalize(dataIn.worldPos.xyz);
vec3 viewDir = normalize(u_camPos - dataIn.worldPos.xyz);

// Flip to avoid using vips flipping operations
vec2 globalUV = vec2(
  0.5f - atan(sphereNormal.z, sphereNormal.x) / (2.f * PI),
  0.5f - (asin(sphereNormal.y) / PI)
);

vec3 applyMask(vec3 no, vec3 yes, float mask) {
  return yes * mask + (1.f - mask) * no;
}

vec3 directionalLight(vec3 normal) {
  float diffuse = max(dot(normal, u_sunDir), 0.f);
  float total = diffuse + u_ambient;

  return u_sunColor * total * u_lightMultiplier;
}

vec3 specularLight(vec3 normal) {
  vec3 halfwayDir = normalize(u_sunDir + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.f), 128.f);

  return u_sunColor * spec;
}

vec3 triplanarSample(sampler2D tex, float scale, vec2 offset) {
  vec3 scaledWorldPos = dataIn.worldPos.xyz * scale;
  vec3 blendAxis = abs(sphereNormal);
  vec3 xProj = texture(tex, scaledWorldPos.yz + offset).rgb * blendAxis.x;
  vec3 yProj = texture(tex, scaledWorldPos.xz + offset).rgb * blendAxis.y;
  vec3 zProj = texture(tex, scaledWorldPos.xy + offset).rgb * blendAxis.z;

  return xProj + yProj + zProj;
}

vec3 triplanarNormal(sampler2D normalmap, vec3 normal, float scale, float timeStep) {
  vec3 absNormal = abs(normal);
  vec3 weights = pow(absNormal, vec3(u_triplanarBlendSharpness));
  weights /= (weights.x + weights.y + weights.z);

  vec3 scaledWorldPos = dataIn.worldPos.xyz * scale + timeStep;
  vec3 tangentNormalX = texture(normalmap, scaledWorldPos.zy).rgb * 2.f - 1.f;
  vec3 tangentNormalY = texture(normalmap, scaledWorldPos.xz).rgb * 2.f - 1.f;
  vec3 tangentNormalZ = texture(normalmap, scaledWorldPos.xy).rgb * 2.f - 1.f;

  vec3 axisSign = sign(normal);
  tangentNormalX.xy *= axisSign.x;
  tangentNormalY.xy *= axisSign.y;
  tangentNormalZ.xy *= axisSign.z;

  vec3 worldX = vec3(normal.x, tangentNormalX.yx);
  vec3 worldY = vec3(tangentNormalY.x, normal.y, tangentNormalY.y);
  vec3 worldZ = vec3(tangentNormalZ.xy, normal.z);

  return normalize(worldX * weights.x + worldY * weights.y + worldZ * weights.z);
}

vec4 textureVirtual(sampler2DArray texVirtual, usampler2D texIndirection) {
  ivec2 tileCoord = ivec2(globalUV * u_virtualDims);
  tileCoord = min(tileCoord, ivec2(u_virtualDims) - 1);

  uint physicalSlot = texelFetch(texIndirection, tileCoord, 0).r;

  if (physicalSlot == 255u)
    return vec4(0.f, 1.f, 0.f, 0.f);

  vec2 localUV = fract(globalUV * u_virtualDims);

  return texture(texVirtual, vec3(localUV, physicalSlot));
}

vec3 getDeepnessWaterColor(float deepness) {
  return mix(u_waterDeepColor, u_waterShallowColor, deepness * u_waterDeepFactor);
}

float getShoreFoam(float sdf) {
  float dist = sdf * u_waterShoreScale;

  vec2 noiseOffset = vec2(0.0617f, 0.0314f) * u_waterShoreNoiseFreq * u_time;
  float noise = triplanarSample(u_texNoise, u_waterShoreNoiseScale, noiseOffset).r;
  noise = (noise - 0.5f) * u_waterShoreNoiseStrength * sdf;

  vec2 maskOffset = vec2(-0.021f, 0.07f) * u_waterShoreNoiseFreq * u_time;
  float mask = triplanarSample(u_texNoise, u_waterShoreNoiseScale, maskOffset).r;
  float threshold = mix(0.375f, 0.55f, sdf);
  mask = smoothstep(threshold, threshold + u_waterShoreMaskBlend, mask);

  float strength = sin(dist - (u_time + 1000.f) * u_waterShoreFreq + noise);
  strength = saturate(smoothstep(u_waterShoreWidth, u_waterShoreWidth + u_waterShoreEdgeBlend, strength + 1.f)) * mask;
  strength *= 1.f - smoothstep(0.7f, 1.f, dist);

  return strength;
}

vec3 getWavesNormal() {
  float waveFreq = u_time * u_waterWaveFreq;
  vec3 normalWave0 = triplanarNormal(u_texNormalmapWave0, sphereNormal, u_waterWaveResScale, -waveFreq);
  vec3 normalWave1 = triplanarNormal(u_texNormalmapWave1, sphereNormal, u_waterWaveResScale,  waveFreq);

  return normalize(normalWave0 + normalWave1);
}

vec3 getLandNormal() {
  vec4 landNormalSample = textureVirtual(u_texVirt32kNormalmapLand, u_texIndirection32k);
  vec3 landNormal = normalize(landNormalSample.rgb * 2.f - 1.f);

  landNormal.z *= -1.f;
  landNormal.xz = landNormal.zx;

  return landNormal;
}

void main() {
  vec3 surfaceColor = textureVirtual(u_texVirt32kColors, u_texIndirection32k).rgb;
  vec3 landNormal = getLandNormal();
  vec3 waterWavesNormal = getWavesNormal();

  float border = texture(u_texBorders, globalUV).r;
  float deepness = texture(u_texBathymetry, globalUV).r;
  float sdf = texture(u_texShoreSDF, globalUV).r;
  float maskWater = dataIn.maskWater;
  float maskLand = 1.f - maskWater;

  vec3 landColor = surfaceColor;
  landColor *= directionalLight(landNormal);
  landColor *= maskLand;

  vec3 waterColor = getDeepnessWaterColor(deepness);
  waterColor = mix(waterColor, vec3(1.f), getShoreFoam(sdf));
  waterColor *= directionalLight(sphereNormal);
  waterColor += specularLight(waterWavesNormal);
  waterColor *= maskWater;

  vec3 finalColor = landColor + waterColor;

  finalColor = mix(finalColor, u_bordersColor, border); // TODO: Generate smoother borders

  // debug
  finalColor = applyMask(finalColor, u_terrainFaceColor, u_maskTerrainFaceColor);
  finalColor = applyMask(finalColor, u_terrainFaceChunkColor, u_maskTerrainFaceChunkColor);

  FragColor = vec4(finalColor, 1.f);
}

