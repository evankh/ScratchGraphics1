#version 450

in vec2 pass_texcoord[9];

layout (binding = 0) uniform sampler2D color_buffer;
layout (binding = 1) uniform sampler2D depth_buffer;

out vec4 fragColor;

layout (location = 2) uniform float kernel[9];

void main() {
	fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 9; i++)
		fragColor += kernel[i] * texture(color_buffer, pass_texcoord[i]);
	fragColor = 0.125 * fragColor + 0.5;
	fragColor.w = 1.0f;
}