uniform mat4 projection;
uniform mat4 modelView;
attribute vec2 aPosition;

void main()
{
	gl_Position = projection * modelView * vec4(aPosition, 0.0, 1.0);
}
