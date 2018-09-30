#version 330

in vec4 iPosition;
layout (location = 8) in vec2 iTexCoord;

out vec2 pTexCoord[9];

uniform float uPixWidth;
uniform float uPixHeight;

void main() {
	gl_Position = vec4(iPosition.x, iPosition.y, 0.0f, 1.0f);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			pTexCoord[i + 3 * j] = vec2(iTexCoord.x + (i - 1) * uPixWidth, iTexCoord.y + (j - 1) * uPixHeight);
		}
	}
}