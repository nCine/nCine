#ifndef CLASS_NCINE_IGFXDEVICE
#define CLASS_NCINE_IGFXDEVICE

#include "Vector2.h"
#include "Rect.h"
#include "DisplayMode.h"
#include "AppConfiguration.h"

#ifdef __EMSCRIPTEN__
class EmscriptenUiEvent;
class EmscriptenFullscreenChangeEvent;
class EmscriptenFocusEvent;
#endif

namespace ncine {

class Colorf;

/// It represents the interface to the graphics device where everything is rendered
class DLL_PUBLIC IGfxDevice
{
  public:
	static const unsigned int MaxMonitors = 4;
#if defined(WITH_QT5)
	// Qt5 cannot query the list of supported video modes of a monitor
	static const unsigned int MaxVideoModes = 1;
#elif defined(__ANDROID__)
	static const unsigned int MaxVideoModes = 16;
#else
	static const unsigned int MaxVideoModes = 128;
#endif

	/// A structure used to initialize window properties
	struct WindowMode
	{
		WindowMode()
		    : width(0), height(0), isFullScreen(false), isResizable(false) {}
		WindowMode(unsigned int w, unsigned int h, bool fullscreen, bool resizable)
		    : width(w), height(h), isFullScreen(fullscreen), isResizable(resizable) {}

		unsigned int width;
		unsigned int height;
		bool isFullScreen;
		bool isResizable;
	};

	/// A structure representing a video mode supported by a monitor
	struct VideoMode
	{
		VideoMode()
		    : width(0), height(0), refreshRate(0.0f), redBits(8), greenBits(8), blueBits(8) {}
		inline bool operator==(const VideoMode &mode) const
		{
			return (width == mode.width && height == mode.height && refreshRate == mode.refreshRate &&
			        redBits == mode.redBits && greenBits == mode.greenBits && blueBits == mode.blueBits);
		}
		inline bool operator!=(const VideoMode &mode) const { return !operator==(mode); }

		unsigned int width;
		unsigned int height;
		float refreshRate;
		unsigned char redBits;
		unsigned char greenBits;
		unsigned char blueBits;
	};

	/// A structure representing a connected monitor
	struct Monitor
	{
		/// The monitor name
		const char *name;
		/// The position of the monitor's viewport on the virtual screen
		Vector2i position;
		/// The horizontal and vertical dots per inch
		Vector2i dpi;
		/// The content scale factor
		Vector2f scale;

		/// The number of video modes in the array
		unsigned int numVideoModes;
		/// The array of video modes supported by the monitor
		VideoMode videoModes[MaxVideoModes];
	};

	/// Contains the attributes to create an OpenGL context
	struct GLContextInfo
	{
		explicit GLContextInfo(const AppConfiguration &appCfg)
		    : majorVersion(appCfg.glMajorVersion()), minorVersion(appCfg.glMinorVersion()),
		      coreProfile(appCfg.glCoreProfile()), forwardCompatible(appCfg.glForwardCompatible()),
		      debugContext(appCfg.withGlDebugContext) {}

		unsigned int majorVersion;
		unsigned int minorVersion;
		bool coreProfile;
		bool forwardCompatible;
		bool debugContext;
	};

	IGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode);
	virtual ~IGfxDevice() {}

	/// Sets the number of vertical blanks to occur before a buffer swap
	/*! An interval of `-1` will enable adaptive v-sync if available */
	virtual void setSwapInterval(int interval) = 0;

	/// Returns true if the device renders in full screen
	inline bool isFullScreen() const { return isFullScreen_; }
	/// Sets the full screen flag of the window
	virtual void setFullScreen(bool fullScreen) = 0;

	/// Returns true if the window is resizable
	inline bool isResizable() const { return isResizable_; }

	/// Returns window horizontal position
	inline virtual int windowPositionX() const { return 0; }
	/// Returns window vertical position
	inline virtual int windowPositionY() const { return 0; }
	/// Returns window position as a `Vector2i` object
	inline virtual const Vector2i windowPosition() const { return Vector2i(0, 0); }
	/// Sets the position of the application window with two integers
	virtual void setWindowPosition(int x, int y) = 0;
	/// Sets the position of the application window with a `Vector2<int>` object
	inline void setWindowPosition(Vector2i position) { setWindowPosition(position.x, position.y); }

