#version 460 core

#define FLOAT_MAX 3.4028235e38f

in vec3 vertPos;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D u_screenColorTex;
uniform sampler2D u_screenDepthTex;
uniform vec3 u_camPos;
uniform float u_atmosphereRadius;
uniform float u_camNear;
uniform float u_camFar;

float LinearEyeDepth(float depth) {
  // Assuming perspective projection
  float z = depth * 2.f - 1.f; // Convert [0,1] to NDC [-1,1]

  // Convert NDC z to eye space z
  return (2.f * u_camNear * u_camFar) / (u_camFar + u_camNear - z * (u_camFar - u_camNear));
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

    if (dstToSphereFar >= 0)
      return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
  }

  return vec2(FLOAT_MAX, 0.f);
}

void main() {
    vec2 uv = gl_FragCoord.xy / textureSize(u_screenDepthTex, 0);
    vec3 viewVector = u_camPos - vertPos;
    vec3 color = texture(u_screenColorTex, texCoord).rgb;
    // vec2 hitInfo = raySphere(vec3(0.f), u_atmosphereRadius, u_camPos, normalize(viewVector));

    // float sceneDepthNonLinear = texture(u_screenDepthTex, uv).r;
    // float sceneDepth = LinearEyeDepth(sceneDepthNonLinear);

    // float dstToAtmosphere = hitInfo.x;
    // float dstThroughAtmosphere = hitInfo.y;
    // float dstThroughAtmosphere = min(hitInfo.y , sceneDepth - dstToAtmosphere);

    FragColor = vec4(color, 1.f);
}

