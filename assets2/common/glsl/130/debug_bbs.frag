#version 130

out vec4 oFragColor;

uniform vec4 uDebugColor;

void main() {
	oFragColor = uDebugColor;
}