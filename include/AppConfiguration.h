#ifndef CLASS_NCINE_APPCONFIGURATION
#define CLASS_NCINE_APPCONFIGURATION

#include "nctl/String.h"
#include "ILogger.h"

namespace ncine {

/// The class storing initialization settings for an nCine application
class DLL_PUBLIC AppConfiguration
{
  public:
	/// Default constructor setting the defaults
	AppConfiguration();

	/// Returns true if the OpenGL profile is going to be core
	bool glCoreProfile() const { return glCoreProfile_; }
	/// Returns true if the OpenGL context is going to be forward compatible
	bool glForwardCompatible() const { return glForwardCompatible_; }
	/// Returns the major version number of the OpenGL context
	unsigned int glMajorVersion() const { return glMajorVersion_; }
	/// Returns the minor version number of the OpenGL context
	unsigned int glMinorVersion() const { return glMinorVersion_; }
	/// Returns true if the OpenGL context is going to be a debug context
	bool glDebugContext() const { return withGLDebug_; }
	/// Returns the update time in seconds for the profile text nodes
	float profileTextUpdateTime() const { return profileTextUpdateTime_; }

	/// Sets the path for the application to load data from
	void setDataPath(const nctl::String &dataPath);
	/// Sets the name of the log file
	void setLogFile(const nctl::String &logFile);
	/// Sets the logging level for messages printed on the console
	void setConsoleLogLevel(ILogger::LogLevel logLevel);
	/// Sets the logging level for messages written in the log file
	void setFileLogLevel(ILogger::LogLevel logLevel);
	/// Sets the interval for frame timer accumulation average and log
	void setFrameTimerLogInterval(float logInterval);
	/// Sets the screen resolution
	/*! \note If either \p x or \p y are zero then the screen resolution will not be changed. */
	void setResolution(unsigned int x, unsigned int y);
	/// Sets the fullscreen mode
	void setFullScreen(bool inFullscreen);
	/// Sets the window title
	void setWindowTitle(const nctl::String &windowTitle);
	/// Sets the window icon filename
	void setWindowIconFilename(const nctl::String &windowIconFilename);
	/// Sets the font texture filename for profiler information text
	void setFontTexFilename(const nctl::String &fontTexFilename);
	/// Sets the font FNT filename for profiler information text
	void setFontFntFilename_(const nctl::String &fontFntFilename);
	/// Enables the profiler graphs
	void enableProfilerGraphs(bool shouldEnable);
	/// Enables the profiler information text
	void enableProfilerText(bool shouldEnable);
	/// Enables the audio subsystem
	void enableAudio(bool shouldEnable);
	/// Enables the threading subsystem
	void enableThreads(bool shouldEnable);
	/// Enables the scenegraph based rendering
	void enableScenegraph(bool shouldEnable);
	/// Enables vertical synchronization
	void enableVSync(bool shouldEnable);
	/// Enables OpenGL debug context
	void enableGLDebug(bool shouldEnable);

  private:
	// Pre-configured compile-time variables
	const bool glCoreProfile_;
	const bool glForwardCompatible_;
	const unsigned int glMajorVersion_;
	const unsigned int glMinorVersion_;
	const float profileTextUpdateTime_;

	// User configurable compile-time variables
	nctl::String logFile_;
	ILogger::LogLevel consoleLogLevel_;
	ILogger::LogLevel fileLogLevel_;
	float frameTimerLogInterval_;
	unsigned int xResolution_;
	unsigned int yResolution_;
	bool inFullscreen_;
	nctl::String windowTitle_;
	nctl::String windowIconFilename_;
	nctl::String fontTexFilename_;
	nctl::String fontFntFilename_;
	bool withProfilerGraphs_;
	bool withProfilerText_;
	bool withAudio_;
	bool withThreads_;
	bool withScenegraph_;
	bool withVSync_;
	bool withGLDebug_;

	friend class Application;
	friend class PCApplication;
	friend class AndroidApplication;
};

}

#endif
