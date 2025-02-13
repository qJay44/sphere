#version 460 core

out vec4 FragColor;

in vec3 vertPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec4 lightColor;
uniform sampler2D diffuse0;

const float PI = 3.141592265359f;

vec4 pointLight() {
  // used in two variables so I calculate it here to not have to do it twice
  vec3 lightVec = lightPos - vertPos;

  // intensity of light with respect to distance
  float dist = length(lightVec);
  float a = 0.5f;
  float b = 0.7f;
  float inten = 1.f / (a * dist * dist + b * dist + 1.f);

  // ambient lighting
  float ambient = 0.2f;

  // diffuse lighting
  vec3 n = normalize(normal);
  vec3 lightDirection = normalize(lightVec);
  float diffuse = max(dot(n, lightDirection), 0.f);

  // specular lighting
  float specularLight = 0.5f;
  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, n);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 16);
  float specular = specAmount * specularLight;

  return texture(diffuse0, texCoord) * (diffuse * inten + ambient) * lightColor;
}

void main() {
  float lon = 0.5f - atan(normal.z, normal.x) / (2.f * PI);
  float lat = 0.5f + asin(normal.y) / PI;
  vec3 col = vec3(texture(diffuse0, vec2(lon, lat)).r);
  FragColor = vec4(col, 1.f);
}
