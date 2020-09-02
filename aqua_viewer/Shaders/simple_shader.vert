
//constants:
uniform mat4 u_modelViewProj;

// inputs:
attribute vec4 a_position;

void main() {

	gl_Position = u_modelViewProj * a_position;
}

