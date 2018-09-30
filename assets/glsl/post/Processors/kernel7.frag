#version 150

in vec2 pTexCoord[7];

out vec4 oFragColor;

uniform sampler2D uColorBuffer;
uniform sampler2D uDepthBuffer;
uniform float uKernel[7];

void main() {
	vec4 col = vec4(0.0f);
	for (int i = 0; i < 7; i++) {
		col += texture(uColorBuffer, pTexCoord[i]) * uKernel[i];
	}
	oFragColor.rgb = col.rgb;
	oFragColor.a = 1.0f;
}