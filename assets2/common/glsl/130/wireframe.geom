#version 150

layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

in PassData {
	vec4 position;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
	vec3 view;
} pIn[];

out PassData {
	vec4 position;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
	vec3 view;
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