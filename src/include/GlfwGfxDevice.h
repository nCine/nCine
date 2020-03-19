#ifndef CLASS_NCINE_GLFWGFXDEVICE
#define CLASS_NCINE_GLFWGFXDEVICE

#ifdef WITH_GLEW
	#define GLEW_NO_GLU
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
	GlfwGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode);
	~GlfwGfxDevice() override;

	void setSwapInterval(int interval) override;

	void setResolution(int width, int height) override;
	inline void setResolution(Vector2i size) override { setResolution(size.x, size.y); }

	void setFullScreen(bool fullScreen) override;

	inline void update() override { glfwSwapBuffers(windowHandle_); }

	inline void setWindowTitle(const char *windowTitle) override { glfwSetWindowTitle(windowHandle_, windowTitle); }
	void setWindowIcon(const char *windowIconFilename) override;

	const VideoMode &currentVideoMode() const override;
	bool setVideoMode(unsigned int index) override;
	void updateVideoModes() override;

  private:
	/// GLFW3 window handle
	static GLFWwindow *windowHandle_;

	/// Deleted copy constructor
	GlfwGfxDevice(const GlfwGfxDevice &) = delete;
	/// Deleted assignment operator
	GlfwGfxDevice &operator=(const GlfwGfxDevice &) = delete;

	/// Initilizes the video subsystem (GLFW)
	void initGraphics();
	/// Initilizes the OpenGL graphic context
	void initDevice();

	void convertVideoModeInfo(const GLFWvidmode &glfwVideoMode, IGfxDevice::VideoMode &videoMode) const;

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
