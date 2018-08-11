#version 450

layout (location = 0) in vec4 position;
layout (location = 8) in vec2 texcoord;

out vec2 pass_texcoord[7];

//const float pixel_size = 1.0f / 800.0f;		// Will be a uniform once resizes are fixed
layout (location = 0) uniform float pixel_width;
layout (location = 1) uniform float pixel_height;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	for (int i = -3; i < 4; i++)
		pass_texcoord[i + 3] = vec2(texcoord.x + i * pixel_width, texcoord.y);
}