#ifndef CLASS_NCSDLGFXDEVICE
#define CLASS_NCSDLGFXDEVICE

#ifndef NO_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>

#include "ncIGfxDevice.h"
#include "ncPoint.h"
#include "ncDisplayMode.h"

/// It represents a graphics device where everything is rendered
/*! At the moment it uses SDL to create an OpenGL context */
class ncSDLGfxDevice : public ncIGfxDevice
{
private:
	// Initialize the class
	void Init(int iWidth, int iHeight, ncDisplayMode mode, bool bIsWindowed);
	// Init the video subsystem (SDL)
	void InitGraphics();
	// Init the OpenGL graphic context
	void InitDevice();
	// Init starting OpenGL state
	void InitGL();

public:
	// Constructor taking the resolution as two integer
	ncSDLGfxDevice(int iWidth, int iHeight);
	// Constructor taking the resolution as a size class
	ncSDLGfxDevice(ncPoint size);
	// Constructor taking the resolution as two integer and a DisplayMode
	ncSDLGfxDevice(int iWidth, int iHeight, ncDisplayMode mode);
	// Constructor taking the resolution as a Size class and a DisplayMode
	ncSDLGfxDevice(ncPoint size, ncDisplayMode mode);
	// Destructor
	virtual ~ncSDLGfxDevice() { SDL_Quit(); }

	// Set screen resolution with two integers
	virtual void SetResolution(int iWidth, int iHeight);
	// Set screen resolution with the Size class
	virtual void SetResolution(ncPoint size);

	// Toggle between fullscreen and windowed mode
	virtual void ToggleFullScreen();

	/// Update the screen swapping back and front buffers
	inline virtual void Update() { SDL_GL_SwapBuffers(); }
	/// Clear the screen
	inline virtual void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
};

#endif
