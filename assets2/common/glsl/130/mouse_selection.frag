#version 150

in vec4 pPosition;

// The possibilities here are really endless; I could do another one with object space coordinates
// or texture coordinates so the object could tell what part was being clicked on
// Or more... Texture painting, vertex manipulation...
out int oObjectID;
out vec4 oPosition;

uniform int uObjectID;

void main() {
	oObjectID = uObjectID;
	oPosition = pPosition;
}