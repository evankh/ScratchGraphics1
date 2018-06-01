#version 450

layout (location = 0) in vec4 position;
layout (location = 3) in vec4 color;

//layout (location = 0) uniform mat4 mvp;
uniform mat4 m;
uniform mat4 vp;

out vec3 pass_color;
out vec4 pass_position;

void main() {
	pass_position = m * position;
	gl_Position = vp * pass_position;
	pass_color = position.xyz;
}