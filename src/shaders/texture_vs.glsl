uniform mat4 projection;
uniform mat4 modelView;
attribute vec3 aPosition;
attribute vec2 aTexCoords;
varying vec2 vTexCoords;

void main()
{
	gl_Position = projection * modelView * vec4(aPosition, 1.0);
	vTexCoords = aTexCoords;
}
