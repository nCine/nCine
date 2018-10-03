#ifndef CLASS_NCINE_IGFXDEVICE
#define CLASS_NCINE_IGFXDEVICE

#include "Vector2.h"
#include "Rect.h"
#include "DisplayMode.h"
#include "AppConfiguration.h"

namespace ncine {

class Colorf;

/// It represents the interface to the graphics device where everything is rendered
class DLL_PUBLIC IGfxDevice
{
  public:
	/// A structure used to initialize window properties
	struct WindowMode
	{
		WindowMode() : width(0), height(0), isFullScreen(false), isResizable(false) { }
		WindowMode(unsigned int w, unsigned int h, bool fullscreen, bool resizable)
			: width(w), height(h), isFullScreen(fullscreen), isResizable(resizable) { }

		unsigned int width;
		unsigned int height;
		bool isFullScreen;
		bool isResizable;
	};

	/// Contains the attributes to create an OpenGL context
	struct GLContextInfo
	{
		GLContextInfo(const AppConfiguration &appCfg)
			: majorVersion(appCfg.glMajorVersion()), minorVersion(appCfg.glMinorVersion()),
			  coreProfile(appCfg.glCoreProfile()), forwardCompatible(appCfg.glForwardCompatible()),
			  debugContext(appCfg.glDebugContext()) { }

		unsigned int majorVersion;
		unsigned int minorVersion;
		bool coreProfile;
		bool forwardCompatible;
		bool debugContext;
	};

	IGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode);
	virtual ~IGfxDevice() { }

	/// Sets screen resolution with two integers
	virtual void setResolution(int width, int height) = 0;
	/// Sets screen resolution with a `Vector2<int>` object
	virtual void setResolution(Vector2i size) = 0;

	/// Returns true if the device renders in full screen
	inline bool isFullScreen() const { return isFullScreen_; }
	/// Sets the full screen flag of the window
	virtual void setFullScreen(bool fullScreen) = 0;
	/// Toggles between full screen and windowed mode
	inline void toggleFullScreen() { bool fullScreen = !isFullScreen_; setFullScreen(fullScreen); }

	/// Returns true if the window is resizable
	inline bool isResizable() const { return isResizable_; }

	/// Sets the application window title
	virtual void setWindowTitle(const char *windowTitle) = 0;
	/// Sets the application window icon
	virtual void setWindowIcon(const char *iconFilename) = 0;

	/// Returns device width
	inline int width() const { return width_; }
	/// Returns device height
	inline int height() const { return height_; }
	/// Returns device resolution as a `Vector2i` object
	inline const Vector2i resolution() const { return Vector2i(width_, height_); }
	/// Returns device resolution as a `Rectf` object
	inline const Rectf screenRect() const { return Rectf(0.0f, 0.0f, static_cast<float>(width_), static_cast<float>(height_)); }
	/// Returns device aspect ratio
	inline float aspect() const { return width_ / static_cast<float>(height_); }

	/// Sets the OpenGL clear color through four float
	void setClearColor(float red, float green, float blue, float alpha);
	/// Sets the OpenGL clear color through a `Colorf` object
	void setClearColor(const Colorf &color);

	/// Resizes the OpenGL viewport
	void setViewport(int width, int height);
	/// Resizes the OpenGL viewport through a `Vector2i` object
	void setViewport(const Vector2i &size);

	/// Returns the OpenGL context creation attributes
	inline const GLContextInfo &glContextInfo() const { return glContextInfo_; }
	/// Returns display mode
	inline const DisplayMode &displayMode() const { return displayMode_; }

  protected:
	/// Device width
	int width_;
	/// Device height
	int height_;
	/// Whether device rendering occurs in full screen
	bool isFullScreen_;
	/// Whether the window is resizable
	bool isResizable_;
	/// OpenGL context creation attributes
	GLContextInfo glContextInfo_;
	/// Display properties
	DisplayMode displayMode_;

  private:
	/// Sets up the initial OpenGL state for the scenegraph
	virtual void setupGL();

	/// Updates the screen swapping back and front buffers
	virtual void update() = 0;
	/// Clears the screen
	virtual void clear();

	friend class Application;
#if defined(WITH_SDL)
	friend class PCApplication;
#endif
};

}

#endif
