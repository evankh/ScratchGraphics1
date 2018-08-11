#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 pass_color[];
in vec4 pass_position[];

out vec3 pass_normal;
out vec3 pass_color_g;
out vec3 pass_worldcoord;

void main() {
	for (int i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position;
		pass_normal = normalize(cross(pass_position[1].xyz - pass_position[0].xyz, pass_position[2].xyz - pass_position[0].xyz));
		pass_worldcoord = pass_position[i].xyz;
		pass_color_g = pass_color[i];
		EmitVertex();
	}
}