#version 450

in vec2 pass_texcoord[11];

layout (binding = 0) uniform sampler2D color_buffer;
layout (binding = 1) uniform sampler2D depth_buffer;

out vec4 fragColor;

const float kernel[11] = float[](0.010000020102080308,
0.029100872215875997,
0.06679148652061107,
0.12090553500182073,
0.17261607109179009,
0.19436866071302797,
0.17261607109179009,
0.12090553500182073,
0.06679148652061107,
0.029100872215875997,
0.010000020102080308);	// If we make this a uniform, we can get away with only one kernel convolution fragment shader for each kernel size

void main() {
	fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 11; i++)
		fragColor += kernel[i] * texture(color_buffer, pass_texcoord[i]);
	fragColor.w = 1.0f;
}