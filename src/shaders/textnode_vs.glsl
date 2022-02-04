uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

layout (std140) uniform InstanceBlock
{
	mat4 modelMatrix;
	vec4 color;
};

in vec2 aPosition;
in vec2 aTexCoords;
out vec2 vTexCoords;
out vec4 vColor;

void main()
{
	gl_Position = uProjectionMatrix * uViewMatrix * modelMatrix * vec4(aPosition, 0.0, 1.0);
	vTexCoords = aTexCoords;
	vColor = color;
}
