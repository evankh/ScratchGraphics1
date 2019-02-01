#version 330

in vec2 pTexCoord[9];

layout (location = 0) out vec4 oEdgeH;
layout (location = 1) out vec4 oEdgeV;

uniform sampler2D uColorBuffer;
uniform float uKernel0[9];
uniform float uKernel1[9];

void main() {
	vec4 col1 = vec4(0.0f);
	vec4 col2 = vec4(0.0f);
	for (int i = 0; i < 9; i++) {
		vec4 sample = texture(uColorBuffer, pTexCoord[i]);
		col1 += sample * uKernel0[i];
		col2 += sample * uKernel1[i];
	}
	// Sign won't carry over to the next stage in the pipeline ATM, so need to do the abs here
	oEdgeH.rgb = vec3(length(col1.rgb));
	oEdgeH.a = 1.0f;
	oEdgeV.rgb = vec3(length(col2.rgb));
	oEdgeV.a = 1.0f;
}