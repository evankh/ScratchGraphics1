#version 330

in vec3 iPosition;
layout (location = 2) in vec3 iNormal;
layout (location = 3) in vec3 iColor;
layout (location = 8) in vec2 iTexCoord;

out PassData {
	vec4 position;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
	vec3 view;
} pOut;

uniform mat4 uM = mat4(1.0);
uniform mat4 uVP;
uniform vec3 uCamera;

void main() {
	pOut.position = uM * vec4(iPosition, 1.0);
	pOut.color = iColor;
	pOut.normal = (uM * vec4(iNormal, 0.0)).xyz;
	pOut.texCoord = iTexCoord;
	pOut.view = uCamera - pOut.position.xyz;
	gl_Position = uVP * pOut.position;
}