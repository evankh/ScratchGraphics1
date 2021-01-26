#version 400

layout (quads, fractional_even_spacing, ccw) in;

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
	return a*weights.x + b*weights.y + c*weights.z;
}
float interpolate(float a, float b, float c, vec3 weights) {
	return a*weights.x + b*weights.y + c*weights.z;
}

unsigned int factorial[6] = unsigned int[6](1u, 1u, 2u, 6u, 24u, 120u);

unsigned int binom(unsigned int n, unsigned int i) {
	return factorial[n] / (factorial[i] * factorial[n-i]);
}

float B(unsigned int n, unsigned int i, float u) {
	return binom(n, i) * pow(u, i) * pow(1.0 - u, n - i);
}

//unsigned int index[16] = unsigned int[16](20u, 12u, 13u, 29u, 23u, 15u, 14u, 30u, 4u, 5u, 0u, 1u, 7u, 6u, 3u, 2u);

// 31 is the highest allowed index
void main() {
	vec4 position = vec4(0.0);
	for (unsigned int i = 0; i <= 3; i++) {
		for (unsigned int j = 0; j <= 3; j++) {
			position += B(3, i, gl_TessCoord.x) * B(3, j, gl_TessCoord.y) * pIn[4 * j + i].position;
		}
	}
	pOut.position = position;
	pOut.color = gl_TessCoord;
	pOut.normal = vec3(0.0,0.0,1.0);	// Finding properly interpolated normals is going to be difficult, there's probably a formula for it I can look up somewhere
	pOut.texCoord = gl_TessCoord.xy;
	gl_Position = uVP * pOut.position;
	pOut.view = uCamera - pOut.position.xyz;
}