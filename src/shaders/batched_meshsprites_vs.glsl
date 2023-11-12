uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

struct Instance
{
	mat4 modelMatrix;
	vec4 color;
	vec4 texRect;
	vec2 spriteSize;
};

layout (std140) uniform InstancesBlock
{
#ifndef BATCH_SIZE
	#define BATCH_SIZE (585) // 64 Kb / 112 b
#endif
	Instance[BATCH_SIZE] instances;
} block;

in vec2 aPosition;
in vec2 aTexCoords;
in uint aMeshIndex;
out vec2 vTexCoords;
out vec4 vColor;

#define i block.instances[aMeshIndex]

void main()
{
	vec4 position = vec4(aPosition.x * i.spriteSize.x, aPosition.y * i.spriteSize.y, 0.0, 1.0);

	gl_Position = uProjectionMatrix * uViewMatrix * i.modelMatrix * position;
	vTexCoords = vec2(aTexCoords.x * i.texRect.x + i.texRect.y, aTexCoords.y * i.texRect.z + i.texRect.w);
	vColor = i.color;
}
