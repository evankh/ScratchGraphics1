#version 400

layout (quads, fractional_even_spacing, ccw) in;

in PassData {
	vec4 position;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
	vec3 view;
} pIn[];

out PassData {
	vec4 position;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
	vec3 view;
} pOut;

uniform mat4 uVP;
uniform vec3 uCamera;

float factorial[7] = float[7](1., 1., 2., 6., 24., 120., 720.);

float binom(unsigned int n, unsigned int i) {
	return factorial[n] / (factorial[i] * factorial[n-i]);
}

float B(unsigned int n, unsigned int i, float u) {
	return binom(n, i) * pow(u, i) * pow(1.0 - u, n - i);
}

void main() {
	vec4 position = vec4(0.0);
	vec3 color = vec3(0.0);
	float u = gl_TessCoord.y;
	float v = gl_TessCoord.x;
/*
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			position += B(3, i, v) * B(3, j, u) * pIn[4 * i + j].position;
		}
	}//*/
//*
	vec4 p00 = pIn[0].position;
	vec4 p01 = pIn[1].position;
	vec4 p02 = pIn[2].position;
	vec4 p03 = pIn[3].position;
	vec4 p10 = pIn[4].position;
	vec4 p11 = pIn[5].position;
	vec4 p12 = pIn[6].position;
	vec4 p13 = pIn[7].position;
	vec4 p20 = pIn[8].position;
	vec4 p21 = pIn[9].position;
	vec4 p22 = pIn[10].position;
	vec4 p23 = pIn[11].position;
	vec4 p30 = pIn[12].position;
	vec4 p31 = pIn[13].position;
	vec4 p32 = pIn[14].position;
	vec4 p33 = pIn[15].position;
	float bu0 = (1.-u)*(1.-u)*(1.-u);
	float bu1 = 3.*u*(1.-u)*(1.-u);
	float bu2 = 3.*u*u*(1.-u);
	float bu3 = u*u*u;
	float bv0 = (1.-v)*(1.-v)*(1.-v);
	float bv1 = 3.*v*(1.-v)*(1.-v);
	float bv2 = 3.*v*v*(1.-v);
	float bv3 = v*v*v;
	position = bu0 * (bv0*p00+bv1*p01+bv2*p02+bv3*p03)
		 + bu1 * (bv0*p10+bv1*p11+bv2*p12+bv3*p13)
		 + bu2 * (bv0*p20+bv1*p21+bv2*p22+bv3*p23)
		 + bu3 * (bv0*p30+bv1*p31+bv2*p32+bv3*p33);
//*/

	pOut.position = position;
	pOut.color = color;
	pOut.normal = vec3(0.0,0.0,1.0);	// Finding properly interpolated normals is going to be difficult
	pOut.texCoord = gl_TessCoord.xy;
	gl_Position = uVP * pOut.position;
	pOut.view = uCamera - pOut.position.xyz;
}