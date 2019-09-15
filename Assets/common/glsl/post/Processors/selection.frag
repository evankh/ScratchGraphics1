#version 150

in vec2 pTexCoord;

out vec4 oFragColor;

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
/*uniform sampler2D uTexture3;
uniform sampler2D uTexture4;
uniform sampler2D uTexture5;
uniform sampler2D uTexture6;
uniform sampler2D uTexture7;*/
uniform unsigned int uSelector;

void main() {
	vec4 samples[3] = vec4[](texture(uTexture0, pTexCoord),
				 texture(uTexture1, pTexCoord),
				 texture(uTexture2, pTexCoord)/*,
				 texture(uTexture3, pTexCoord),
				 texture(uTexture4, pTexCoord),
				 texture(uTexture5, pTexCoord),
				 texture(uTexture6, pTexCoord),
				 texture(uTexture7, pTexCoord)*/);
	oFragColor = samples[uSelector];
}