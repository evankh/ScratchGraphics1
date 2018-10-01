#version 150

in vec4 pPosition;
in vec3 pColor;
in vec3 pNormal;
in vec2 pTexCoord;
in vec3 pView;

out vec4 oFragColor;

const vec3 lightvec = vec3(0.3520,0.2304,0.9072);

void main() {
	vec3 n = normalize(pNormal);
	vec3 view = normalize(pView);

	float diff = dot(n, lightvec);
	vec3 r = 2.0 * diff * n - lightvec;
	float spec = dot(view, r);
	oFragColor.rgb = vec3(diff + spec);
	oFragColor.a = 1.0f;
}