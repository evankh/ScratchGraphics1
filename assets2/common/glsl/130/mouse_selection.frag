#version 330

in vec4 pPosition;

// The possibilities here are really endless; I could do another one with object space coordinates
// or texture coordinates so the object could tell what part was being clicked on
// Or more... Texture painting, vertex manipulation...
layout (location = 0) out int oObjectID;
layout (location = 1) out vec4 oPosition;

uniform int uObjectID;

void main() {
	oObjectID = uObjectID;
	oPosition = vec4(sin(gl_FragCoord.x));
}