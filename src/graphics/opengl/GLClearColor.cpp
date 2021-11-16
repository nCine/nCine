#include "common_macros.h"
#include "GLClearColor.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLClearColor::State GLClearColor::state_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLClearColor::setColor(const Colorf &color)
{
	if (color.r() != state_.color.r() || color.g() != state_.color.g() || color.b() != state_.color.b() || color.a() != state_.color.a())
	{
		glClearColor(color.r(), color.g(), color.b(), color.a());
		state_.color = color;
	}
}

void GLClearColor::setColor(float red, float green, float blue, float alpha)
{
	setColor(Colorf(red, green, blue, alpha));
}

void GLClearColor::setState(State newState)
{
	setColor(newState.color);
	state_ = newState;
}

}
