#version 120

uniform float uWiggleAmplitude = 0.15;
uniform float uWiggleFrequency = 3.0;
uniform float uRadius = 0.5;

float ScalarField(vec3 p) {
	float freq = uWiggleFrequency * radians(360.0);
	p.x += uWiggleAmplitude * cos(freq * p.z);
	p.y += uWiggleAmplitude * sin(freq * p.z);
	p.xy *= 1.0 + 4.0 * uWiggleAmplitude;
	return length(p) - uRadius;
}

vec3 GradientField(vec3 p) {
	float g = ScalarField(p);
	float ddg = 0.5 / sqrt(g);
	float B = 1.0 + 4.0 * uWiggleAmplitude;
	B *= B;
	float A = uWiggleAmplitude;
	float F = uWiggleFrequency * radians(360.0);
	vec3 result;
	result.x = ddg * (2*p.x + 2*A*cos(F*p.z)) * B;
	result.y = ddg * (2*p.y + 2*A*sin(F*p.z)) * B;
	result.z = ddg * (2*p.z + (-2*p.x*A*F*sin(F*p.z) + 2*p.y*A*F*cos(F*p.z)) * B);
	return result;
}

float SDF(vec3 p) {
	float center = ScalarField(p);
	vec3 grad = GradientField(p);
	return center / length(grad);
}