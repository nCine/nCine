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

	IGfxDevice(int width, int height, const GLContextInfo &glContextInfo, const DisplayMode &mode, bool isFullScreen);
	virtual ~IGfxDevice() { }

	/// Sets screen resolution with two integers
	virtual void setResolution(int width, int height) = 0;
	/// Sets screen resolution with a `Vector2<int>` object
	virtual void setResolution(Vector2i size) = 0;

	/// Toggles between fullscreen and windowed mode
	virtual void toggleFullScreen() = 0;

	/// Sets the application window title
	virtual void setWindowTitle(const char *windowTitle) = 0;
	/// Sets the application window icon
	virtual void setWindowIcon(const char *iconFilename) = 0;

	/// Returns device width
	inline int width() const { return width_; }
	/// Returns device height
	inline int height() const { return height_; }
	/// Returns device resolution as a `Vector2<int>` object
	inline const Vector2i resolution() const { return Vector2i(width_, height_); }
	/// Returns device resolution as a `Rect<float>` object
	inline const Rectf screenRect() const { return Rectf(0.0f, 0.0f, static_cast<float>(width_), static_cast<float>(height_)); }
	/// Returns device aspect ratio
	inline float aspect() const { return width_ / static_cast<float>(height_); }

	/// Sets the OpenGL clear color through four float
	void setClearColor(float red, float green, float blue, float alpha);
	/// Sets the OpenGL clear color through a `Colorf` object
	void setClearColor(const Colorf &color);

	/// Returns the OpenGL context creation attributes
	inline const GLContextInfo &glContextInfo() const { return glContextInfo_; }
	/// Returns display mode
	inline const DisplayMode &mode() const { return mode_; }
	/// Returns true if the device renders in full screen
	inline bool isFullScreen() const { return isFullScreen_; }

  protected:
	/// Device width
	int width_;
	/// Device height
	int height_;
	/// OpenGL context creation attributes
	GLContextInfo glContextInfo_;
	/// Display properties
	DisplayMode mode_;
	/// Whether device rendering occurs in full screen
	bool isFullScreen_;

  private:
	/// Sets up the initial OpenGL state for the scenegraph
	virtual void setupGL();

	/// Updates the screen swapping back and front buffers
	virtual void update() = 0;
	/// Clears the screen
	virtual void clear();

	friend class Application;
};

}

#endif
