#version 150

in vec3 pNormal;
in vec2 pTexCoord;

out vec4 oFragColor;

uniform sampler2D uTexture;
uniform float uRimFactor;

const vec3 lightvec = vec3(0.3520,0.2304,0.9072);
const vec3 rimvec = normalize(vec3(0.0, 1.05, -2.0));
const vec3 rimcolor = vec3(0.0, 0.5, 1.0);

void main(void)
{
	vec3 diffuse = texture2D(uTexture, pTexCoord).rgb;
	float light = dot(pNormal, lightvec) * 0.5 + 0.5;
	float rim = max(dot(pNormal, rimvec) * (1.0 - uRimFactor) + uRimFactor, 0.0);
	oFragColor.rgb = rim * rimcolor + light * diffuse;
	oFragColor.a = 1.0;
}