#ifndef CLASS_NCINE_QTGFXDEVICE
#define CLASS_NCINE_QTGFXDEVICE

#include <nctl/StaticString.h>
#include "IGfxDevice.h"
#include "Vector2.h"
#include "DisplayMode.h"

class QScreen;

namespace ncine {

class QtWidget;

/// The Qt based graphics device
class QtGfxDevice : public IGfxDevice
{
  public:
	QtGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode, QtWidget &widget);

	void setSwapInterval(int interval) override;

	void setFullscreen(bool fullscreen) override;
	void setResizable(bool resizable) override;

	int windowPositionX() const override;
	int windowPositionY() const override;
	const Vector2i windowPosition() const override;
	void setWindowPosition(int x, int y) override;

	void setWindowSize(int width, int height) override;

	void setWindowTitle(const char *windowTitle) override;
	void setWindowIcon(const char *windowIconFilename) override;
	void flashWindow() const override;

	unsigned int primaryMonitorIndex() const;
	unsigned int windowMonitorIndex() const;

	const VideoMode &currentVideoMode(unsigned int monitorIndex) const override;
	bool setVideoMode(unsigned int modeIndex) override { return false; }

	inline void swapBuffers() override {}

#ifdef WITH_GLEW
	void initGlew();
#endif
	void resetTextureBinding();
	void bindDefaultDrawFramebufferObject();
	/// Resets the OpenGL state cache to bind the default Qt Framebuffer Object
	void resetFramebufferObjectBinding();
	/// Resets the OpenGL state cache for vertex, index, and uniform buffer bindings
	void resetBufferObjectBinding();
	/// Resets the OpenGL state cache for the currently bound shader program
	void resetShaderProgramBinding();
	/// Forces the framebuffer's alpha channel to fully opaque
	void forceOpaqueAlpha();
	/// Clears the screen at the start of a frame when the scenegraph is disabled
	void clearScreen();

  private:
	QtWidget &widget_;

	/// Qt screen pointers
	/*! \note Used to retrieve the index in the monitors array */
	static QScreen *screenPointers_[MaxMonitors];
	static const unsigned int MaxMonitorNameLength = 128;
	static nctl::StaticString<MaxMonitorNameLength> monitorNames_[MaxMonitors];

	/// Deleted copy constructor
	QtGfxDevice(const QtGfxDevice &) = delete;
	/// Deleted assignment operator
	QtGfxDevice &operator=(const QtGfxDevice &) = delete;

	/// Initilizes the OpenGL graphic context
	void initDevice(const WindowMode &windowMode);

	void updateMonitors() override;

	int retrieveMonitorIndex(QScreen *screen) const;

	/// Called by the resize event
	void setSize(int width, int height);

	friend class QtWidget; // for `updateMonitors()`
	friend class QtInputManager;
};

}

#endif
