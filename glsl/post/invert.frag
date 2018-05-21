#version 450

in vec2 pass_texcoord;

layout (binding = 0) uniform sampler2D color_buffer;
layout (binding = 1) uniform sampler2D depth_buffer;

out vec4 fragColor;

void main() {
	fragColor = vec4(1.0f) - texture(color_buffer, pass_texcoord);
	fragColor.w = 1.0f;
}