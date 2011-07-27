#include <cstdlib> // for exit()
#include <android_native_app_glue.h> // for android_app
#include "ncEGLGfxDevice.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking a DisplayMode
ncEGLGfxDevice::ncEGLGfxDevice(struct android_app* state, ncDisplayMode mode)
{
	m_mode = mode;
	InitDevice(state);
	InitGL();
}

ncEGLGfxDevice::~ncEGLGfxDevice()
{
	if (m_display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (m_context != EGL_NO_CONTEXT)
			eglDestroyContext(m_display, m_context);
		if (m_surface != EGL_NO_SURFACE)
			eglDestroySurface(m_display, m_surface);
		eglTerminate(m_display);
	}

	m_display = EGL_NO_DISPLAY;
	m_context = EGL_NO_CONTEXT;
	m_surface = EGL_NO_SURFACE;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initialize the class
void ncEGLGfxDevice::InitDevice(struct android_app* state)
{
	const EGLint attribs[] = {
		  EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		  EGL_BLUE_SIZE, m_mode.BlueBits(),
		  EGL_GREEN_SIZE, m_mode.GreenBits(),
		  EGL_RED_SIZE, m_mode.RedBits(),
		  EGL_NONE
	};

	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;

	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(m_display, 0, 0);

	eglChooseConfig(m_display, attribs, &config, 1, &numConfigs);
	eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format);

#ifdef __ANDROID__
	ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);
#endif

	m_surface = eglCreateWindowSurface(m_display, config, state->window, NULL);
	m_context = eglCreateContext(m_display, config, NULL, NULL);

	if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncEGLGfxDevice::Init - Unable to eglMakeCurrent");
		exit(-1);
	}

	eglQuerySurface(m_display, m_surface, EGL_WIDTH, &m_iWidth);
	eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_iHeight);
}

/// Init starting OpenGL
void ncEGLGfxDevice::InitGL()
{
//	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glOrthof(0, m_iWidth, 0, m_iHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
