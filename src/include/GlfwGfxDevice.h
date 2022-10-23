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

	void setFullScreen(bool fullScreen) override;

	int windowPositionX() const override;
	int windowPositionY() const override;
	const Vector2i windowPosition() const override;
	void setWindowPosition(int x, int y) override;

	void setWindowSize(int width, int height) override;

	inline void setWindowTitle(const char *windowTitle) override { glfwSetWindowTitle(windowHandle_, windowTitle); }
	void setWindowIcon(const char *windowIconFilename) override;
	void flashWindow() const override;

	unsigned int primaryMonitorIndex() const override;
	unsigned int windowMonitorIndex() const override;

	const VideoMode &currentVideoMode(unsigned int monitorIndex) const override;
	bool setVideoMode(unsigned int modeIndex) override;

	inline void update() override { glfwSwapBuffers(windowHandle_); }

  private:
	/// GLFW3 window handle
	static GLFWwindow *windowHandle_;

	/// GLFW3 monitor pointers
	/*! \note Used to retrieve the index in the monitors array */
	static GLFWmonitor *monitorPointers_[MaxMonitors];

	/// Monitor index to use in full screen
	static int fsMonitorIndex_;
	/// Video mode index to use in full screen
	static int fsModeIndex_;
#ifdef _WIN32
	/// Swap interval to restore after going full screen
	static int swapInterval_;
#endif

	/// Deleted copy constructor
	GlfwGfxDevice(const GlfwGfxDevice &) = delete;
	/// Deleted assignment operator
	GlfwGfxDevice &operator=(const GlfwGfxDevice &) = delete;

	/// Initilizes the video subsystem (GLFW)
	void initGraphics();
	/// Initilizes the OpenGL graphic context
	void initDevice();

	void updateMonitors() override;
	void updateMonitorScaling(unsigned int monitorIndex);

	int retrieveMonitorIndex(GLFWmonitor *monitor) const;
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
