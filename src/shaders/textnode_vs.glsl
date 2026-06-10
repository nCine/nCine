uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

layout (std140) uniform InstanceBlock
{
	vec4 transform;
	vec4 translation;
	uint color;
};

in vec2 aPosition;
in vec2 aTexCoords;
out vec2 vTexCoords;
out vec4 vColor;

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
		transform.x * aPosition.x + transform.y * aPosition.y,
		transform.z * aPosition.x + transform.w * aPosition.y
	) + translation.xy;

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(worldPos, translation.z, 1.0);
	vTexCoords = aTexCoords;
	vColor = unpackColor(color);
}
