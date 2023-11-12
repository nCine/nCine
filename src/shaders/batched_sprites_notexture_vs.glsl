uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

struct Instance
{
	mat4 modelMatrix;
	vec4 color;
	vec2 spriteSize;
};

layout (std140) uniform InstancesBlock
{
#ifndef BATCH_SIZE
	#define BATCH_SIZE (682) // 64 Kb / 96 b
#endif
	Instance[BATCH_SIZE] instances;
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
