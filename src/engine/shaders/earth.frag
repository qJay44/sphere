#version 460 core

#define INT16_MIN -32768
#define INT16_MAX  32767
#define UINT16_MAX 65535u

out vec4 FragColor;

in vec3 vertPos;
in vec3 vertPosOriginal;
in vec3 defaultNormal;
in vec3 lightPos;
in vec3 camPos;
in vec2 texCoord;

uniform vec3 u_debug_terrainFaceColor;
uniform vec3 u_debug_terrainFaceChunkColor;

layout(binding = 0) uniform isamplerCube u_heightmapsWater;
layout(binding = 1) uniform usamplerCube u_distanceFieldsWater;
layout(binding = 2) uniform samplerCube u_normalheightmapsLand;
layout(binding = 3) uniform samplerCube u_worldColors;
layout(binding = 4) uniform samplerCube u_borders;
layout(binding = 5) uniform sampler2D u_normalmapWave0;
layout(binding = 6) uniform sampler2D u_normalmapWave1;

uniform vec3 u_lightColor;
uniform vec3 u_bordersColor;
uniform vec3 u_waterShallowColor;
uniform vec3 u_waterDeepColor;
uniform float u_lightMultiplier;
uniform float u_ambient;
uniform float u_specularLight;
uniform float u_waterDeepFactor;
uniform float u_waterDeepEdgeStart;
uniform float u_waterDeepEdgeEnd;
uniform float u_waterSpecularSmoothness;
uniform float u_waterWaveFreq;
uniform float u_waterWaveResMult;
uniform float u_waterShoreWaveFreq;
uniform float u_waterShoreWaveThreshold;
uniform float u_waterShoreWaveThresholdStart;
uniform float u_waterShoreWaveThresholdEnd;
uniform float u_waterShoreWaveAmplitude;
uniform float u_waterShoreWaveNoiseScale;
uniform float u_waterShoreWaveNoiseSpeed;
uniform float u_waterShoreWaveNoiseAmplitude;
uniform float u_time;
uniform float u_triplanarBlendSharpness;

float hash(vec2 p) {
  p = fract(p * vec2(123.34, 456.21));
  p += dot(p, p + 78.233);
  return fract(p.x * p.y);
}

float noise(vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);

  // Four corners
  float a = hash(i);
  float b = hash(i + vec2(1.0, 0.0));
  float c = hash(i + vec2(0.0, 1.0));
  float d = hash(i + vec2(1.0, 1.0));

  // Interpolation
  vec2 u = f * f * (3.0 - 2.0 * f);

  return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

vec3 directionalLight(vec3 normal) {
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(normal, lightDirection), 0.f);

  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 8);
  float specular = specAmount * u_specularLight;

  return (diffuse + u_ambient + specular) * u_lightMultiplier * u_lightColor;
}

vec3 triplanarNormal(sampler2D normalmap, float timeStep) {
  vec3 absNormal = abs(defaultNormal);
  float sum = absNormal.x + absNormal.y + absNormal.z;
  vec3 weights = absNormal / (sum + 1e-16f);

  vec3 colorX = texture(normalmap, vertPosOriginal.yz * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;
  vec3 colorY = texture(normalmap, vertPosOriginal.xz * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;
  vec3 colorZ = texture(normalmap, vertPosOriginal.xy * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;

  weights = pow(weights, vec3(u_triplanarBlendSharpness));
  weights /= (weights.x + weights.y + weights.z);

  return normalize(colorX * weights.x + colorY * weights.y + colorZ * weights.z);
}

float calculateSpecular(vec3 normal) {
  vec3 lightDirection = normalize(lightPos - vertPos);
  float specularAngle = acos(dot(lightDirection, normal));
  float specularExponent = specularAngle / u_waterSpecularSmoothness;
  float specularHighlight = exp(-specularExponent * specularExponent);

  return specularHighlight;
}

vec3 calculateDeepColor(int deepness) {
  float d = float(deepness) / INT16_MAX * 0.5f + 0.5f;
  float t = pow(d, u_waterDeepFactor);
  t = smoothstep(u_waterDeepEdgeStart, u_waterDeepEdgeEnd, t);

  return mix(u_waterDeepColor, u_waterShallowColor, t);
}

float calculateShoreWaves() {
  const uint distFromShore = texture(u_distanceFieldsWater, defaultNormal).r;
  float dist = 1.f - float(distFromShore) / UINT16_MAX;
  float factor = smoothstep(u_waterShoreWaveThresholdStart, u_waterShoreWaveThresholdEnd, dist);
  float n = noise(vertPosOriginal.xy * u_waterShoreWaveNoiseScale + u_time * u_waterShoreWaveNoiseSpeed);
  float wave = sin(dist * u_waterShoreWaveFreq + u_time * 2.f + n * u_waterShoreWaveNoiseAmplitude) * u_waterShoreWaveAmplitude;

  return abs(mix(0.f, wave, factor));
}

void main() {
  vec3 normal = texture(u_normalheightmapsLand, defaultNormal).rgb * 2.f - 1.f;
  vec3 color = texture(u_worldColors, defaultNormal).rgb;
  vec3 normalWave0 = triplanarNormal(u_normalmapWave0, -u_time * u_waterWaveFreq);
  vec3 normalWave1 = triplanarNormal(u_normalmapWave1,  u_time * u_waterWaveFreq);
  vec3 normalWaves = normalize(normalWave0 + normalWave1);
  vec3 dirLight = directionalLight(normal);
  int deepness = texture(u_heightmapsWater, defaultNormal).r; // [-32768, 32767]

  float border = texture(u_borders, defaultNormal).r;
  float isWater = 1.f - (sign(deepness) * 0.5f + 0.5f);
  float isLand = 1.f - isWater;

  color = isWater * calculateDeepColor(deepness) + isLand * color;
  color *= isLand * dirLight + isWater;
  color += isWater * calculateShoreWaves() * dirLight;
  color *= isWater * directionalLight(normalWaves) + isLand;
  color += isWater * calculateSpecular(normalWaves);
  color += border * u_bordersColor;

  FragColor = vec4(color, 1.f);
}

