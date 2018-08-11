#version 450

layout (location = 0) in vec4 position;
layout (location = 8) in vec2 texcoord;

out vec2 pass_texcoord[9];

//const float pixel_width = 1.0f / 800.0f;
//const float pixel_height = 1.0f / 600.0f;		// Will be a uniform once resizes are fixed
layout (location = 0) uniform float pixel_width;
layout (location = 1) uniform float pixel_height;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	for (int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++)
			pass_texcoord[i + 1 + 3 * j + 3] = vec2(texcoord.x + i * pixel_width, texcoord.y + j * pixel_height);
}