#version 150

in PassData {
	vec4 position;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
	vec3 view;
} pIn;

out vec4 oFragColor;

uniform vec3 uColor;
const vec3 lightvec = vec3(0.3520,0.2304,0.9072);

void main() {
	float shadeFac = dot(pIn.normal, lightvec) * 0.25 + 0.75;
	oFragColor.rgb = uColor * shadeFac;
	oFragColor.a = 1.0f;
}