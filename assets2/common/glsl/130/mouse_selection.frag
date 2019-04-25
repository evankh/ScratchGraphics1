#version 330

in PassData {
	vec4 position;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
	vec3 view;
} pIn;

// The possibilities here are really endless; I could do another one with object space coordinates
// or texture coordinates so the object could tell what part was being clicked on
// Or more... Texture painting, vertex manipulation...

layout (location = 0) out uint oObjectID;
layout (location = 1) out vec4 oPosition;

uniform uint uObjectID;

void main() {
	//oObjectID = uObjectID;
	oObjectID = 1u;
	//oPosition = pIn.position;
	oPosition = vec4(1.0);
}