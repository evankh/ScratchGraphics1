#version 450

in vec3 pass_color_g;
in vec3 pass_normal;
in vec3 pass_worldcoord;

layout (location = 16) uniform vec3 lightPos;

out vec4 fragColor;

void main() {
	vec3 normal = normalize(pass_normal);
	vec3 lightVec = normalize(vec3(3,4,5) - pass_worldcoord);
	float diffuse = dot(normal, lightVec) * 0.5 + 0.5;
	fragColor.rgb = vec3(diffuse) * pass_color_g;
	fragColor.a = 1.0f;
}