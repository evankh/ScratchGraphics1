#version 150

in vec4 pPosition;
in vec3 pNormal;
in vec2 pTexCoord;
in vec3 pView;

out vec4 oFragColor;

const vec3 lightPos = vec3(-5.0, 5.0, 3.0);

void main() {
	vec3 view = normalize(pView);
	vec3 lightDir = normalize(lightPos - pPosition.xyz);
	float diffuse = max(0.0, dot(pNormal, lightDir));
	vec3 reflection = reflect(-lightDir, pNormal);
	float angle = max(0.0, dot(reflection, view));
	float specular = pow(angle, 16.0);
	oFragColor.rgb = vec3(diffuse + specular + 0.025);
	oFragColor.a = 1.0;
}