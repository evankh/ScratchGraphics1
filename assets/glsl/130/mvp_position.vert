#version 130

in vec4 iPosition;
in vec4 iColor;

out vec4 pPosition;
out vec3 pColor;
out vec2 pTexCoord;

uniform mat4 uM;
uniform mat4 uVP;

void main() {
	pPosition = uM * iPosition;
	pColor = iPosition.xyz;
	gl_Position = uVP * pPosition;
}