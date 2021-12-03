uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
in vec3 aPosition;
in vec3 aColor;
out vec3 vColor;

void main()
{
	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);
	vColor = aColor;
}
