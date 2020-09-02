
//constants:
uniform mat4 u_modelViewProj;

// inputs:
attribute vec4 a_position;
attribute vec2 a_texCoord0;

// outputs:
varying vec2 v_texCoord0;
 

void main() {

	gl_Position = u_modelViewProj * a_position;
	v_texCoord0 = a_texCoord0;

}

