#version 450

in vec2 pass_texcoord[7];

layout (binding = 0) uniform sampler2D color_buffer;
layout (binding = 1) uniform sampler2D depth_buffer;

out vec4 fragColor;

const float kernel[7] = float[](0.00443185, 0.053991, 0.241971, 0.398942, 0.241971, 0.053991, 0.00443185);

void main() {
	fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 7; i++)
		fragColor += kernel[i] * texture(color_buffer, pass_texcoord[i]);
	fragColor.w = 1.0f;
}