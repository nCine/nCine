uniform mat4 projection;

struct SpriteInstance
{
	mat4 modelView;
	vec4 color;
	vec4 texRect;
	vec2 spriteSize;
};

layout (std140) uniform InstancesBlock
{
#ifdef WITH_FIXED_BATCH_SIZE
	SpriteInstance[BATCH_SIZE] instances;
#else
	SpriteInstance[585] instances;
#endif
} block;

out vec2 vTexCoords;
out vec4 vColor;

#define i block.instances[gl_VertexID / 6]

void main()
{
	vec2 aPosition = vec2(-0.5 + float(((gl_VertexID + 2) / 3) % 2), 0.5 - float(((gl_VertexID + 1) / 3) % 2));
	vec2 aTexCoords = vec2(float(((gl_VertexID + 2) / 3) % 2), float(((gl_VertexID + 1) / 3) % 2));
	vec4 position = vec4(aPosition.x * i.spriteSize.x, aPosition.y * i.spriteSize.y, 0.0, 1.0);

	gl_Position = projection * i.modelView * position;
	vTexCoords = vec2(aTexCoords.x * i.texRect.x + i.texRect.y, aTexCoords.y * i.texRect.z + i.texRect.w);
	vColor = i.color;
}
