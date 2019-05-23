#include "common_macros.h"
#include "GLBlending.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool GLBlending::enabled_ = false;
GLenum GLBlending::sfactor_ = GL_ONE;
GLenum GLBlending::dfactor_ = GL_ZERO;

bool GLBlending::stateSaved_ = false;
bool GLBlending::wasEnabled_ = false;
GLenum GLBlending::oldSfactor_ = GL_ONE;
GLenum GLBlending::oldDfactor_ = GL_ZERO;

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
	if (sfactor != sfactor_ || dfactor != dfactor_)
	{
		glBlendFunc(sfactor, dfactor);
		sfactor_ = sfactor;
		dfactor_ = dfactor;
	}
}

void GLBlending::pushState()
{
	ASSERT(stateSaved_ == false);

	wasEnabled_ = enabled_;
	oldSfactor_ = sfactor_;
	oldDfactor_ = dfactor_;

	stateSaved_ = true;
}

void GLBlending::popState()
{
	ASSERT(stateSaved_ == true);

	if (wasEnabled_)
		enable();
	else
		disable();
	blendFunc(oldSfactor_, oldDfactor_);

	stateSaved_ = false;
}

}
