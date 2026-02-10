#version 460 core

#define FLOAT_MAX 3.4028235e38f
#define PI 3.141592265359f

in vec2 texCoord;
in vec3 viewVec;
in vec3 dirToSun;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_screenColorTex;
layout(binding = 1) uniform sampler2D u_screenDepthTex;
layout(binding = 2) uniform sampler2D u_bakedOpticalDepth;

uniform vec3 u_camPos;
uniform vec3 u_camForward;
uniform float u_camNear;
uniform float u_camFar;

uniform vec3 u_planetCenter;
uniform vec3 u_scatteringCoefficientsR;
uniform vec3 u_scatteringCoefficientsM;
uniform int u_scatteringPoints;
uniform int u_opticalDepthPoints;
uniform float u_planetRadius;
uniform float u_atmosphereRadius;
uniform float u_densityFalloffR;
uniform float u_densityFalloffM;
uniform float u_sunIntensity;
uniform float u_maskGammaCorrection;

float linearizeDepth(float depth) {
  float z = depth * 2.f - 1.f;

  return (2.f * u_camNear * u_camFar) / (u_camFar + u_camNear - z * (u_camFar - u_camNear));
}

vec3 applyMask(vec3 no, vec3 yes, float mask) {
  return yes * mask + (1.f - mask) * no;
}

vec2 raySphere(vec3 center, float radius, vec3 rayOrigin, vec3 rayDir) {
  vec3 offset = rayOrigin - center;
  float a = 1.f;
  float b = 2.f * dot(offset, rayDir);
  float c = dot(offset, offset) - radius * radius;
  float discriminant = b * b - 4.f * a * c;

  if (discriminant > 0.f) {
    float s = sqrt(discriminant);
    float dstToSphereNear = max(0.f, (-b - s) / (2.f * a));
    float dstToSphereFar = (-b + s) / (2.f * a);

    if (dstToSphereFar >= 0.f)
      return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
  }

  return vec2(FLOAT_MAX, 0.f);
}

float densityAtPoint(vec3 densitySamplePoint, float scale) {
  float heightAboveSurface = length(densitySamplePoint - u_planetCenter) - u_planetRadius;
  float heightNorm = heightAboveSurface / (u_atmosphereRadius - u_planetRadius);
  float localDensity = exp(-heightNorm * scale) * (1.f - heightNorm);

  return localDensity;
}

vec2 opticalDepth(vec3 rayOrigin, vec3 rayDir) {
  float height = length(u_planetCenter - rayOrigin) - u_planetRadius;
  float heightNorm = height / (u_atmosphereRadius - u_planetRadius);
  float angleNorm = dot(normalize(rayOrigin - u_planetCenter), rayDir) * 0.5f + 0.5f;
  vec2 uv = vec2(angleNorm, heightNorm);

  return texture(u_bakedOpticalDepth, uv).rg;
}

float RayleighPhase(vec3 rayDir) {
  float cosAngle = dot(rayDir, dirToSun);
  return (3.f / (16.f * PI)) * (1.f + cosAngle * cosAngle);
}

float MiePhase(vec3 rayDir) {
  const float g = 0.76f;
  const float gg = g * g;
  float cosAngle = dot(rayDir, dirToSun);

  return ((1.f - gg) / pow(1.f + gg - 2.f * g * cosAngle, 1.5f));
}

vec3 calcLight(vec3 rayOrigin, vec3 rayDir, float rayLength, vec3 originalColor) {
  // inScatteredLight
  vec3 totalR = vec3(0.f);
  vec3 totalM = vec3(0.f);

  float viewRayOpticalDepthR = 0.f;
  float viewRayOpticalDepthM = 0.f;

  float stepSize = rayLength / float(u_scatteringPoints - 1);

  for (int i = 0; i < u_scatteringPoints; i++) {
    vec3 p = rayOrigin + rayDir * i * stepSize; // inScatteringPoint

    float densityR = densityAtPoint(p, u_densityFalloffR) * stepSize;
    float densityM = densityAtPoint(p, u_densityFalloffM) * stepSize;

    viewRayOpticalDepthR += densityR;
    viewRayOpticalDepthM += densityM;

    vec2 sunRayOpticalDepth = opticalDepth(p, rayDir);

    vec3 transmittance = exp(
      -(sunRayOpticalDepth.r + viewRayOpticalDepthR) * u_scatteringCoefficientsR +
      -(sunRayOpticalDepth.g + viewRayOpticalDepthM) * u_scatteringCoefficientsM
    );

    totalR += densityR * transmittance;
    totalM += densityM * transmittance;
  }

  vec3 rayleighFinal = totalR * u_scatteringCoefficientsR * RayleighPhase(rayDir);
  vec3 mieFinal      = totalM * u_scatteringCoefficientsM * MiePhase(rayDir);

  vec3 atmosphereColor = (rayleighFinal + mieFinal) * u_sunIntensity;

  vec3 groundTransmittance = exp(-(
    viewRayOpticalDepthR * u_scatteringCoefficientsR +
    viewRayOpticalDepthM * u_scatteringCoefficientsM
  ));

  return (originalColor * groundTransmittance) + atmosphereColor;
}

void main() {
  vec3 rayOrigin = u_camPos;
  vec3 rayDir = normalize(viewVec);
  vec3 color = texture(u_screenColorTex, texCoord).rgb;
  vec2 hitInfo = raySphere(u_planetCenter, u_atmosphereRadius, rayOrigin, rayDir);

  float sceneDepthNonLinear = texture(u_screenDepthTex, texCoord).r;
  float eyeDepth = linearizeDepth(sceneDepthNonLinear);
  float sceneDepth = eyeDepth / dot(rayDir, u_camForward);

  float dstToAtmosphere = hitInfo.x;
  float dstThroughAtmosphere = min(hitInfo.y, sceneDepth - dstToAtmosphere);

  if (dstThroughAtmosphere > 0.f) {
    vec3 pointInAtmosphere = rayOrigin + rayDir * dstToAtmosphere;
    vec3 light = calcLight(pointInAtmosphere, rayDir, dstThroughAtmosphere, color);

    color = light;
  }

  color = applyMask(color, pow(color, vec3(1.f / 2.2f)), u_maskGammaCorrection);

  FragColor = vec4(color, 1.f);
}

