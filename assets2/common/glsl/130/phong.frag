#version 150

in PassData {
	vec4 position;
	vec3 normal;
	vec3 view;
	vec3 color;
} pIn;

out vec4 oFragColor;

const vec3 lightPos = vec3(-5.0, 5.0, 3.0);

void main() {
	vec3 view = normalize(pIn.view);
	vec3 normal = normalize(pIn.normal);
	vec3 lightDir = normalize(lightPos - pIn.position.xyz);
	float diffuse = max(0.0, dot(normal, lightDir));
	vec3 reflection = reflect(-lightDir, normal);
	float angle = max(0.0, dot(reflection, view));
	float specular = pow(angle, 16.0);
	oFragColor.rgb = vec3(diffuse + specular + 0.25) * pIn.color;
	oFragColor.a = 1.0;
}