
varying vec2 		    v_texCoord0;
uniform sampler2D 		s_texture0;

void main()
{
	gl_FragColor = texture2D(s_texture0, v_texCoord0);
}
