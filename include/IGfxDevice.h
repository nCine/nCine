#ifndef CLASS_NCINE_IGFXDEVICE
#define CLASS_NCINE_IGFXDEVICE

#include "Vector2.h"
#include "DisplayMode.h"

namespace ncine {

/// It represents the interface to the graphics device where everything is rendered
class DLL_PUBLIC IGfxDevice
{
  public:
	virtual ~IGfxDevice() { }

	/// Sets screen resolution with two integers
	virtual void setResolution(int width, int height) = 0;
	/// Sets screen resolution with a `Vector2<int>` object
	virtual void setResolution(Vector2i size) = 0;

	/// Toggles between fullscreen and windowed mode
	virtual void toggleFullScreen() = 0;

	/// Sets the application window title
	virtual void setWindowTitle(const char *windowTitle) = 0;

	/// Returns device resolution as a `Vector2<int>` object
	inline Vector2i resolution() const { return Vector2i(width_, height_); }
	/// Returns device width
	inline int width() const { return width_; }
	/// Returns device height
	inline int height() const { return height_; }
	/// Returns device aspect ratio
	inline float aspect() const { return width_ / float(height_); }
	/// Returns true if the device renders in full screen
	inline bool isFullScreen() const { return isFullScreen_; }
	/// Returns display mode
	inline const DisplayMode& mode() const { return mode_; }

  protected:
	/// Device width
	int width_;
	/// Device height
	int height_;
	/// Display properties
	DisplayMode mode_;
	/// Whether device rendering occurs in full screen
	bool isFullScreen_;

  private:
	/// Sets up the initial OpenGL state
	virtual void setupGL();

	/// Updates the screen swapping back and front buffers
	virtual void update() = 0;
	/// Clears the screen
	virtual void clear();

	friend class Application;
};

}

#endif
