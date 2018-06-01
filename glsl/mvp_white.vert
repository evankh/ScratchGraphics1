#version 450

layout (location = 0) in vec4 position;
layout (location = 3) in vec4 color;

layout (location = 0) uniform mat4 mvp;

out vec3 pass_color;

void main() {
	gl_Position = mvp * position;
	pass_color = vec3(1.0f);
}