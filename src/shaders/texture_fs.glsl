#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uTexture;
uniform vec4 uColor;
in vec2 vTexCoords;
out vec4 fragColor;

void main()
{
	fragColor = texture(uTexture, vTexCoords) * uColor;
}
