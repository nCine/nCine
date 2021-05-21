#include "common_macros.h"
#include "GLBlending.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool GLBlending::enabled_ = false;
GLenum GLBlending::srcRgb_ = GL_ONE;
GLenum GLBlending::dstRgb_ = GL_ZERO;
GLenum GLBlending::srcAlpha_ = GL_ONE;
GLenum GLBlending::dstAlpha_ = GL_ZERO;

bool GLBlending::stateSaved_ = false;
bool GLBlending::wasEnabled_ = false;
GLenum GLBlending::oldSrcRgb_ = GL_ONE;
GLenum GLBlending::oldDstRgb_ = GL_ZERO;
GLenum GLBlending::oldSrcAlpha_ = GL_ONE;
GLenum GLBlending::oldDstAlpha_ = GL_ZERO;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLBlending::enable()
{
	if (enabled_ == false)
	{
		glEnable(GL_BLEND);
		enabled_ = true;
	}
}

void GLBlending::disable()
{
	if (enabled_ == true)
	{
		glDisable(GL_BLEND);
		enabled_ = false;
	}
}

void GLBlending::blendFunc(GLenum sfactor, GLenum dfactor)
{
	if (sfactor != srcRgb_ || dfactor != dstRgb_ ||
	    sfactor != srcAlpha_ || dfactor != dstAlpha_)
	{
		glBlendFunc(sfactor, dfactor);
		srcRgb_ = sfactor;
		dstRgb_ = dfactor;
		srcAlpha_ = sfactor;
		dstAlpha_ = dfactor;
	}
}

void GLBlending::blendFunc(GLenum srcRgb, GLenum dstRgb, GLenum srcAlpha, GLenum dstAlpha)
{
	if (srcRgb != srcRgb_ || dstRgb != dstRgb_ ||
	    srcAlpha != srcAlpha_ || dstAlpha != dstAlpha_)
	{
		glBlendFuncSeparate(srcRgb, dstRgb, srcAlpha, dstAlpha);
		srcRgb_ = srcRgb;
		dstRgb_ = dstRgb;
		srcAlpha_ = srcAlpha;
		dstAlpha_ = dstAlpha;
	}
}

void GLBlending::pushState()
{
	ASSERT(stateSaved_ == false);

	wasEnabled_ = enabled_;
	oldSrcRgb_ = srcRgb_;
	oldDstRgb_ = dstRgb_;
	oldSrcAlpha_ = srcAlpha_;
	oldDstAlpha_ = dstAlpha_;

	stateSaved_ = true;
}

void GLBlending::popState()
{
	ASSERT(stateSaved_ == true);

	if (wasEnabled_)
		enable();
	else
		disable();

	if (oldSrcRgb_ == oldSrcAlpha_ && oldDstRgb_ == oldDstAlpha_)
		blendFunc(oldSrcRgb_, oldDstRgb_);
	else
		blendFunc(oldSrcRgb_, oldDstRgb_, oldSrcAlpha_, oldDstAlpha_);

	stateSaved_ = false;
}

}
