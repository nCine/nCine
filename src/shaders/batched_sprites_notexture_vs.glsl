uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

struct SpriteInstance
{
	mat4 modelMatrix;
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

	gl_Position = uProjectionMatrix * uViewMatrix * i.modelMatrix * position;
	vColor = i.color;
}
