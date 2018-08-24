uniform mat4 projection;
in vec2 aPosition;
in vec2 aTexCoords;
in vec4 aColor;
out vec2 vTexCoords;
out vec4 vColor;

void main()
{
	gl_Position = projection * vec4(aPosition, 0.0, 1.0);
	vTexCoords = aTexCoords;
	vColor = aColor;
}
