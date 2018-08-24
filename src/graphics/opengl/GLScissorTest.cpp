#include "common_macros.h"
#include "GLScissorTest.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool GLScissorTest::enabled_ = false;
GLint GLScissorTest::x_ = 0;
GLint GLScissorTest::y_ = 0;
GLsizei GLScissorTest::width_ = 0;
GLsizei GLScissorTest::height_ = 0;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLScissorTest::enable(GLint x, GLint y, GLsizei width, GLsizei height)
{
	if (enabled_ == false)
	{
		glEnable(GL_SCISSOR_TEST);
		enabled_ = true;
	}

	if (x != x_ || y != y_ || width != width_ || height != height_)
	{
		FATAL_ASSERT(width > 0 && height > 0);
		glScissor(x, y, width, height);
		x_ = x;
		y_ = y;
		width_ = width;
		height_ = height;
	}
}

void GLScissorTest::enable()
{
	if (enabled_ == false)
	{
		FATAL_ASSERT(width_ > 0 && height_ > 0);
		glEnable(GL_SCISSOR_TEST);
		enabled_ = true;
	}
}

void GLScissorTest::disable()
{
	if (enabled_ == true)
	{
		glDisable(GL_SCISSOR_TEST);
		enabled_ = false;
	}
}

}
