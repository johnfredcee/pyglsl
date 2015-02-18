#version 150

uniform sampler2D tex0;

uniform  vec4 red_color;
uniform  vec4 blue_color;
uniform  vec4 green_color;
uniform  vec4 alpha_color;

uniform float alpha;
uniform float tension;

in  vec2 st;

out vec4 fcolor;

float spline(float t, vec4 v, float tension)
{
    float M12 =    1.0;
    float M21 =   -tension;
    float M23 =    tension;
    float M31 =    2.0 * tension;
    float M32 =    tension - 3.0;
    float M33 =    3.0 - 2.0 * tension;
    float M34 =   -tension;
    float M41 =   -tension;
    float M42 =    2.0 - tension;
    float M43 =   tension - 2.0;
    float M44 =   tension;

    float c1 = M12*v.y;
    float c2 = M21*v.x          + M23*v.z;
    float c3 = M31*v.x + M32*v.y + M33*v.z + M34*v.w;
    float c4 = M41*v.x + M42*v.y + M43*v.z + M44*v.w;

    float interpolant = c1 + ( c2 * t ) + ( c3 * t * t ) + ( c4 * t * t * t );
	return interpolant;
}

void main() {
	// Set the output color of our current pixel
	vec4 sample = texture2D(tex0, st);
	fcolor = vec4(spline(alpha, red_color, tension),
				  spline(alpha, blue_color, tension),
				  spline(alpha, green_color, tension),
				  spline(alpha, alpha_color, tension));
}
