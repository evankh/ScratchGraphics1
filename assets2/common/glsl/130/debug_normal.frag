#version 130

in vec3 pNormal;

out vec4 oFragColor;

const vec3 lightvec = vec3(0.3520,0.2304,0.9072);

void main() {
	float shadeFac = dot(pNormal, lightvec)*0.25 + 0.75;
	oFragColor.rgb = (pNormal*0.5+0.5) * shadeFac;
	oFragColor.a = 1.0f;
}