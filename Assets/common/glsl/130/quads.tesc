#version 400

layout (vertices = 4) out;

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
} pOut[];

void main() {
	gl_TessLevelOuter[0] = 1.0;
	gl_TessLevelOuter[1] = 1.0;
	gl_TessLevelOuter[2] = 1.0;
	gl_TessLevelOuter[3] = 1.0;
	gl_TessLevelInner[0] = 1.0;
	gl_TessLevelInner[1] = 1.0;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	pOut[gl_InvocationID].position = pIn[gl_InvocationID].position;
	pOut[gl_InvocationID].normal = pIn[gl_InvocationID].normal;
	pOut[gl_InvocationID].color = pIn[gl_InvocationID].color;
	pOut[gl_InvocationID].texCoord = pIn[gl_InvocationID].texCoord;
	pOut[gl_InvocationID].view = pIn[gl_InvocationID].view;
}