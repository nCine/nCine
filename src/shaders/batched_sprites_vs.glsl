uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

struct Instance
{
	vec4 transform;
	vec4 translation;
	uint color;
	uint spriteSize;
	uint uvEndpointsU;
	uint uvEndpointsV;
};

layout (std140) uniform InstancesBlock
{
#ifndef BATCH_SIZE
	#define BATCH_SIZE (1365) // 64 KiB / 48 B
#endif
	Instance[BATCH_SIZE] instances;
} block;

out vec2 vTexCoords;
out vec4 vColor;

#define i block.instances[gl_VertexID / 6]

vec2 unpackSpriteSize(uint ss)
{
	return vec2(float(ss & 0xFFFFu), float((ss >> 16u) & 0xFFFFu));
}

vec4 unpackUVEndpoints(uint epU, uint epV)
{
	const float InvU16 = 1.0 / 65535.0;

	return vec4(
		float(epU & 0xFFFFu) * InvU16,
		float(epU >> 16u) * InvU16,
		float(epV & 0xFFFFu) * InvU16,
		float(epV >> 16u) * InvU16
	);
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
	vec2 aPosition = vec2(-0.5 + float(((gl_VertexID + 2) / 3) & 1), 0.5 - float(((gl_VertexID + 1) / 3) & 1));
	vec2 aTexCoords = vec2(float(((gl_VertexID + 2) / 3) & 1), float(((gl_VertexID + 1) / 3) & 1));
	vec2 unpackedSpriteSize = unpackSpriteSize(i.spriteSize);
	vec4 position = vec4(aPosition.x * unpackedSpriteSize.x, aPosition.y * unpackedSpriteSize.y, 0.0, 1.0);

	vec2 worldPos = vec2(
		i.transform.x * position.x + i.transform.y * position.y,
		i.transform.z * position.x + i.transform.w * position.y
	) + i.translation.xy;

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(worldPos, i.translation.z, 1.0);
	vec4 uvEndpoints = unpackUVEndpoints(i.uvEndpointsU, i.uvEndpointsV);
	vTexCoords = vec2(mix(uvEndpoints.x, uvEndpoints.y, aTexCoords.x), mix(uvEndpoints.z, uvEndpoints.w, aTexCoords.y));
	vColor = unpackColor(i.color);
}
