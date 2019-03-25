#version 400

layout (triangles, fractional_odd_spacing, ccw) in;

in vec4 kPosition[];
in vec3 kNormal[];

out PassData {
	vec4 position;
	vec3 normal;
	vec3 view;
	vec3 color;
} pOut;

uniform mat4 uVP;
uniform vec3 uCamera;

vec4 interpolate(vec4 a, vec4 b, vec4 c, vec3 weights) {
	return a*weights.x + b*weights.y + c*weights.z;
}
vec3 interpolate(vec3 a, vec3 b, vec3 c, vec3 weights) {
	return a*weights.x + b*weights.y + c*weights.z;
}

void main() {
	pOut.position = interpolate(kPosition[0], kPosition[1], kPosition[2], gl_TessCoord);
	gl_Position = uVP * pOut.position;
	pOut.normal = interpolate(kNormal[0], kNormal[1], kNormal[2], gl_TessCoord);
	pOut.view = uCamera - pOut.position.xyz;
	pOut.color = gl_TessCoord;
}