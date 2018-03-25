#ifdef GL_ES
precision mediump float;
#endif

in vec4 vColor;
out vec4 fragColor;

void main()
{
	fragColor = vColor;
}
