uniform mat4 projection;
uniform mat4 modelView;
uniform vec4 texRect;
uniform vec2 spriteSize;
attribute vec2 aPosition;
attribute vec2 aTexCoords;
varying vec2 vTexCoords;

void main()
{
	vec4 position = vec4(aPosition.x * spriteSize.x, aPosition.y * spriteSize.y, 0.0, 1.0);

	gl_Position = projection * modelView * position;
	vTexCoords = vec2(aTexCoords.x * texRect.x + texRect.y, aTexCoords.y * texRect.z + texRect.w);
}
