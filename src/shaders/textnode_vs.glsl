#version 330
uniform mat4 projection;

layout (std140) uniform TextnodeBlock
{
	mat4 modelView;
	vec4 color;
};

in vec2 aPosition;
in vec2 aTexCoords;
out vec2 vTexCoords;
out vec4 vColor;

void main()
{
	gl_Position = projection * modelView * vec4(aPosition, 0.0, 1.0);
	vTexCoords = aTexCoords;
	vColor = color;
}
