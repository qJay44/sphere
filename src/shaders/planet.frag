#version 460 core

out vec4 FragColor;

in vec3 vertPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float seaLevel;
uniform float ambient;
uniform float specularLight;

#define HEIGHT_MIN -10894
#define HEIGHT_MAX 7891
#define HEIGHT_METRES(h) (HEIGHT_MIN + (HEIGHT_MAX - HEIGHT_MIN) * h)

vec4 directionalLight() {
  vec3 n = normal;
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(n, lightDirection), 0.f);

  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, n);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 8);
  float specular = specAmount * specularLight;

  return vec4(color, 1.f) * vec4(lightColor, 1.f) * (diffuse + ambient + specular);
}


void main() {
  // if (HEIGHT_METRES(color.a) > seaLevel) col = vec3(0.f, 1.f, 0.f);

  // vec3 lightDirection = normalize(lightPos - vertPos);
  // vec3 col = color * max(0.f, dot(normal, lightDirection));
  // FragColor = vec4(col, 1.f);
  FragColor = directionalLight();
}

