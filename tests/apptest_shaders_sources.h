namespace {

char const * const common_functions_vs = R"(
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
)";

char const * const sprite_vs = R"(
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

// Instance data should go in a uniform block called `InstanceBlock` for batching to work
layout (std140) uniform InstanceBlock
{
	vec4 transform;
	vec4 translation;
	float angle;
	uint spriteSize;
	uint uvEndpointsU;
	uint uvEndpointsV;
};

out vec2 vTexCoords;
flat out vec2 vSpriteSize;

void main()
{
	vec2 aPosition = vec2(0.5 - float(gl_VertexID >> 1), -0.5 + float(gl_VertexID & 1));
	vec2 aTexCoords = vec2(1.0 - float(gl_VertexID >> 1), 1.0 - float(gl_VertexID & 1));
	vec2 unpackedSpriteSize = unpackSpriteSize(spriteSize);
	vec4 position = vec4(aPosition.x * unpackedSpriteSize.x, aPosition.y * unpackedSpriteSize.y, 0.0, 1.0);

	vec2 worldPos = vec2(
		transform.x * position.x + transform.y * position.y,
		transform.z * position.x + transform.w * position.y
	) + translation.xy;

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(worldPos, translation.z, 1.0);
	vec2 twistedCoords = vec2((cos(angle) * cos(angle) * 0.85 + 0.15) * aTexCoords.x, (sin(angle) * sin(angle) * 0.85 + 0.15) * aTexCoords.y);
	vec4 uvEndpoints = unpackUVEndpoints(uvEndpointsU, uvEndpointsV);
	vTexCoords = vec2(mix(uvEndpoints.x, uvEndpoints.y, twistedCoords.x), mix(uvEndpoints.z, uvEndpoints.w, twistedCoords.y));
	vSpriteSize = unpackedSpriteSize;
}
)";

char const * const batched_sprite_vs = R"(
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

struct Instance
{
	vec4 transform;
	vec4 translation;
	float angle;
	uint spriteSize;
	uint uvEndpointsU;
	uint uvEndpointsV;
};

// Single instances data will be collected in a uniform block called `InstancesBlock`
layout (std140) uniform InstancesBlock
{
#ifndef BATCH_SIZE
	#define BATCH_SIZE (1365) // 64 KiB / 48 B
#endif
	Instance[BATCH_SIZE] instances;
} block;

out vec2 vTexCoords;
flat out vec2 vSpriteSize;

#define i block.instances[gl_VertexID / 6]

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
	vec2 twistedCoords = vec2((cos(i.angle) * cos(i.angle) * 0.85 + 0.15) * aTexCoords.x, (sin(i.angle) * sin(i.angle) * 0.85 + 0.15) * aTexCoords.y);
	vec4 uvEndpoints = unpackUVEndpoints(i.uvEndpointsU, i.uvEndpointsV);
	vTexCoords = vec2(mix(uvEndpoints.x, uvEndpoints.y, twistedCoords.x), mix(uvEndpoints.z, uvEndpoints.w, twistedCoords.y));
	vSpriteSize = unpackedSpriteSize;
}
)";

char const * const sprite_fs = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uTexture;
in vec2 vTexCoords;
flat in vec2 vSpriteSize;
out vec4 fragColor;

void main()
{
	vec2 onePixel = vec2(1.0 / vSpriteSize.x, 1.0 / vSpriteSize.y);
	vec4 color = vec4(0.5);

	float alpha = texture(uTexture, vTexCoords).a;
	color -= texture(uTexture, vTexCoords - onePixel) * 5.0;
	color += texture(uTexture, vTexCoords + onePixel) * 5.0;
	color.rgb = vec3((color.r + color.g + color.b) / 3.0);

	fragColor = vec4(color.rgb, alpha);
}
)";

char const * const sprite_blur_fs = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uTexture;
uniform vec2 uResolution;
uniform vec2 uDirection;
in vec2 vTexCoords;
out vec4 fragColor;

void main()
{
	vec2 uv = vec2(gl_FragCoord.xy / uResolution.xy);
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.3846153846) * uDirection;
	vec2 off2 = vec2(3.2307692308) * uDirection;

	color += texture(uTexture, uv) * 0.2270270270;
	color += texture(uTexture, uv + (off1 / uResolution)) * 0.3162162162;
	color += texture(uTexture, uv - (off1 / uResolution)) * 0.3162162162;
	color += texture(uTexture, uv + (off2 / uResolution)) * 0.0702702703;
	color += texture(uTexture, uv - (off2 / uResolution)) * 0.0702702703;

	fragColor = color;
}
)";

