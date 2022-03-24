namespace {

char const * const sprite_vs = R"(
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

// Instance data should go in a uniform block called `InstanceBlock` for batching to work
layout (std140) uniform InstanceBlock
{
	mat4 modelMatrix;
	vec4 texRect;
	vec2 spriteSize;
	float angle;
};

out vec2 vTexCoords;
flat out vec2 vSpriteSize;

void main()
{
	vec2 aPosition = vec2(0.5 - float(gl_VertexID >> 1), -0.5 + float(gl_VertexID % 2));
	vec2 aTexCoords = vec2(1.0 - float(gl_VertexID >> 1), 1.0 - float(gl_VertexID % 2));
	vec4 position = vec4(aPosition.x * spriteSize.x, aPosition.y * spriteSize.y, 0.0, 1.0);

	gl_Position = uProjectionMatrix * uViewMatrix * modelMatrix * position;
	vec2 twistedCoords = vec2((cos(angle) * cos(angle) * 0.85 + 0.15) * aTexCoords.x, (sin(angle) * sin(angle) * 0.85 + 0.15) * aTexCoords.y);
	vTexCoords = vec2(twistedCoords.x * texRect.x + texRect.y, twistedCoords.y * texRect.z + texRect.w);
	vSpriteSize = spriteSize;
}
)";

char const * const batched_sprite_vs = R"(
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

struct Instance
{
	mat4 modelMatrix;
	vec4 texRect;
	vec2 spriteSize;
	float angle;
};

// Single instances data will be collected in a uniform block called `InstancesBlock`
layout (std140) uniform InstancesBlock
{
#ifdef WITH_FIXED_BATCH_SIZE
	Instance[BATCH_SIZE] instances;
#else
	Instance[585] instances;
#endif
} block;

out vec2 vTexCoords;
flat out vec2 vSpriteSize;

#define i block.instances[gl_VertexID / 6]

void main()
{
	vec2 aPosition = vec2(-0.5 + float(((gl_VertexID + 2) / 3) % 2), 0.5 - float(((gl_VertexID + 1) / 3) % 2));
	vec2 aTexCoords = vec2(float(((gl_VertexID + 2) / 3) % 2), float(((gl_VertexID + 1) / 3) % 2));
	vec4 position = vec4(aPosition.x * i.spriteSize.x, aPosition.y * i.spriteSize.y, 0.0, 1.0);

	gl_Position = uProjectionMatrix * uViewMatrix * i.modelMatrix * position;
	vec2 twistedCoords = vec2((cos(i.angle) * cos(i.angle) * 0.85 + 0.15) * aTexCoords.x, (sin(i.angle) * sin(i.angle) * 0.85 + 0.15) * aTexCoords.y);
	vTexCoords = vec2(twistedCoords.x * i.texRect.x + i.texRect.y, twistedCoords.y * i.texRect.z + i.texRect.w);
	vSpriteSize = i.spriteSize;
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

}
