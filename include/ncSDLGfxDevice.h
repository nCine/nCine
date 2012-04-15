#ifndef CLASS_NCSDLGFXDEVICE
#define CLASS_NCSDLGFXDEVICE

#ifdef WITH_GLEW
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
	// Initilizes the class
	void Init(int iWidth, int iHeight, ncDisplayMode mode, bool bIsWindowed);
	// Initilizes the video subsystem (SDL)
	void InitGraphics();
	// Initilizes the OpenGL graphic context
	void InitDevice();
	// Initilizes starting OpenGL state
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

	// Sets screen resolution with two integers
	virtual void SetResolution(int iWidth, int iHeight);
	// Sets screen resolution with the Size class
	virtual void SetResolution(ncPoint size);

	// Toggles between fullscreen and windowed mode
	virtual void ToggleFullScreen();

	/// Updates the screen swapping back and front buffers
	inline virtual void Update() { SDL_GL_SwapBuffers(); }
	/// Clears the screen
	inline virtual void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
};

#endif
