#include "common_macros.h"
#include "GLDepthTest.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLDepthTest::State GLDepthTest::state_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLDepthTest::enable()
{
	if (state_.enabled == false)
	{
		glEnable(GL_DEPTH_TEST);
		state_.enabled = true;
	}
}

void GLDepthTest::disable()
{
	if (state_.enabled == true)
	{
		glDisable(GL_DEPTH_TEST);
		state_.enabled = false;
	}
}

void GLDepthTest::enableDepthMask()
{
	if (state_.depthMaskEnabled == false)
	{
		glDepthMask(GL_TRUE);
		state_.depthMaskEnabled = true;
	}
}

void GLDepthTest::disableDepthMask()
{
	if (state_.depthMaskEnabled == true)
	{
		glDepthMask(GL_FALSE);
		state_.depthMaskEnabled = false;
	}
}

void GLDepthTest::setState(State newState)
{
	if (newState.enabled)
		enable();
	else
		disable();

	if (newState.depthMaskEnabled)
		enableDepthMask();
	else
		disableDepthMask();

	state_ = newState;
}

}
