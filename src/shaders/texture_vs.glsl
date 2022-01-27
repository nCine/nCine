uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
in vec3 aPosition;
in vec2 aTexCoords;
out vec2 vTexCoords;

void main()
{
	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);
	vTexCoords = aTexCoords;
}
