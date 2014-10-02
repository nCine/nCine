#ifndef CLASS_NCGLFWGFXDEVICE
#define CLASS_NCGLFWGFXDEVICE

#ifdef WITH_GLEW
	#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
	
#include "ncIGfxDevice.h"
#include "ncPoint.h"
#include "ncDisplayMode.h"

/// The GLFW based graphics device
class ncGLFWGfxDevice : public ncIGfxDevice
{
 public:
	// Constructor taking the resolution as two integer
	ncGLFWGfxDevice(int iWidth, int iHeight);
	// Constructor taking the resolution as a size class
	ncGLFWGfxDevice(ncPoint size);
	// Constructor taking the resolution as two integer and a DisplayMode
	ncGLFWGfxDevice(int iWidth, int iHeight, ncDisplayMode mode);
	// Constructor taking the resolution as a Size class and a DisplayMode
	ncGLFWGfxDevice(ncPoint size, ncDisplayMode mode);
	~ncGLFWGfxDevice();

	void SetResolution(int iWidth, int iHeight);
	void SetResolution(ncPoint size);

	void ToggleFullScreen();

	inline void Update() { glfwSwapBuffers(s_pWindowHandle); }

	inline void SetWindowTitle(const char *pWindowTitle) { glfwSetWindowTitle(s_pWindowHandle, pWindowTitle); }

	/// Returns the window handle used by GLFW3
	static GLFWwindow* WindowHandle() { return s_pWindowHandle; }

 private:
	/// GLFW3 window handle
	static GLFWwindow *s_pWindowHandle;

	// Initilizes the class
	void Init(int iWidth, int iHeight, ncDisplayMode mode, bool bIsWindowed);
	// Initilizes the video subsystem (GLFW)
	void InitGraphics();
	// Initilizes the OpenGL graphic context
	void InitDevice();

	// Callback for glfwSetErrorCallback()
	static void ErrorCallback(int error, const char* description);
};

#endif
