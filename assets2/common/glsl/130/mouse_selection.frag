#version 330

in PassData {
	vec4 position;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
	vec3 view;
} pIn;
in vec3 pLocal;

// The possibilities here are really endless; I could do another one with object space coordinates
// or texture coordinates so the object could tell what part was being clicked on
// Or more... Texture painting, vertex manipulation...

layout (location = 0) out uint oObjectID;
layout (location = 1) out vec4 oPosition;
layout (location = 2) out vec4 oLocalPosition;

uniform uint uObjectID;

void main() {
	oObjectID = uObjectID;
//	oObjectID += 4000000000u;
	//oPosition = pIn.position;
	//oLocalPosition = pLocal;
	oPosition = vec4(1.0, 0.5, 0.0, 1.0);
	oLocalPosition = vec4(0.0, 0.5, 1.0, 1.0);
}