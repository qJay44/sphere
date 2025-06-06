#version 460 core

out vec4 FragColor;

in vec3 vertPos;
in vec2 texCoord;
in vec3 lightPos;
in vec3 camPos;
in float u0;
in float u1;
in float idx;

uniform vec3 u_lightColor;
uniform vec3 u_bordersColor;
uniform vec3 u_waterShallowColor;
uniform vec3 u_waterDeepColor;
uniform sampler2DArray u_normalheightmapsLand;
uniform sampler2DArray u_worldColors;
uniform sampler2DArray u_borders;
uniform isampler2DArray u_heightmapsWater;
uniform float u_lightMultiplier;
uniform float u_ambient;
uniform float u_specularLight;
uniform float u_waterDeepFactor;
uniform float u_waterSpecularSmoothness;

vec2 texUV = vec2((fwidth(u0) < fwidth(u1) - 0.001f) ? u0 : u1, texCoord.y);
vec3 texP = vec3(texUV, idx);
vec3 normal = texture(u_normalheightmapsLand, texP).rgb * 2.f - 1.f;
vec3 color = texture(u_worldColors, texP).rgb;
float border = texture(u_borders, texP).r;
float deepness = float(texture(u_heightmapsWater, texP).r); // [-32768, 32767]
float isWater = 1.f - (sign(deepness) * 0.5f + 0.5f);
float isLand = 1.f - isWater;

vec3 directionalLight() {
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(normal, lightDirection), 0.f);

  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 8);
  float specular = specAmount * u_specularLight;

  return (diffuse + u_ambient + specular) * u_lightMultiplier * u_lightColor;
}

float calculateSpecular() {
  vec3 lightDirection = normalize(lightPos - vertPos);
  float specularAngle = max(acos(dot(lightDirection, normal)), 0.f);
  float specularExponent = specularAngle / u_waterSpecularSmoothness;
  float specularHighlight = exp(-specularExponent * specularExponent);

  return specularHighlight * sign(specularHighlight);
}

void main() {
  deepness = deepness / 32768.f * 0.5f + 0.5f;

  color += isWater * u_waterDeepColor * deepness;
  color += isWater * u_waterShallowColor * (deepness + u_waterDeepFactor);

  color *= directionalLight();
  color += isWater * calculateSpecular();
  color += border * u_bordersColor;

  FragColor = vec4(color, 1.f);
}

