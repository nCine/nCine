uniform mat4 projection;
uniform mat4 modelView;
in vec3 aPosition;
in vec3 aColor;
out vec3 vColor;

void main()
{
	gl_Position = projection * modelView * vec4(aPosition, 1.0);
	vColor = aColor;
}
