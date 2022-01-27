#ifndef CLASS_NCINE_QT5GFXDEVICE
#define CLASS_NCINE_QT5GFXDEVICE

#include "IGfxDevice.h"
#include "Vector2.h"
#include "DisplayMode.h"

namespace ncine {

class Qt5Widget;

/// The Qt5 based graphics device
class Qt5GfxDevice : public IGfxDevice
{
  public:
	Qt5GfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode, Qt5Widget &widget);

	void setSwapInterval(int interval) override;

	void setResolution(int width, int height) override;

	void setFullScreen(bool fullScreen) override;

	inline void update() override {}

	void setWindowPosition(int x, int y) override;
	void setWindowTitle(const char *windowTitle) override;
	void setWindowIcon(const char *windowIconFilename) override;

	int windowPositionX() const override;
	int windowPositionY() const override;
	const Vector2i windowPosition() const override;

	void flashWindow() const override;

	const VideoMode &currentVideoMode() const override;
	bool setVideoMode(unsigned int index) override { return false; }
	void updateVideoModes() override;

#ifdef WITH_GLEW
	void initGlew();
#endif
	void resetTextureBinding();

  private:
	Qt5Widget &widget_;

	/// Deleted copy constructor
	Qt5GfxDevice(const Qt5GfxDevice &) = delete;
	/// Deleted assignment operator
	Qt5GfxDevice &operator=(const Qt5GfxDevice &) = delete;

	/// Initilizes the OpenGL graphic context
	void initDevice();

	friend class Qt5InputManager;
};

}

#endif
