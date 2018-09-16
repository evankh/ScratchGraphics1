#version 150

in vec4 pPosition;
in vec3 pColor;
in vec2 pTexCoord;

out vec4 oFragColor;

uniform sampler2D uTexture;

void main() {
	oFragColor = texture(uTexture, pTexCoord);
}