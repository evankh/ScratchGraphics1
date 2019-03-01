#version 450

#define NUM_TEXTURES 3

in vec2 pTexCoord;

out vec4 oFragColor;

uniform sampler2D uTexture[NUM_TEXTURES];

void main() {
	vec3 composite = vec3(0.0);
	// Screen compositing
	/*for (int i = 0; i < NUM_TEXTURES; i++) {
		composite += texture(uTexture[i], pTexCoord).rgb;
	}
	oFragColor.rgb = composite;*/
	oFragColor.rgb = texture(uTexture[2], pTexCoord).rgb +0.01*texture(uTexture[1], pTexCoord).rgb;
	oFragColor.a = 1.0;
}