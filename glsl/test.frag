#version 450

out vec4 fragColor;

in vec3 pass_color;

void main() {
	fragColor.rgb = pow(pass_color, vec3(2.2));
	fragColor.a = 1.0;
}