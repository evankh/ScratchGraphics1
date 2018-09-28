#version 130

in vec4 pPosition;
in vec3 pColor;
in vec3 pNormal;
in vec2 pTexCoord;

out vec4 oFragColor;

void main() {
	oFragColor.rgb = pColor * (dot(pNormal, vec3(0.48,0.60,0.64))*0.25+0.75);
	oFragColor.a = 1.0f;
}