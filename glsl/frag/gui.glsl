#version 150 compatibility

uniform sampler2D tex0;

in  vec2 st;
in  vec3 norm;
out vec4 fcolor;

void main() {
	// Set the output color of our current pixel
	fcolor = texture2D(tex0, st);
	fcolor = fcolor * vec4(norm, 1.0);
}
