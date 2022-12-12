#ifndef CLASS_NCINE_QT5GFXDEVICE
#define CLASS_NCINE_QT5GFXDEVICE

#include <nctl/StaticString.h>
#include "IGfxDevice.h"
#include "Vector2.h"
#include "DisplayMode.h"

class QScreen;

namespace ncine {

class Qt5Widget;

/// The Qt5 based graphics device
class Qt5GfxDevice : public IGfxDevice
{
  public:
	Qt5GfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode, Qt5Widget &widget);

	void setSwapInterval(int interval) override;

	void setFullScreen(bool fullScreen) override;

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

	inline void update() override {}

#ifdef WITH_GLEW
	void initGlew();
#endif
	void resetTextureBinding();
	void bindDefaultDrawFramebufferObject();
	/// Resets the OpenGL state cache to bind the default Qt5 Framebuffer Object
	void resetFramebufferObjectBinding();

  private:
	Qt5Widget &widget_;

	/// Qt5 screen pointers
	/*! \note Used to retrieve the index in the monitors array */
	static QScreen *screenPointers_[MaxMonitors];
	static const unsigned int MaxMonitorNameLength = 128;
	static nctl::StaticString<MaxMonitorNameLength> monitorNames_[MaxMonitors];

	/// Deleted copy constructor
	Qt5GfxDevice(const Qt5GfxDevice &) = delete;
	/// Deleted assignment operator
	Qt5GfxDevice &operator=(const Qt5GfxDevice &) = delete;

	/// Initilizes the OpenGL graphic context
	void initDevice(const WindowMode &windowMode);

	void updateMonitors() override;

	int retrieveMonitorIndex(QScreen *screen) const;

	/// Called by the resize event
	void setSize(int width, int height);

	friend class Qt5Widget; // for `updateMonitors()`
	friend class Qt5InputManager;
};

}

#endif
