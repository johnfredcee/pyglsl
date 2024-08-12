#version 330 core


in  vec3 position;
in  vec2 uv;
out vec2 st;

uniform WindowBlock 
{                       // This UBO is defined on Window creation, and available
    mat4 projection;    // in all Shaders. You can modify these matrixes with the
    mat4 view;          // Window.view and Window.projection properties.
} window;  


void main() {
    // Set the position of the current vertex
    gl_Position =  vec4(window.projection * window.view * vec4(position, 1.0));
    st          =  uv;  // vec2(texMatrix * vec4(uv, 0.0, 0.0));
}   
    
 