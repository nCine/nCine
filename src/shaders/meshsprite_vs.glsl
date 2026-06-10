uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

layout (std140) uniform InstanceBlock
{
	vec4 transform;
	vec4 translation;
	uint color;
	uint spriteSize;
	uint uvEndpointsU;
	uint uvEndpointsV;
};

in vec2 aPosition;
in vec2 aTexCoords;
out vec2 vTexCoords;
out vec4 vColor;

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
	vec2 unpackedSpriteSize = unpackSpriteSize(spriteSize);
	vec4 position = vec4(aPosition.x * unpackedSpriteSize.x, aPosition.y * unpackedSpriteSize.y, 0.0, 1.0);

	vec2 worldPos = vec2(
		transform.x * position.x + transform.y * position.y,
		transform.z * position.x + transform.w * position.y
	) + translation.xy;

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(worldPos, translation.z, 1.0);
	vec4 uvEndpoints = unpackUVEndpoints(uvEndpointsU, uvEndpointsV);
	vTexCoords = vec2(mix(uvEndpoints.x, uvEndpoints.y, aTexCoords.x), mix(uvEndpoints.z, uvEndpoints.w, aTexCoords.y));
	vColor = unpackColor(color);
}
