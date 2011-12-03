#version 150

uniform sampler2D tex0;

in  vec2 st;

out vec4 fcolor;

void main() {
	// Set the output color of our current pixel
	fcolor = texture2D(tex0, st);
}