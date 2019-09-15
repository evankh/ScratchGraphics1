#version 330

in vec3 iPosition;

out vec3 pPosition;
out vec3 pRay;

uniform mat4 uM;
uniform mat4 uVP;
uniform vec3 uCamera;

void main() {
	vec4 worldPos = uM * vec4(iPosition, 1.0);
	pPosition = worldPos.xyz;
	pRay = normalize(pPosition.xyz - uCamera);
	gl_Position = uVP * worldPos;
}