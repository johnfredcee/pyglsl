#version 150 

uniform mat4 guiMatrix;

in vec2 position;
in vec3 normal;
in vec2 uv;

out vec2 st;
out vec3 norm;

void main()
{
	float x = (position.x / 400.0f) - 1.0f;
	float y = (position.y / 300.0f) - 1.0f;
	gl_Position = vec4(guiMatrix * vec4(x, y, 0.5, 1.0));
	norm = normal;
	st     =  uv;
}
	
