uniform mat4 projection;
uniform mat4 view;

layout (std140) uniform SpriteBlock
{
	mat4 modelView;
	vec4 color;
	vec2 spriteSize;
};

out vec4 vColor;

void main()
{
	vec2 aPosition = vec2(0.5 - float(gl_VertexID >> 1), -0.5 + float(gl_VertexID % 2));
	vec4 position = vec4(aPosition.x * spriteSize.x, aPosition.y * spriteSize.y, 0.0, 1.0);

	gl_Position = projection * view * modelView * position;
	vColor = color;
}
