uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

struct Instance
{
	vec4 transform;
	vec4 translation;
	uint color;
};

layout (std140) uniform InstancesBlock
{
#ifndef BATCH_SIZE
	#define BATCH_SIZE (1365) // 64 KiB / 48 B
#endif
	Instance[BATCH_SIZE] instances;
} block;

in vec2 aPosition;
in vec2 aTexCoords;
in uint aMeshIndex;
out vec2 vTexCoords;
out vec4 vColor;

#define i block.instances[aMeshIndex]

vec4 unpackColor(uint c)
{
	return vec4(
		float((c >> 0) & 0xFFu),
		float((c >> 8) & 0xFFu),
		float((c >> 16) & 0xFFu),
		float((c >> 24) & 0xFFu)
	) * (1.0 / 255.0);
}

void main()
{
	vec2 worldPos = vec2(
		i.transform.x * aPosition.x + i.transform.y * aPosition.y,
		i.transform.z * aPosition.x + i.transform.w * aPosition.y
	) + i.translation.xy;

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(worldPos, i.translation.z, 1.0);
	vTexCoords = aTexCoords;
	vColor = unpackColor(i.color);
}
