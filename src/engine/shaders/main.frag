#version 330 core

out vec4 FragColor;

in vec3 vertPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

uniform vec3 camPos;

void main() {
	FragColor = vec4(color, 1.f);
}

