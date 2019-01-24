#version 130

in vec3 pColor;

out vec4 oFragColor;

void main() {
	oFragColor.rgb = pColor;
	oFragColor.a = 1.0f;
}