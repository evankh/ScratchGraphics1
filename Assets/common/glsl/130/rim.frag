#version 150

in PassData {
	vec4 position;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
	vec3 view;
} pIn;

out vec4 oFragColor;

uniform sampler2D uTexture;
uniform float uRimFactor;

const vec3 lightvec = vec3(0.3520,0.2304,0.9072);
const vec3 rimvec = normalize(vec3(0.0, 1.05, -2.0));
const vec3 rimcolor = vec3(0.0, 0.5, 1.0);

void main(void)
{
	vec3 diffuse = texture2D(uTexture, pIn.texCoord).rgb;
	float light = dot(pIn.normal, lightvec) * 0.5 + 0.5;
	float rim = max(dot(pIn.normal, rimvec) * (1.0 - uRimFactor) + uRimFactor, 0.0);
	oFragColor.rgb = rim * rimcolor + light * diffuse;
	oFragColor.a = 1.0;
}