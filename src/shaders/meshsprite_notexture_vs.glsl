uniform mat4 projection;

layout (std140) uniform MeshSpriteBlock
{
	mat4 modelView;
	vec4 color;
	vec2 spriteSize;
};

in vec2 aPosition;
out vec4 vColor;

void main()
{
	vec4 position = vec4(aPosition.x * spriteSize.x, aPosition.y * spriteSize.y, 0.0, 1.0);

	gl_Position = projection * modelView * position;
	vColor = color;
}
