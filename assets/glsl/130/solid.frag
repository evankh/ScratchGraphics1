#version 130

in vec4 pPosition;
in vec3 pColor;
in vec3 pNormal;
in vec2 pTexCoord;
in vec3 pView;

out vec4 oFragColor;

const vec3 lightvec = vec3(0.3520,0.2304,0.9072);

void main() {
	float shadeFac = dot(pNormal, lightvec)*0.25 + 0.75;
	oFragColor.rgb = pColor;
	oFragColor.a = 1.0f;
}