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
	ncGLFWGfxDevice(int width, int height);
	// Constructor taking the resolution as a size class
	explicit ncGLFWGfxDevice(ncPoint size);
	// Constructor taking the resolution as two integer and a DisplayMode
	ncGLFWGfxDevice(int width, int height, ncDisplayMode mode);
	// Constructor taking the resolution as a Size class and a DisplayMode
	ncGLFWGfxDevice(ncPoint size, ncDisplayMode mode);
	~ncGLFWGfxDevice();

	void setResolution(int width, int height);
	void setResolution(ncPoint size);

	void toggleFullScreen();

	inline void update() { glfwSwapBuffers(windowHandle_); }

	inline void setWindowTitle(const char *windowTitle) { glfwSetWindowTitle(windowHandle_, windowTitle); }

	/// Returns the window handle used by GLFW3
	static GLFWwindow* windowHandle() { return windowHandle_; }

  private:
	/// GLFW3 window handle
	static GLFWwindow *windowHandle_;

	// Initilizes the class
	void init(int width, int height, ncDisplayMode mode, bool isWindowed);
	// Initilizes the video subsystem (GLFW)
	void initGraphics();
	// Initilizes the OpenGL graphic context
	void initDevice();

	// Callback for glfwSetErrorCallback()
	static void errorCallback(int error, const char* description);
};

#endif
