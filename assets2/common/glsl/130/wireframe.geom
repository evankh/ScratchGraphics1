#version 150

layout (triangles) in;
layout (line_strip, max_vertices = 4) out; 
/*
// These damn name conflicts make it impossible to pop a shader stage in between two already-existing ones
in vec4 tPosition[];
in vec3 tNormal[];
*/
in PassData {
	vec4 position;
	vec3 normal;
	vec3 view;
	vec3 color;
} pIn[];

out PassData {
	vec4 position;
	vec3 normal;
	vec3 view;
	vec3 color;
} pOut;

uniform mat4 uVP;
uniform vec3 uCamera;

void main() {
	pOut.position = pIn[0].position;
	pOut.view = uCamera - pOut.position.xyz;
	gl_Position = uVP * pIn[0].position;
	pOut.normal = pIn[0].normal;
	EmitVertex();
	
	pOut.position = pIn[1].position;
	pOut.view = uCamera - pOut.position.xyz;
	gl_Position = uVP * pIn[1].position;
	pOut.normal = pIn[1].normal;
	EmitVertex();
	
	pOut.position = pIn[2].position;
	pOut.view = uCamera - pOut.position.xyz;
	gl_Position = uVP * pIn[2].position;
	pOut.normal = pIn[2].normal;
	EmitVertex();
	
	pOut.position = pIn[0].position;
	pOut.view = uCamera - pOut.position.xyz;
	gl_Position = uVP * pIn[0].position;
	pOut.normal = pIn[0].normal;
	EmitVertex();
	
	EndPrimitive();
}