uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

struct Instance
{
	mat4 modelMatrix;
	vec4 color;
};

layout (std140) uniform InstancesBlock
{
#ifndef BATCH_SIZE
	#define BATCH_SIZE (819) // 64 Kb / 80 b
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
	gl_Position = uProjectionMatrix * uViewMatrix * i.modelMatrix * vec4(aPosition, 0.0, 1.0);
	vTexCoords = aTexCoords;
	vColor = i.color;
}
