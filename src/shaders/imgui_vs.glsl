uniform mat4 uGuiProjection;
uniform float uDepth;
in vec2 aPosition;
in vec2 aTexCoords;
in vec4 aColor;
out vec2 vTexCoords;
out vec4 vColor;

void main()
{
	gl_Position = uGuiProjection * vec4(aPosition, uDepth, 1.0);
	vTexCoords = aTexCoords;
	vColor = aColor;
}
