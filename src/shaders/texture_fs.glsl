#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D texture;
varying vec2 vTexCoords;

void main()
{
	gl_FragColor = texture2D(texture, vTexCoords);
}
