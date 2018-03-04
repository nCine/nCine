#version 330
uniform mat4 projection;

struct SpriteInstance
{
	mat4 modelView;
	vec4 color;
	vec4 texRect;
	vec2 spriteSize;
};

layout (std140) uniform InstancesBlock
{
	SpriteInstance[1] instances;
};

out vec2 vTexCoords;
out vec4 vColor;

void main()
{
	SpriteInstance i = instances[gl_InstanceID];

	vec2 aPosition = vec2(-0.5 + (gl_VertexID >> 1), 0.5 - (gl_VertexID % 2));
	vec2 aTexCoords = vec2(gl_VertexID >> 1, gl_VertexID % 2);
	vec4 position = vec4(aPosition.x * i.spriteSize.x, aPosition.y * i.spriteSize.y, 0.0, 1.0);

	gl_Position = projection * i.modelView * position;
	vTexCoords = vec2(aTexCoords.x * i.texRect.x + i.texRect.y, aTexCoords.y * i.texRect.z + i.texRect.w);
	vColor = i.color;
}
