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
uniform vec3 u_scatteringCoefficients;
uniform int u_scatteringPoints;
uniform int u_opticalDepthPoints;
uniform float u_planetRadius;
uniform float u_atmosphereRadius;
uniform float u_densityFalloff;
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

float densityAtPoint(vec3 densitySamplePoint) {
  float heightAboveSurface = length(densitySamplePoint - u_planetCenter) - u_planetRadius;
  float heightNorm = heightAboveSurface / (u_atmosphereRadius - u_planetRadius);
  float localDensity = exp(-heightNorm * u_densityFalloff) * (1.f - heightNorm);

  return localDensity;
}

float opticalDepth(vec3 rayOrigin, vec3 rayDir) {
  float height = length(u_planetCenter - rayOrigin) - u_planetRadius;
  float heightNorm = height / (u_atmosphereRadius - u_planetRadius);
  float angleNorm = dot(normalize(rayOrigin - u_planetCenter), rayDir) * 0.5f + 0.5f;
  vec2 uv = vec2(angleNorm, heightNorm);

  return texture(u_bakedOpticalDepth, uv).r;
}

vec3 calcLight(vec3 rayOrigin, vec3 rayDir, float rayLength, vec3 originalColor) {
  vec3 inScatteringPoint = rayOrigin;
  vec3 inScatteredLight = vec3(0.f);
  float stepSize = rayLength / float(u_scatteringPoints - 1);
  float viewRayOpticalDepth = 0.f;

  for (int i = 0; i < u_scatteringPoints; i++) {
    float sunRayOpticalDepth = opticalDepth(inScatteringPoint, dirToSun);
    float localDensity = densityAtPoint(inScatteringPoint);
    viewRayOpticalDepth += localDensity * stepSize;
    vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * u_scatteringCoefficients);

    inScatteredLight += localDensity * transmittance * u_scatteringCoefficients * stepSize;
    inScatteringPoint += rayDir * stepSize;
  }

  vec3 originalColorTransmittance = exp(-viewRayOpticalDepth * u_scatteringCoefficients);

  return originalColor * originalColorTransmittance + inScatteredLight * u_sunIntensity;
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

