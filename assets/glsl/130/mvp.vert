#version 330

in vec3 iPosition;
layout (location = 2) in vec3 iNormal;
layout (location = 3) in vec3 iColor;
layout (location = 8) in vec2 iTexCoord;

out vec4 pPosition;
out vec3 pColor;
out vec3 pNormal;
out vec2 pTexCoord;
out vec3 pView;

uniform mat4 uM;
uniform mat4 uVP;
uniform vec4 uCamera;
uniform vec3 uColor;

void main() {
	pPosition = uM * vec4(iPosition, 1.0);
	pColor = uColor;
	pNormal = (uM * vec4(iNormal, 0.0)).xyz;
	pTexCoord = iTexCoord;
	pView = uCamera.xyz - pPosition.xyz;
	gl_Position = uVP * pPosition;
}