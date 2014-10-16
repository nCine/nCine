#ifndef CLASS_NCIGFXDEVICE
#define CLASS_NCIGFXDEVICE

#include "ncPoint.h"
#include "ncDisplayMode.h"

/// It represents the interface to the graphics device where everything is rendered
class ncIGfxDevice
{
  public:
	virtual ~ncIGfxDevice() = 0;

	/// Sets screen resolution with two integers
	virtual void setResolution(int width, int height) = 0;
	/// Sets screen resolution with the Size class
	virtual void setResolution(ncPoint size) = 0;

	/// Toggles between fullscreen and windowed mode
	virtual void toggleFullScreen() { }

	/// Updates the screen swapping back and front buffers
	virtual void update() = 0;
	/// Clears the screen
	virtual void clear();

	/// Sets the application window title
	virtual void setWindowTitle(const char *windowTitle) = 0;

	/// Returns device resolution
	inline ncPoint resolution() const { return ncPoint(width_, height_); }
	/// Returns device width
	inline int width() const { return width_; }
	/// Returns device height
	inline int height() const { return height_; }
	/// Returns true if the device renders inside a window
	inline bool isWindowed() const { return isWindowed_; }
	/// Returns display mode
	inline const ncDisplayMode& mode() const { return mode_; }

  protected:
	/// Device width
	int width_;
	/// Device height
	int height_;
	/// Display properties
	ncDisplayMode mode_;
	/// Device rendering occurs inside a window
	bool isWindowed_;

	// Initilizes starting OpenGL state
	virtual void initGL();
};

inline ncIGfxDevice::~ncIGfxDevice() { }

#endif