char const * const meshsprite_vs = R"(
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

// Instance data should go in a uniform block called `InstanceBlock` for batching to work
layout (std140) uniform InstanceBlock
{
	vec4 transform;
	vec4 translation;
	uint color;
	uint spriteSize;
	uint uvEndpointsU;
	uint uvEndpointsV;
};

in vec2 aTexCoords;
in float aDepth;
in vec2 aPosition;
out vec2 vTexCoords;
out vec4 vColor;

void main()
{
	vec2 unpackedSpriteSize = unpackSpriteSize(spriteSize);
	vec4 position = vec4(aPosition.x * unpackedSpriteSize.x, aPosition.y * unpackedSpriteSize.y, aDepth, 1.0);

	vec2 worldPos = vec2(
		transform.x * position.x + transform.y * position.y,
		transform.z * position.x + transform.w * position.y
	) + translation.xy;

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(worldPos, translation.z, 1.0);
	vec4 uvEndpoints = unpackUVEndpoints(uvEndpointsU, uvEndpointsV);
	vTexCoords = vec2(mix(uvEndpoints.x, uvEndpoints.y, aTexCoords.x), mix(uvEndpoints.z, uvEndpoints.w, aTexCoords.y));
	vColor = unpackColor(color);
}
)";

char const * const batched_meshsprite_vs = R"(
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

// Single instances data will be collected in a uniform block called `InstancesBlock`
layout (std140) uniform InstancesBlock
{
#ifndef BATCH_SIZE
	#define BATCH_SIZE (1365) // 64 KiB / 48 B
#endif
	Instance[BATCH_SIZE] instances;
} block;

in vec2 aTexCoords;
in float aDepth;
in vec2 aPosition;
in uint aMeshIndex;
out vec2 vTexCoords;
out vec4 vColor;

#define i block.instances[aMeshIndex]

void main()
{
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
)";

char const * const meshsprite_fs = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uTexture;
in vec2 vTexCoords;
in vec4 vColor;
out vec4 fragColor;

void main()
{
	vec4 color = texture(uTexture, vTexCoords);
	float lum = dot(vec3(0.30, 0.59, 0.11), color.rgb);
	fragColor = vec4(vec3(lum) * vColor.rgb, color.a * vColor.a);
}
)";

char const * const blending_fs = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
in vec2 vTexCoords;
out vec4 fragColor;

void main()
{
	vec3 color = texture(uTexture0, vTexCoords).rgb;
	vec3 bloom = texture(uTexture1, vTexCoords).rgb;
	fragColor = vec4(color + bloom, 1.0);
}
)";

char const * const multitexture_vs = R"(
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

// Instance data should go in a uniform block called `InstanceBlock` for batching to work
layout (std140) uniform InstanceBlock
{
	vec4 transform;
	vec4 translation;
	float rotation;
	uint spriteSize;
	uint uvEndpointsU;
	uint uvEndpointsV;
};

out vec3 vFragPos;
#ifdef __ANDROID__
// Workaround for wrong normalization in fragment shader
out vec3 vFragPosNorm;
#endif
out vec2 vTexCoords;
flat out float vRotation;

void main()
{
	vec2 aPosition = vec2(0.5 - float(gl_VertexID >> 1), -0.5 + float(gl_VertexID & 1));
	vec2 aTexCoords = vec2(1.0 - float(gl_VertexID >> 1), 1.0 - float(gl_VertexID & 1));
	vec2 unpackedSpriteSize = unpackSpriteSize(spriteSize);
	vec4 position = vec4(aPosition.x * unpackedSpriteSize.x, aPosition.y * unpackedSpriteSize.y, 0.0, 1.0);

	vec2 worldPos = vec2(
		transform.x * position.x + transform.y * position.y,
		transform.z * position.x + transform.w * position.y
	) + translation.xy;

	vFragPos = vec3(uViewMatrix * vec4(worldPos, translation.z, 1.0));
	gl_Position = uProjectionMatrix * vec4(vFragPos, 1.0);
#ifdef __ANDROID__
	vFragPosNorm = normalize(vFragPos);
#endif
	vec4 uvEndpoints = unpackUVEndpoints(uvEndpointsU, uvEndpointsV);
	vTexCoords = vec2(mix(uvEndpoints.x, uvEndpoints.y, aTexCoords.x), mix(uvEndpoints.z, uvEndpoints.w, aTexCoords.y));
	vRotation = rotation;
}
)";

