#version 450

layout (location = 0) in vec4 position;
layout (location = 8) in vec2 texcoord;

out vec2 pass_texcoord[3];

layout (location = 0) uniform float pixel_width;
layout (location = 1) uniform float pixel_height;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	for (int i = -1; i < 2; i++)
		pass_texcoord[i + 1] = vec2(texcoord.x, texcoord.y + i * pixel_height);
}