#version 330 core

uniform sampler2D tex0;

in  vec2 st;
out vec4 final_colors;

void main() {
	// Set the output color of our current pixel
	final_colors = texture(tex0, st);
}