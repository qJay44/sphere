#version 460 core

out vec4 FragColor;

in vec3 vertPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec4 lightColor;

uniform float seaLevel;

#define HEIGHT_MIN -10894
#define HEIGHT_MAX 7891
#define HEIGHT_METRES(h) (HEIGHT_MIN + (HEIGHT_MAX - HEIGHT_MIN) * h)

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

  return (diffuse * inten + ambient) * lightColor;
}

vec4 directionalLight() {
  vec3 n = normalize(normal);
  vec3 lightDirection = normalize(vec3(1.f, 1.f, 0.f));
  float diffuse = max(dot(n, lightDirection), 0.f);
  float ambient = 0.2f;

  float specularLight = 0.5f;
  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, n);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 16);
  float specular = specAmount * specularLight;

  return (diffuse + ambient) * lightColor;
}

void main() {
  vec3 col = color;

  // color is just a height
  // if (HEIGHT_METRES(color.r) > seaLevel) col = vec3(0.f, 1.f, 0.f);

  FragColor = vec4(col, 1.f) * directionalLight();
}
