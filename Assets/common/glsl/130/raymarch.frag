#version 150
#extension GL_ARB_conservative_depth : enable

in vec3 pPosition;
in vec3 pRay;

out vec4 oFragColor;
layout (depth_greater) out float gl_FragDepth;	// Raymarching will only move the point farther away than the surface of the containing mesh

// Scene
const vec3 lightDirection = vec3(0.3520, 0.2304, 0.9072);

uniform mat4 uM;
uniform mat4 uInverseM;
uniform mat4 uVP;

// Rendering
uniform float uEps = 0.0001;
uniform int uMaxIterations = 500;
uniform float uStepSize = 0.5;
uniform int uMaxReflections = 4;
uniform float uFogNear = 75.0;
uniform float uFogFar = 200.0;

float depthNDCToWindow(float z) {
	return (gl_DepthRange.diff * z + gl_DepthRange.far + gl_DepthRange.near) / 2.0;
}

float SDF(vec3);
vec3 Normal(vec3);
vec2 TexCoord(vec3);

void main() {
	gl_FragDepth = gl_FragCoord.z;
	vec3 p = pPosition;
	vec3 ray = normalize(pRay);
	float min_dist = SDF(p);
	if (min_dist < 0.0) {
		oFragColor.rgb = vec3(1.0, 0.0, 0.0);
		oFragColor.a = 1.0;
		vec4 proj = (uVP * uM) * vec4(p, 1.0);
		gl_FragDepth = depthNDCToWindow(proj.z / proj.w);
		return;
	}
	for (int i = 0; i < uMaxIterations; i++) {
		float this_dist = SDF(p);
		min_dist = min(min_dist, this_dist);
		if (this_dist <= uEps) break;
		p += ray * uStepSize * this_dist;
	}
	if (SDF(p) <= uEps) {
		vec3 normal = Normal(p);
		// Shading is all done in world space, so need to transform the position & normal vectors to their real coordinates
		normal = normalize((transpose(uInverseM) * vec4(normal, 1.0)).xyz);
		p = (uM * vec4(p, 1.0)).xyz;
		float shadeFac = clamp(dot(normal, lightDirection), 0.0, 1.0);
		vec4 proj = uVP * vec4(p, 1.0);
		gl_FragDepth = depthNDCToWindow(proj.z / proj.w);
		oFragColor.rgb = vec3(shadeFac);
		oFragColor.a = 1.0;
	}
	else {
		// Other, better options include setting the alpha
		vec4 proj = uVP * uM * vec4(p, 1.0);
		gl_FragDepth = depthNDCToWindow(proj.z / proj.w);
		oFragColor = vec4(0.0, 0.0, 1.0, 0.5);
//		discard;
	}
	// To do eventually: depth testing against the back of the volume to cancel the ray marching
}