#version 460 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D u_screenTex;

void main() {
  vec3 color = texture(u_screenTex, texCoord).rgb;
	FragColor = vec4(color, 1.f);
}

