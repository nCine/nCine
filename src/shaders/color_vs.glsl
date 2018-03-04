#version 330
uniform mat4 projection;

layout (std140) uniform ColorBlock
{
	mat4 modelView;
	vec4 color;
};

in vec2 aPosition;
out vec4 vColor;

void main()
{
	gl_Position = projection * modelView * vec4(aPosition, 0.0, 1.0);
	vColor = color;
}
