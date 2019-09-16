#version 120

uniform float uWiggleAmplitude = 0.25;
uniform float uWiggleFrequency = 3.0;
uniform float uRadius = 0.5;

float SDF(vec3 p) {
	float freq = uWiggleFrequency * radians(360.0);
	p.x += uWiggleAmplitude * cos(freq * p.z);
	p.y += uWiggleAmplitude * sin(freq * p.z);
	return length(p) - uRadius;
}