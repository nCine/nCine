uniform mat4 projection;

layout (std140) uniform MeshSpriteBlock
{
	mat4 modelView;
	vec4 color;
	vec4 texRect;
	vec2 spriteSize;
};

in vec2 aPosition;
in vec2 aTexCoords;
out vec2 vTexCoords;
out vec4 vColor;

void main()
{
	vec4 position = vec4(aPosition.x * spriteSize.x, aPosition.y * spriteSize.y, 0.0, 1.0);

	gl_Position = projection * modelView * position;
	vTexCoords = vec2(aTexCoords.x * texRect.x + texRect.y, aTexCoords.y * texRect.z + texRect.w);
	vColor = color;
}
