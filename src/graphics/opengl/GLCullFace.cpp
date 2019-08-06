#include "common_macros.h"
#include "GLCullFace.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool GLCullFace::enabled_ = false;
GLenum GLCullFace::mode_ = GL_BACK;

bool GLCullFace::stateSaved_ = false;
bool GLCullFace::wasEnabled_ = false;
GLenum GLCullFace::oldMode_ = GL_BACK;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLCullFace::enable()
{
	if (enabled_ == false)
	{
		glEnable(GL_CULL_FACE);
		enabled_ = true;
	}
}

void GLCullFace::disable()
{
	if (enabled_ == true)
	{
		glDisable(GL_CULL_FACE);
		enabled_ = false;
	}
}

void GLCullFace::set(GLenum mode)
{
	if (mode != mode_)
	{
		glCullFace(mode);
		mode_ = mode;
	}
}

void GLCullFace::pushState()
{
	ASSERT(stateSaved_ == false);

	wasEnabled_ = enabled_;
	oldMode_ = mode_;

	stateSaved_ = true;
}

void GLCullFace::popState()
{
	ASSERT(stateSaved_ == true);

	if (wasEnabled_)
		enable();
	else
		disable();
	set(oldMode_);

	stateSaved_ = false;
}

}
