#version 120

uniform int uIterationLevel = 6;
const float uScale = 2.0;
const vec3 uOffset = vec3(1.0);

float shape(vec3 p) {
	return length(p) - 1.0;
	p = abs(p);
	return (p.x + p.y + p.z - 1.0) * 0.577;
}

float SDF(vec3 p) {
	p = uScale * p - uOffset * (uScale - 1.0);
	int n = 0;
	while (n < uIterationLevel) {
		if (p.x + p.y < 0.0) p.xy = -p.yx;
		if (p.x + p.z < 0.0) p.xz = -p.zx;
		if (p.y + p.z < 0.0) p.zy = -p.yz;
		p = uScale * p - uOffset * (uScale - 1.0);
		n++;
	}
	return shape(p) * pow(uScale, -float(n));
}