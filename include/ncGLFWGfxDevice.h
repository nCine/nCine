#ifndef CLASS_NCGLFWGFXDEVICE
#define CLASS_NCGLFWGFXDEVICE

#ifdef WITH_GLEW
	#include <GL/glew.h>
#endif
#if defined(__APPLE__)
	#include <GLFW/glfw.h>
#else
	#include <GL/glfw.h>
#endif
	
#include "ncIGfxDevice.h"
#include "ncPoint.h"
#include "ncDisplayMode.h"

/// The GLFW based graphics device
class ncGLFWGfxDevice : public ncIGfxDevice
{
private:
	// Initilizes the class
	void Init(int iWidth, int iHeight, ncDisplayMode mode, bool bIsWindowed);
	// Initilizes the video subsystem (GLFW)
	void InitGraphics();
	// Initilizes the OpenGL graphic context
	void InitDevice();

public:
	// Constructor taking the resolution as two integer
	ncGLFWGfxDevice(int iWidth, int iHeight);
	// Constructor taking the resolution as a size class
	ncGLFWGfxDevice(ncPoint size);
	// Constructor taking the resolution as two integer and a DisplayMode
	ncGLFWGfxDevice(int iWidth, int iHeight, ncDisplayMode mode);
	// Constructor taking the resolution as a Size class and a DisplayMode
	ncGLFWGfxDevice(ncPoint size, ncDisplayMode mode);
	~ncGLFWGfxDevice() { glfwTerminate(); }

	void SetResolution(int iWidth, int iHeight);
	void SetResolution(ncPoint size);

	void ToggleFullScreen();

	inline void Update() { glfwSwapBuffers(); }

	inline void SetWindowTitle(const char *pWindowTitle) { glfwSetWindowTitle(pWindowTitle); }
};

#endif
