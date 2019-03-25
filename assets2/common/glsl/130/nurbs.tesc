#version 400

layout (vertices = 3) out;

in vec4 pPosition[];
in vec3 pNormal[];

out vec4 kPosition[];
out vec3 kNormal[];

void main() {
	gl_TessLevelOuter[0] = 5.0;	// These are probably where to stick my uniforms
	gl_TessLevelOuter[1] = 5.0;
	gl_TessLevelOuter[2] = 5.0;
	gl_TessLevelInner[0] = 3.0;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	kPosition[gl_InvocationID] = pPosition[gl_InvocationID];
	kNormal[gl_InvocationID] = pNormal[gl_InvocationID];
}