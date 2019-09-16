#version 120

float SDF(vec3);
uniform float uEps = 0.0001;

vec3 Normal(vec3 p) {
	float eps = 0.0001;
	return normalize(vec3(SDF(p + vec3(eps, 0.0, 0.0)) - SDF(p - vec3(eps, 0.0, 0.0)),
				SDF(p + vec3(0.0, eps, 0.0)) - SDF(p - vec3(0.0, eps, 0.0)),
				SDF(p + vec3(0.0, 0.0, eps)) - SDF(p - vec3(0.0, 0.0, eps))));
}