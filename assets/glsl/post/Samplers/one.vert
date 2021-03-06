#version 330

in vec3 iPosition;
layout (location = 8) in vec2 iTexCoord;

out vec2 pTexCoord;

uniform float uPixWidth;
uniform float uPixHeight;

void main() {
	gl_Position = vec4(iPosition.x +0.1*uPixWidth, iPosition.y +0.1*uPixHeight, 0.0f, 1.0f);
	pTexCoord = iTexCoord;
}