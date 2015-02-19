uniform mat4 projection;
uniform mat4 modelview;
attribute vec3 aPosition;
attribute vec3 aColor;
varying vec3 vColor;

void main()
{
	gl_Position = projection * modelview * vec4(aPosition, 1.0);
	vColor = aColor;
}
