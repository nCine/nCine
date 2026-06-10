uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

struct Instance
{
	vec4 transform;
	vec4 translation;
	uint color;
	uint spriteSize;
};

layout (std140) uniform InstancesBlock
{
#ifndef BATCH_SIZE
	#define BATCH_SIZE (1365) // 64 KiB / 48 B
#endif
	Instance[BATCH_SIZE] instances;
} block;

in vec2 aPosition;
in uint aMeshIndex;
out vec4 vColor;

#define i block.instances[aMeshIndex]

vec2 unpackSpriteSize(uint ss)
{
	return vec2(float(ss & 0xFFFFu), float((ss >> 16u) & 0xFFFFu));
}

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
	vec2 unpackedSpriteSize = unpackSpriteSize(i.spriteSize);
	vec4 position = vec4(aPosition.x * unpackedSpriteSize.x, aPosition.y * unpackedSpriteSize.y, 0.0, 1.0);

	vec2 worldPos = vec2(
		i.transform.x * position.x + i.transform.y * position.y,
		i.transform.z * position.x + i.transform.w * position.y
	) + i.translation.xy;

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(worldPos, i.translation.z, 1.0);
	vColor = unpackColor(i.color);
}
