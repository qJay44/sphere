#version 460 core

#define HEIGHT_MIN -10894
#define HEIGHT_MAX 7891
#define HEIGHT_METRES(h) (HEIGHT_MIN + (HEIGHT_MAX - HEIGHT_MIN) * h)

out vec4 FragColor;

in vec3 vertPos;
in vec2 texCoord;
in float u0;
in float u1;
in float idx;

uniform vec3 u_lightPos;
uniform vec3 u_lightColor;

uniform sampler2DArray u_normalheightmaps;
uniform sampler2DArray u_worldColors;

vec2 texUV = vec2((fwidth(u0) < fwidth(u1) - 0.001f) ? u0 : u1, texCoord.y);
vec3 normal = texture(u_normalheightmaps, vec3(texUV, idx)).rgb;
vec3 color = texture(u_worldColors, vec3(texUV, idx)).rgb;

void main() {
  vec3 lightDir = normalize(u_lightPos - vertPos);
  vec3 col = color * max(0.f, dot(normal, lightDir)) * u_lightColor;

  FragColor = vec4(col, 1.f);
}

