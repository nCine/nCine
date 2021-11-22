uniform mat4 projection;
uniform mat4 view;

struct SpriteInstance
{
	mat4 modelView;
	vec4 color;
	vec2 spriteSize;
};

layout (std140) uniform InstancesBlock
{
#ifdef WITH_FIXED_BATCH_SIZE
	SpriteInstance[BATCH_SIZE] instances;
#else
	SpriteInstance[682] instances;
#endif
} block;

out vec4 vColor;

#define i block.instances[gl_VertexID / 6]

void main()
{
	vec2 aPosition = vec2(-0.5 + float(((gl_VertexID + 2) / 3) % 2), 0.5 - float(((gl_VertexID + 1) / 3) % 2));
	vec4 position = vec4(aPosition.x * i.spriteSize.x, aPosition.y * i.spriteSize.y, 0.0, 1.0);

	gl_Position = projection * view * i.modelView * position;
	vColor = i.color;
}