char const * const batched_multitexture_vs = R"(
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

struct Instance
{
	vec4 transform;
	vec4 translation;
	float rotation;
	uint spriteSize;
	uint uvEndpointsU;
	uint uvEndpointsV;
};

// Single instances data will be collected in a uniform block called `InstancesBlock`
layout (std140) uniform InstancesBlock
{
#ifndef BATCH_SIZE
	#define BATCH_SIZE (1365) // 64 KiB / 48 B
#endif
	Instance[BATCH_SIZE] instances;
} block;

out vec3 vFragPos;
#ifdef __ANDROID__
// Workaround for wrong normalization in fragment shader
out vec3 vFragPosNorm;
#endif
out vec2 vTexCoords;
flat out float vRotation;

#define i block.instances[gl_VertexID / 6]

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

	vFragPos = vec3(uViewMatrix * vec4(worldPos, i.translation.z, 1.0));
	gl_Position = uProjectionMatrix * vec4(vFragPos, 1.0);
#ifdef __ANDROID__
	vFragPosNorm = normalize(vFragPos);
#endif
	vec4 uvEndpoints = unpackUVEndpoints(i.uvEndpointsU, i.uvEndpointsV);
	vTexCoords = vec2(mix(uvEndpoints.x, uvEndpoints.y, aTexCoords.x), mix(uvEndpoints.z, uvEndpoints.w, aTexCoords.y));
	vRotation = i.rotation;
}
)";

char const * const multitexture_fs = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
in vec3 vFragPos;
#ifdef __ANDROID__
// Workaround for wrong normalization in fragment shader
in vec3 vFragPosNorm;
#endif
in vec2 vTexCoords;
flat in float vRotation;
layout(location = 0) out vec4 fragColor0;
#ifdef WITH_MRT
layout(location = 1) out vec4 fragColor1;
#endif

// This uniform block does not have a special name and will be shared by all instances
layout (std140) uniform DataBlock
{
	vec4 lightPos;
	vec4 ambientColor; // a component is intensity
	vec4 diffuseColor; // a component is intensity
	vec4 specularColor; // a component is intensity
	vec4 attFactors; // w component is specular scatter
};

mat4 zRotation(in float angle)
{
	return mat4(cos(angle), -sin(angle), 0, 0,
	            sin(angle),  cos(angle), 0, 0,
	            0,           0,          1, 0,
	            0,           0,          0, 1);
}

void main()
{
	vec4 diffuseMap = texture(uTexture0, vTexCoords);
	vec3 normal = normalize(vec4(texture(uTexture1, vTexCoords).xyz * 2.0 - 1.0, 0.0) * zRotation(vRotation)).xyz;
	float specMap = texture(uTexture1, vTexCoords).a;

#ifdef __ANDROID__
	vec3 viewDir = -vFragPosNorm;
#else
	vec3 viewDir = normalize(-vFragPos);
#endif
	vec3 lightDir = lightPos.xyz - vec3(vFragPos.xy, 0.0);
	float distance = length(lightDir);
	lightDir = normalize(lightDir);
	float attenuation = 1.0 / (attFactors.x + attFactors.y * distance + attFactors.z * distance * distance);

	vec3 reflectDir = reflect(-lightDir, normal);
#ifdef __ANDROID__
	// On Android a further normalization might be needed
	reflectDir = normalize(reflectDir);
#endif

	float specularScatter = attFactors.w;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularScatter);

	float specularIntensity = specularColor.a;
	vec3 specular = specularIntensity * spec * specularColor.rgb;

	float nl = dot(lightDir, normal);
	float diff = max(nl, 0.0);

	float diffuseIntensity = diffuseColor.a;
	vec3 diffuse = diffuseIntensity * diff * diffuseColor.rgb;

	float ambientIntensity = ambientColor.a;
	vec3 ambient = ambientIntensity * ambientColor.rgb;

	fragColor0 = vec4(ambient, 0.0) + attenuation * (diffuseMap * vec4(diffuse, 0.0) + specMap * vec4(specular, 0.0));
	fragColor0.a = diffuseMap.a;

#ifdef WITH_MRT
	float brightness = dot(fragColor0.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		fragColor1 = vec4(fragColor0.rgb, 1.0);
	else
		fragColor1 = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}
)";

}
