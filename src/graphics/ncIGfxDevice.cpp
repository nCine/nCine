#include "ncIGfxDevice.h"
#include "ncServiceLocator.h"

#ifdef __ANDROID__
	#include <GLES/gl.h>
#else
	#ifdef WITH_GLEW
		#include <GL/glew.h>
	#else
		#include <GL/gl.h>
		#include <GL/glext.h>
	#endif
#endif

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Clears the screen
void ncIGfxDevice::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

/// Initilizes starting OpenGL state
void ncIGfxDevice::InitGL()
{
	ncGfxCapabilities& gfxCaps = const_cast<ncGfxCapabilities&>(ncServiceLocator::GfxCapabilities());
	gfxCaps.Init();

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
	glOrtho(0, m_iWidth, 0, m_iHeight, -1, 1);
#else
	glOrthof(0, m_iWidth, 0, m_iHeight, -1, 1);
#endif
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
