#version 150

in vec2 pTexCoord;

out vec4 oFragColor;

uniform sampler2D uColorBuffer;
uniform sampler2D uDepthBuffer;

const vec3 Y709 = vec3(0.2126, 0.7152, 0.0722);
const vec3 Y601 = vec3(0.2989, 0.5870, 0.1140);

void main() {
	vec4 sample = texture(uColorBuffer, pTexCoord);
	float luma = dot(sample.rgb, Y709);
	oFragColor = smoothstep(0.25, 0.75, luma) * sample;
	oFragColor.w = 1.0f;
}