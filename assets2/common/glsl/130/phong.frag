#version 150

in vec4 tPosition;
in vec3 tNormal;
in vec3 tView;
in vec3 tColor;

out vec4 oFragColor;

const vec3 lightPos = vec3(-5.0, 5.0, 3.0);

void main() {
	vec3 view = normalize(tView);
	vec3 normal = normalize(tNormal);
	vec3 lightDir = normalize(lightPos - tPosition.xyz);
	float diffuse = max(0.0, dot(normal, lightDir));
	vec3 reflection = reflect(-lightDir, normal);
	float angle = max(0.0, dot(reflection, view));
	float specular = pow(angle, 16.0);
	oFragColor.rgb = vec3(diffuse + specular + 0.25);
	oFragColor.a = 1.0;
}