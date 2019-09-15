#version 150

in vec2 pTexCoord;

out vec4 oFragColor;

uniform sampler2D uColorBuffer;
uniform sampler2D uDepthBuffer;

uniform float uPixWidth;
uniform float uPixHeight;

void main() {
	oFragColor = 1.0f - texture(uColorBuffer, pTexCoord);
	oFragColor.w = 1.0f;
}