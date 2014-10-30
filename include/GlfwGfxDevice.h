#ifndef CLASS_NCINE_GLFWGFXDEVICE
#define CLASS_NCINE_GLFWGFXDEVICE

#ifdef WITH_GLEW
	#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include "IGfxDevice.h"
#include "Point.h"
#include "DisplayMode.h"

namespace ncine {

/// The GLFW based graphics device
class GlfwGfxDevice : public IGfxDevice
{
  public:
	// Constructor taking the resolution as two integer
	GlfwGfxDevice(int width, int height);
	// Constructor taking the resolution as a Point class
	explicit GlfwGfxDevice(Point size);
	// Constructor taking the resolution as two integer and a DisplayMode
	GlfwGfxDevice(int width, int height, DisplayMode mode);
	// Constructor taking the resolution as a Point class and a DisplayMode
	GlfwGfxDevice(Point size, DisplayMode mode);
	~GlfwGfxDevice();

	void setResolution(int width, int height);
	void setResolution(Point size);

	void toggleFullScreen();

	inline void update() { glfwSwapBuffers(windowHandle_); }

	inline void setWindowTitle(const char *windowTitle) { glfwSetWindowTitle(windowHandle_, windowTitle); }

	/// Returns the window handle used by GLFW3
	static GLFWwindow* windowHandle() { return windowHandle_; }

  private:
	/// GLFW3 window handle
	static GLFWwindow *windowHandle_;

	// Initilizes the class
	void init(int width, int height, DisplayMode mode, bool isWindowed);
	// Initilizes the video subsystem (GLFW)
	void initGraphics();
	// Initilizes the OpenGL graphic context
	void initDevice();

	// Callback for glfwSetErrorCallback()
	static void errorCallback(int error, const char* description);
};

}

#endif
