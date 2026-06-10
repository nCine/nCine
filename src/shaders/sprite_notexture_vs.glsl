uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

layout (std140) uniform InstanceBlock
{
	vec4 transform;
	vec4 translation;
	uint color;
	uint spriteSize;
};

out vec4 vColor;

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
	vec2 aPosition = vec2(0.5 - float(gl_VertexID >> 1), -0.5 + float(gl_VertexID & 1));
	vec2 unpackedSpriteSize = unpackSpriteSize(spriteSize);
	vec4 position = vec4(aPosition.x * unpackedSpriteSize.x, aPosition.y * unpackedSpriteSize.y, 0.0, 1.0);

	vec2 worldPos = vec2(
		transform.x * position.x + transform.y * position.y,
		transform.z * position.x + transform.w * position.y
	) + translation.xy;

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(worldPos, translation.z, 1.0);
	vColor = unpackColor(color);
}
