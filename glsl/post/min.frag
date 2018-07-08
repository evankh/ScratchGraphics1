#version 450

in vec2 pass_texcoord[9];

layout (binding = 0) uniform sampler2D color_buffer;
layout (binding = 1) uniform sampler2D depth_buffer;

out vec4 fragColor;

void main() {
	fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 9; i++)
		if (length(texture(color_buffer, pass_texcoord[i])) < length(fragColor))
			fragColor = texture(color_buffer, pass_texcoord[i]);
	fragColor.w = 1.0f;
}