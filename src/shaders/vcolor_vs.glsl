uniform mat4 projection;
uniform mat4 modelView;
attribute vec3 aPosition;
attribute vec3 aColor;
varying vec3 vColor;

void main()
{
	gl_Position = projection * modelView * vec4(aPosition, 1.0);
	vColor = aColor;
}
