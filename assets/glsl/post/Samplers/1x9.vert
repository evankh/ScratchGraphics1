#version 330

in vec4 iPosition;
layout (location = 8) in vec2 iTexCoord;

out vec2 pTexCoord[9];

uniform float uPixWidth;
uniform float uPixHeight;

void main() {
	gl_Position = vec4(iPosition.x, iPosition.y, 0.0f, 1.0f);
	for (int i = 0; i < 9; i++) {
		pTexCoord[i] = vec2(iTexCoord.x, iTexCoord.y + (i - 4) * uPixHeight);
	}
}