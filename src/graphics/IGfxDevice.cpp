#define NCINE_INCLUDE_OPENGL
#include "common.h"

#include "IGfxDevice.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Clears the screen
void IGfxDevice::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Sets up the initial OpenGL state
void IGfxDevice::setupGL()
{
	glDisable(GL_DITHER);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef WITH_DEPTH_TEST
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glAlphaFunc(GL_GREATER, 0.0f);
#endif

	glMatrixMode(GL_PROJECTION);
#ifndef __ANDROID__
	glOrtho(0, width_, 0, height_, -1, 1);
#else
	glOrthof(0, width_, 0, height_, -1, 1);
#endif
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

}
