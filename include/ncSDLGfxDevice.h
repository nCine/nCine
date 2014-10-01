#ifndef CLASS_NCSDLGFXDEVICE
#define CLASS_NCSDLGFXDEVICE

#include <SDL/SDL.h>

#include "ncIGfxDevice.h"
#include "ncPoint.h"
#include "ncDisplayMode.h"

/// The SDL based graphics device
class ncSDLGfxDevice : public ncIGfxDevice
{
 public:
	// Constructor taking the resolution as two integer
	ncSDLGfxDevice(int iWidth, int iHeight);
	// Constructor taking the resolution as a size class
	ncSDLGfxDevice(ncPoint size);
	// Constructor taking the resolution as two integer and a DisplayMode
	ncSDLGfxDevice(int iWidth, int iHeight, ncDisplayMode mode);
	// Constructor taking the resolution as a Size class and a DisplayMode
	ncSDLGfxDevice(ncPoint size, ncDisplayMode mode);
	~ncSDLGfxDevice() { SDL_Quit(); }

	void SetResolution(int iWidth, int iHeight);
	void SetResolution(ncPoint size);

	void ToggleFullScreen();

	inline void Update() { SDL_GL_SwapBuffers(); }

	inline void SetWindowTitle(const char *pWindowTitle) { SDL_WM_SetCaption(pWindowTitle, NULL); }

 private:
	// Initilizes the class
	void Init(int iWidth, int iHeight, ncDisplayMode mode, bool bIsWindowed);
	// Initilizes the video subsystem (SDL)
	void InitGraphics();
	// Initilizes the OpenGL graphic context
	void InitDevice();
};

#endif
