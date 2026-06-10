uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

in vec4 aTransform;
in vec4 aTranslation;
in uint aColor;
in uint aSpriteSize;
in uint aUvEndpointsU;
in uint aUvEndpointsV;

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
	int vertexID = gl_VertexID & 3; // same as % 4
	vec2 aPosition = vec2(0.5 - float(vertexID >> 1), -0.5 + float(vertexID & 1));
	vec2 aTexCoords = vec2(1.0 - float(vertexID >> 1), 1.0 - float(vertexID & 1));
	vec2 unpackedSpriteSize = unpackSpriteSize(aSpriteSize);
	vec4 position = vec4(aPosition.x * unpackedSpriteSize.x, aPosition.y * unpackedSpriteSize.y, 0.0, 1.0);

	vec2 worldPos = vec2(
		aTransform.x * position.x + aTransform.y * position.y,
		aTransform.z * position.x + aTransform.w * position.y
	) + aTranslation.xy;

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(worldPos, aTranslation.z, 1.0);
	vec4 uvEndpoints = unpackUVEndpoints(aUvEndpointsU, aUvEndpointsV);
	vTexCoords = vec2(mix(uvEndpoints.x, uvEndpoints.y, aTexCoords.x), mix(uvEndpoints.z, uvEndpoints.w, aTexCoords.y));
	vColor = unpackColor(aColor);
}