	/// Returns the window or video mode width in screen coordinates
	inline int width() const { return width_; }
	/// Returns the window or video mode height in screen coordinates
	inline int height() const { return height_; }
	/// Returns the window or video mode resolution in screen coordinates as a `Vector2i` object
	inline Vector2i resolution() const { return Vector2i(width_, height_); }
	/// Returns the window or video mode resolution in screen coordinates as a `Rectf` object
	inline Rectf screenRect() const { return Rectf(0.0f, 0.0f, static_cast<float>(width_), static_cast<float>(height_)); }
	/// Returns the window or video mode resolution aspect ratio
	inline float aspect() const { return width_ / static_cast<float>(height_); }
	/// Sets the window size with two integers
	/*! \note If the application is in full screen this method will have no effect. */
	virtual void setWindowSize(int width, int height) = 0;
	/// Sets the window size with a `Vector2i` object
	/*! \note If the application is in full screen this method will have no effect. */
	inline void setWindowSize(Vector2i size) { setWindowSize(size.x, size.y); }

	/// Returns the window width in pixels
	/*! It may differs from `width()` on HiDPI screens */
	inline int drawableWidth() const { return drawableWidth_; }
	/// Returns the window height in pixels
	/*! It may differs from `height()` on HiDPI screens */
	inline int drawableHeight() const { return drawableHeight_; }
	/// Returns the window resolution in pixels as a `Vector2i` object
	inline Vector2i drawableResolution() const { return Vector2i(drawableWidth_, drawableHeight_); }
	/// Returns the window resolution in pixels as a `Rectf` object
	inline Rectf drawableScreenRect() const { return Rectf(0.0f, 0.0f, static_cast<float>(drawableWidth_), static_cast<float>(drawableHeight_)); }
	/// Returns the window drawable resolution aspect ratio
	inline float drawableAspect() const { return drawableWidth_ / static_cast<float>(drawableHeight_); }

	/// Sets the application window title
	virtual void setWindowTitle(const char *windowTitle) = 0;
	/// Sets the application window icon
	virtual void setWindowIcon(const char *windowIconFilename) = 0;
	/// Highlights the application window to notify the user
	inline virtual void flashWindow() const {}

	/// Returns the OpenGL context creation attributes
	inline const GLContextInfo &glContextInfo() const { return glContextInfo_; }
	/// Returns display mode
	inline const DisplayMode &displayMode() const { return displayMode_; }

	/// Returns the number of connected monitors
	unsigned int numMonitors() const;
	/// Returns the array index of the primary monitor
	/*! \note This is usually the monitor where elements like the task bar or global menu bar are located. */
	inline virtual unsigned int primaryMonitorIndex() const { return 0; }
	/// Returns the array index of the monitor associated with the window
	inline virtual unsigned int windowMonitorIndex() const { return 0; }
	/// Returns the specified monitor
	const Monitor &monitor(unsigned int index) const;
	/// Returns the monitor that hosts the window
	inline const Monitor &monitor() const { return monitor(windowMonitorIndex()); }

	/// Returns the current video mode for the specified monitor
	virtual const VideoMode &currentVideoMode(unsigned int monitorIndex) const = 0;
	/// Returns the current video mode for the monitor that hosts the window
	inline const VideoMode &currentVideoMode() const { return currentVideoMode(windowMonitorIndex()); }
	/// Sets the video mode that will be used in full screen by the monitor that hosts the window
	/*! \note Call this method <b>before>/b> enabling full screen */
	inline virtual bool setVideoMode(unsigned int modeIndex) { return false; }

  protected:
	/// The default value used for a monitor DPI when the real vaue cannot be queried
	static const float DefaultDpi;

	/// Window width in screen coordinates
	int width_;
	/// Window height in screen coordinates
	int height_;
	/// Window width in pixels (for HiDPI screens)
	int drawableWidth_;
	/// Window height in pixels (for HiDPI screens)
	int drawableHeight_;
	/// Whether rendering occurs in full screen
	bool isFullScreen_;
	/// Whether the window is resizable
	bool isResizable_;
	/// OpenGL context creation attributes
	GLContextInfo glContextInfo_;
	/// Display properties
	DisplayMode displayMode_;

	Monitor monitors_[MaxMonitors];
	unsigned int numMonitors_;
	/// Used as a cache to avoid searching the current video mode in a monitor's array
	mutable VideoMode currentVideoMode_;

	/// Inits the OpenGL viewport based on the drawable resolution
	void initGLViewport();

	/// Updates the array of connected monitors
	inline virtual void updateMonitors() {}

  private:
	/// Sets up the initial OpenGL state for the scenegraph
	virtual void setupGL();

	/// Updates the screen swapping back and front buffers
	virtual void update() = 0;

	friend class Application;
#if defined(WITH_SDL)
	friend class PCApplication;
#endif

#ifdef __EMSCRIPTEN__
	static int resize_callback(int eventType, const EmscriptenUiEvent *event, void *userData);
	static int fullscreenchange_callback(int eventType, const EmscriptenFullscreenChangeEvent *event, void *userData);
	static int focus_callback(int eventType, const EmscriptenFocusEvent *event, void *userData);
#endif
};

}

#endif
