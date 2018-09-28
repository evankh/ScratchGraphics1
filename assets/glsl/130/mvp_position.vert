#version 330

in vec3 iPosition;
layout (location = 8) in vec2 iTexCoord;
layout (location = 2) in vec3 iNormal;

out vec4 pPosition;
out vec3 pColor;
out vec3 pNormal;
out vec2 pTexCoord;

uniform mat4 uM;
uniform mat4 uVP;

void main() {
	pPosition = uM * vec4(iPosition, 1.0);
	pColor = iPosition;
	pNormal = iNormal;
	pTexCoord = iTexCoord;
	gl_Position = uVP * pPosition;
}