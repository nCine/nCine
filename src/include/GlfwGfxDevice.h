#ifndef CLASS_NCINE_GLFWGFXDEVICE
#define CLASS_NCINE_GLFWGFXDEVICE

#ifdef WITH_GLEW
	#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include "IGfxDevice.h"
#include "Vector2.h"
#include "DisplayMode.h"

namespace ncine {

/// The GLFW based graphics device
class GlfwGfxDevice : public IGfxDevice
{
  public:
	/// Constructor taking the resolution as two integer
	GlfwGfxDevice(int width, int height, bool isFullScreen_);
	/// Constructor taking the resolution as a `Vector2<int>` object
	explicit GlfwGfxDevice(Vector2i size, bool isFullScreen_);
	/// Constructor taking the resolution as two integer and a `DisplayMode`
	GlfwGfxDevice(int width, int height, DisplayMode mode, bool isFullScreen_);
	/// Constructor taking the resolution as a `Vector2<int>` object and a `DisplayMode`
	GlfwGfxDevice(Vector2i size, DisplayMode mode, bool isFullScreen_);
	~GlfwGfxDevice();

	void setResolution(int width, int height);
	void setResolution(Vector2i size);

	void toggleFullScreen();

	inline void update() { glfwSwapBuffers(windowHandle_); }

	inline void setWindowTitle(const char *windowTitle) { glfwSetWindowTitle(windowHandle_, windowTitle); }

  private:
	/// GLFW3 window handle
	static GLFWwindow *windowHandle_;

	/// Private copy constructor
	GlfwGfxDevice(const GlfwGfxDevice &);
	/// Private assignment operator
	GlfwGfxDevice &operator=(const GlfwGfxDevice &);

	/// Initilizes the object
	void init(int width, int height, DisplayMode mode, bool isFullScreen_);
	/// Initilizes the video subsystem (GLFW)
	void initGraphics();
	/// Initilizes the OpenGL graphic context
	void initDevice();

	/// Returns the window handle used by GLFW3
	static GLFWwindow *windowHandle() { return windowHandle_; }

	/// Callback for `glfwSetErrorCallback()`
	static void errorCallback(int error, const char *description);

	friend class GlfwInputManager;
	friend class GlfwMouseState;
	friend class GlfwKeyboardState;
};

}

#endif
