#version 150

#define NUM_TEXTURES 2

in vec2 pTexCoord;

out vec4 oFragColor;

uniform sampler2D uTexture[NUM_TEXTURES];

void main() {
	vec4 composite = vec4(1.0);
	// Screen compositing
	for (int i = 0; i < NUM_TEXTURES; i++) {
		composite *= 1.0 - texture(uTexture[i], pTexCoord);
	}
	oFragColor = 1.0 - composite;
}