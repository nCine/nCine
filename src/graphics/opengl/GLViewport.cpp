#include "common_macros.h"
#include "GLViewport.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLViewport::State GLViewport::state_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLViewport::setRect(const Recti &rect)
{
	if (rect.x != state_.rect.x || rect.y != state_.rect.y ||
	    rect.w != state_.rect.w || rect.h != state_.rect.h)
	{
		FATAL_ASSERT(rect.w >= 0 && rect.h >= 0);
		glViewport(rect.x, rect.y, rect.w, rect.h);
		state_.rect = rect;
	}
}

void GLViewport::setRect(GLint x, GLint y, GLsizei width, GLsizei height)
{
	setRect(Recti(x, y, width, height));
}

void GLViewport::setState(State newState)
{
	setRect(newState.rect);
	state_ = newState;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLViewport::initRect(GLint x, GLint y, GLsizei width, GLsizei height)
{
	state_.rect.x = x;
	state_.rect.y = y;
	state_.rect.w = width;
	state_.rect.h = height;
}

}
