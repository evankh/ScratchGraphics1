#version 150

in PassData {
	vec4 position;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
	vec3 view;
} pIn;

out vec4 oFragColor;

void main() {
	oFragColor.rgb = pIn.color;
	oFragColor.a = 1.0f;
}