#version 150

in vec2 pTexCoord[9];

out vec4 oFragColor;

uniform sampler2D uColorBuffer;
uniform sampler2D uDepthBuffer;
uniform float uKernel[9];

void main() {
	vec4 col = vec4(0.0f);
	for (int i = 0; i < 9; i++) {
		col += texture(uColorBuffer, pTexCoord[i]) * uKernel[i];
	}
	oFragColor.xyz = col.xyz;
	oFragColor.w = 1.0f;
}