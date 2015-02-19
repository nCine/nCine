uniform mat4 projection;
uniform mat4 modelview;
attribute vec3 aPosition;
attribute vec2 aTexCoords;
varying vec2 vTexCoords;

void main()
{
	gl_Position = projection * modelview * vec4(aPosition, 1.0);
	vTexCoords = aTexCoords;
}
