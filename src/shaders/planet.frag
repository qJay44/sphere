#version 460 core

#define HEIGHT_MIN -10894
#define HEIGHT_MAX 7891
#define HEIGHT_METRES(h) (HEIGHT_MIN + (HEIGHT_MAX - HEIGHT_MIN) * h)

out vec4 FragColor;

in vec3 vertPos;
in vec2 texCoord;
in float idx;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float seaLevel;
uniform float ambient;
uniform float specularLight;

uniform sampler2DArray normalheightmaps;
uniform sampler2DArray worldColors;

vec3 normal = texture(normalheightmaps, vec3(texCoord, idx)).rgb;
vec3 color = texture(worldColors, vec3(texCoord, idx)).rgb;

vec4 directionalLight() {
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(normal, lightDirection), 0.f);

  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 8);
  float specular = specAmount * specularLight;

  return vec4(color, 1.f) * vec4(lightColor, 1.f) * (diffuse + ambient + specular);
}

vec4 pointLight() {
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - vertPos;
  vec3 normal = texture(normalheightmaps, vec3(texCoord, idx)).rgb;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.f;
	float b = 0.7f;
	float inten = 1.f / (a * dist * dist + b * dist + 1.f);

	// diffuse lighting
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.f);

	// specular lighting
	float specular = 0.f;
	if (diffuse) {
		vec3 viewDirection = normalize(camPos - vertPos);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.f), 16);
		specular = specAmount * specularLight;
	};

	return vec4(color, 1.f) * (diffuse * inten + ambient + specular) * vec4(lightColor, 1.f);
}

void main() {
  // if (HEIGHT_METRES(color.a) > seaLevel) col = vec3(0.f, 1.f, 0.f);

  // FragColor = pointLight();
  FragColor = directionalLight();
}

