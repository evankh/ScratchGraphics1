#version 450

layout (location = 0) in vec4 position;
layout (location = 2) in vec4 color;

out vec4 pass_color;

void main() {
	gl_Position = position;
	pass_color = vec4(1.0);
}