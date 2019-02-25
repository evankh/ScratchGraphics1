#version 150

in vec3 pPosition;
in vec3 pRay;

out vec4 oFragColor;

// Scene
uniform float uRadius = 0.5;
uniform vec3 uCenter = vec3(0.0, 2.0, 0.0);
const vec3 lightDirection = vec3(0.3520, 0.2304, 0.9072);

// Rendering
uniform float uEps = 0.0001;
uniform int uMaxIterations = 500;
uniform float uStepSize = 0.5;
uniform int uMaxReflections = 4;
uniform float uFogNear = 75.0;
uniform float uFogFar = 200.0;

float distanceFunc(vec3 p) {
	return distance(p, uCenter) - uRadius;
}

vec3 gradient(vec3 p) {
	float eps = 0.0001;
	return normalize(vec3(distanceFunc(p + vec3(eps, 0.0, 0.0)) - distanceFunc(p - vec3(eps, 0.0, 0.0)),
				distanceFunc(p + vec3(0.0, eps, 0.0)) - distanceFunc(p - vec3(0.0, eps, 0.0)),
				distanceFunc(p + vec3(0.0, 0.0, eps)) - distanceFunc(p - vec3(0.0, 0.0, eps))));
}

void main() {
	vec3 start = pPosition;
	vec3 p = pPosition;
	vec3 ray = normalize(pRay);
	float min_dist = distanceFunc(p);
	if (min_dist < 0.0) {
		oFragColor.rgb = vec3(1.0, 0.0, 0.0);
		oFragColor.a = 1.0;
	}
	for (int i = 0; i < uMaxIterations; i++) {
		float this_dist = distanceFunc(p);
		min_dist = min(min_dist, this_dist);
		if (this_dist <= uEps) break;
		p += ray * uStepSize * this_dist;
	}
	if (distanceFunc(p) <= uEps) {
		vec3 normal = gradient(p);
		float shadeFac = clamp(dot(normal, lightDirection), 0.0, 1.0);
		oFragColor.rgb = vec3(shadeFac);
		oFragColor.a = 1.0;
	}
	else
		// Other, better options include setting the alpha
		discard;
	// To do: set depth, for proper interaction with intersecting objects
	// Also to do: depth testing against the back of the volume to cancel the ray marching
}