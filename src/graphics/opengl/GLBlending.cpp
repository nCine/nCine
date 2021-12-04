#include "common_macros.h"
#include "GLBlending.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLBlending::State GLBlending::state_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLBlending::enable()
{
	if (state_.enabled == false)
	{
		glEnable(GL_BLEND);
		state_.enabled = true;
	}
}

void GLBlending::disable()
{
	if (state_.enabled == true)
	{
		glDisable(GL_BLEND);
		state_.enabled = false;
	}
}

void GLBlending::setBlendFunc(GLenum sfactor, GLenum dfactor)
{
	if (sfactor != state_.srcRgb || dfactor != state_.dstRgb ||
	    sfactor != state_.srcAlpha || dfactor != state_.dstAlpha)
	{
		glBlendFunc(sfactor, dfactor);
		state_.srcRgb = sfactor;
		state_.dstRgb = dfactor;
		state_.srcAlpha = sfactor;
		state_.dstAlpha = dfactor;
	}
}

void GLBlending::setBlendFunc(GLenum srcRgb, GLenum dstRgb, GLenum srcAlpha, GLenum dstAlpha)
{
	if (srcRgb != state_.srcRgb || dstRgb != state_.dstRgb ||
	    srcAlpha != state_.srcAlpha || dstAlpha != state_.dstAlpha)
	{
		glBlendFuncSeparate(srcRgb, dstRgb, srcAlpha, dstAlpha);
		state_.srcRgb = srcRgb;
		state_.dstRgb = dstRgb;
		state_.srcAlpha = srcAlpha;
		state_.dstAlpha = dstAlpha;
	}
}

void GLBlending::setState(State newState)
{
	if (newState.enabled)
		enable();
	else
		disable();

	if (newState.srcRgb == newState.srcAlpha && newState.dstRgb == newState.dstAlpha)
		setBlendFunc(newState.srcRgb, newState.dstRgb);
	else
		setBlendFunc(newState.srcRgb, newState.dstRgb, newState.srcAlpha, newState.dstAlpha);

	state_ = newState;
}

}
