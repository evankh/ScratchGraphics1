#version 150

in vec2 pTexCoord;

out vec4 oFragColor;

uniform sampler2D uColorBuffer;
uniform sampler2D uDepthBuffer;

void main() {
	oFragColor = texture(uColorBuffer, pTexCoord);
}