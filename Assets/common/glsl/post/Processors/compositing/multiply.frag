#version 450

#define MAX_TEXTURES 7

in vec2 pTexCoord;

out vec4 oFragColor;

uniform unsigned int uNumTextures;
uniform sampler2D uTexture[MAX_TEXTURES];

void main() {
	vec3 composite = vec3(1.0);
	// Multiplicative compositing
	for (int i = 0; i < MAX_TEXTURES && i < uNumTextures; i++) {
		composite *= texture(uTexture[i], pTexCoord).rgb;
	}
	oFragColor.rgb = composite;
	oFragColor.a = 1.0;
}