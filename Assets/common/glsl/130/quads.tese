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

// 31 is the highest allowed index
void main() {
	pOut.position = mix(
		mix(pIn[0].position, pIn[1].position, gl_TessCoord.x),
		mix(pIn[3].position, pIn[2].position, gl_TessCoord.x),
		gl_TessCoord.y);
	pOut.color = mix(
		mix(pIn[0].color, pIn[1].color, gl_TessCoord.x),
		mix(pIn[3].color, pIn[2].color, gl_TessCoord.x),
		gl_TessCoord.y);
	pOut.normal = mix(
		mix(pIn[0].normal, pIn[1].normal, gl_TessCoord.x),
		mix(pIn[3].normal, pIn[2].normal, gl_TessCoord.x),
		gl_TessCoord.y);
	pOut.texCoord = mix(
		mix(pIn[0].texCoord, pIn[1].texCoord, gl_TessCoord.x),
		mix(pIn[3].texCoord, pIn[2].texCoord, gl_TessCoord.x),
		gl_TessCoord.y);
	gl_Position = uVP * pOut.position;
	pOut.view = uCamera - pOut.position.xyz;
}