#version 120

uniform float uRadius = 0.5;

#define PI 3.141592654
#define PI2 (2 * PI)

float SDF(vec3 p) {
	return length(p) - uRadius;
}

vec3 Normal(vec3 p) {
	return normalize(p);
}

float angle(vec2 p) {
	float ang = acos(p.x / length(p));
	if (p.y >= 0.0)
		return ang;
	return PI2 - ang;
}

vec2 TexCoord(vec3 p) {
	if (length(p) == 0.0) return vec2(0.0);
	return vec2(angle(p.xy) / PI2, asin(p.z / length(p)) / PI + 0.5);
}