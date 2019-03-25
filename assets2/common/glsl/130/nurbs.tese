#version 400

layout (triangles, fractional_odd_spacing, ccw) in;

in vec4 kPosition[];
in vec3 kNormal[];

out vec4 tPosition;
out vec3 tNormal;
out vec3 tView;
out vec3 tColor;

uniform mat4 uVP;
uniform vec3 uCamera;

vec4 interpolate(vec4 a, vec4 b, vec4 c, vec3 weights) {
	return a*weights.x + b*weights.y + c*weights.z;
}

void main() {
	vec4 position = interpolate(kPosition[0], kPosition[1], kPosition[2], gl_TessCoord);
	gl_Position = uVP * position;
	tNormal = kNormal[0] + kNormal[1] + kNormal[2];
	tView = uCamera - position.xyz;
	tColor = gl_TessCoord;
}