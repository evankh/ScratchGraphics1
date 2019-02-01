#version 150

in vec2 pTexCoord;

out vec4 oFragColor;

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;

void main() {
	vec4 col1 = texture(uTexture0, pTexCoord);
	vec4 col2 = texture(uTexture1, pTexCoord);
	float val1 = col1.g;
	float val2 = col2.g;
	oFragColor.rgb = vec3(sqrt(val1*val1 + val2*val2));
//	oFragColor.rgb = vec3(val1);
	oFragColor.a = 1.0f;
}