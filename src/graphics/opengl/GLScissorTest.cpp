#include "common_macros.h"
#include "GLScissorTest.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLScissorTest::State GLScissorTest::state_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLScissorTest::enable(const Recti &rect)
{
	if (state_.enabled == false)
	{
		glEnable(GL_SCISSOR_TEST);
		state_.enabled = true;
	}

	if (rect.x != state_.rect.x || rect.y != state_.rect.y ||
	    rect.w != state_.rect.w || rect.h != state_.rect.h)
	{
		FATAL_ASSERT(rect.w >= 0 && rect.h >= 0);
		glScissor(rect.x, rect.y, rect.w, rect.h);
		state_.rect = rect;
	}
}

void GLScissorTest::enable(GLint x, GLint y, GLsizei width, GLsizei height)
{
	enable(Recti(x, y, width, height));
}

void GLScissorTest::enable()
{
	if (state_.enabled == false)
	{
		FATAL_ASSERT(state_.rect.w >= 0 && state_.rect.h >= 0);
		glEnable(GL_SCISSOR_TEST);
		state_.enabled = true;
	}
}

void GLScissorTest::disable()
{
	if (state_.enabled == true)
	{
		glDisable(GL_SCISSOR_TEST);
		state_.enabled = false;
	}
}

void GLScissorTest::setState(State newState)
{
	if (newState.enabled)
		enable(newState.rect);
	else
		disable();

	state_ = newState;
}

}
