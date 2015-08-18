#ifdef GL_ES
precision mediump float;
#endif
uniform vec4 color;

void main()
{
	gl_FragColor = color;
}
