#version 150

in vec4 pPosition;
in vec3 pColor;
in vec3 pNormal;
in vec2 pTexCoord;
in vec3 pView;

out vec4 oFragColor;

const vec3 lightvec = vec3(0.3520,0.2304,0.9072);
const vec3 diffuse = vec3(0.3,0.3,0.3);
const vec3 specular = vec3(0.85,0.85,0.85);
const vec3 ambient = vec3(0.05,0.05,0.05);

void main() {
	vec3 view = normalize(pView);

	float diff = dot(pNormal, lightvec);

	vec3 r = 2.0 * diff * pNormal - lightvec;
	r = normalize(r);
	float spec = dot(view, r);
	spec = max(0.0,spec);
	spec *= spec;
	oFragColor.rgb = diff*diffuse + spec*specular + ambient;
	oFragColor.a = 1.0f;
}