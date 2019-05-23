#include "common_macros.h"
#include "GLDepthTest.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool GLDepthTest::enabled_ = false;
bool GLDepthTest::depthMaskEnabled_ = true;

bool GLDepthTest::stateSaved_ = false;
bool GLDepthTest::wasEnabled_ = false;
bool GLDepthTest::depthMaskWasEnabled_ = true;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLDepthTest::enable()
{
	if (enabled_ == false)
	{
		glEnable(GL_DEPTH_TEST);
		enabled_ = true;
	}
}

void GLDepthTest::disable()
{
	if (enabled_ == true)
	{
		glDisable(GL_DEPTH_TEST);
		enabled_ = false;
	}
}

void GLDepthTest::enableDepthMask()
{
	if (depthMaskEnabled_ == false)
	{
		glDepthMask(GL_TRUE);
		depthMaskEnabled_ = true;
	}
}

void GLDepthTest::disableDepthMask()
{
	if (depthMaskEnabled_ == true)
	{
		glDepthMask(GL_FALSE);
		depthMaskEnabled_ = false;
	}
}

void GLDepthTest::pushState()
{
	ASSERT(stateSaved_ == false);

	wasEnabled_ = enabled_;
	depthMaskWasEnabled_ = depthMaskEnabled_;

	stateSaved_ = true;
}

void GLDepthTest::popState()
{
	ASSERT(stateSaved_ == true);

	if (wasEnabled_)
		enable();
	else
		disable();

	if (depthMaskWasEnabled_)
		enableDepthMask();
	else
		disableDepthMask();

	stateSaved_ = false;
}

}
