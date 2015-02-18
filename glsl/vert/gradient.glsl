#version 150

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 texMatrix;

in  vec3 position;
in  vec2 uv;

out vec2 st;

void main() {
    // Set the position of the current vertex
    gl_Position =  vec4(projMatrix * viewMatrix * vec4(position, 1.0));
    st          =  vec2(texMatrix * vec4(uv, 0.0, 0.0));
}   
