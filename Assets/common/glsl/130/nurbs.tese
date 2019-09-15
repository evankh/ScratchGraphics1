#version 400

layout (triangles, fractional_even_spacing, ccw) in;

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

vec4 interpolate(vec4 a, vec4 b, vec4 c, vec3 weights) {
	return a*weights.x + b*weights.y + c*weights.z;
}
vec3 interpolate(vec3 a, vec3 b, vec3 c, vec3 weights) {
	return a*weights.x + b*weights.y + c*weights.z;
}
vec2 interpolate(vec2 a, vec2 b, vec2 c, vec3 weights) {
	return a*weights.x + b*weights.y + b*weights.z;
}
float interpolate(float a, float b, float c, vec3 weights) {
	return a*weights.x + b*weights.y + b*weights.z;
}

void main() {
	pOut.position = interpolate(pIn[0].position, pIn[1].position, pIn[2].position, gl_TessCoord);
	pOut.color = gl_TessCoord;
	pOut.normal = interpolate(pIn[0].normal, pIn[1].normal, pIn[2].normal, gl_TessCoord);
	pOut.texCoord = interpolate(pIn[0].texCoord, pIn[1].texCoord, pIn[2].texCoord, gl_TessCoord);
	gl_Position = uVP * pOut.position;
	pOut.view = uCamera - pOut.position.xyz;
}