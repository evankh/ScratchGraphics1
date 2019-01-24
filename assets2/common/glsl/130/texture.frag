#version 130

in vec3 pNormal;
in vec2 pTexCoord;

out vec4 oFragColor;

uniform sampler2D uTexture;

const vec3 lightvec = vec3(0.3520,0.2304,0.9072);

void main() {
	float shadeFac = dot(pNormal, lightvec)*0.25+0.75;
	oFragColor = shadeFac * texture(uTexture, pTexCoord);
	oFragColor.a = 1.0f;
}