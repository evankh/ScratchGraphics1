#version 150

in vec4 pPosition;
in vec3 pColor;

out vec4 oFragColor;

void main() {
	oFragColor.xyz = pColor;
	oFragColor.w = 1.0f;
}