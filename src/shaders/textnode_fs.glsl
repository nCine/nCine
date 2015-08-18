#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D texture;
uniform vec4 color;
varying vec2 vTexCoords;

void main()
{
	gl_FragColor = color;
	gl_FragColor.a *= texture2D(texture, vTexCoords).r;
}
