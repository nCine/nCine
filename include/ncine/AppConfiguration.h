#ifndef CLASS_NCINE_APPCONFIGURATION
#define CLASS_NCINE_APPCONFIGURATION

#include <nctl/String.h>
#include "ILogger.h"

namespace ncine {

/// The class storing initialization settings for an nCine application
class DLL_PUBLIC AppConfiguration
{
  public:
	/// Default constructor setting the defaults
	AppConfiguration();

	/// \returns True if the OpenGL profile is going to be core
	inline bool glCoreProfile() const { return glCoreProfile_; }
	/// \returns True if the OpenGL context is going to be forward compatible
	inline bool glForwardCompatible() const { return glForwardCompatible_; }
	/// \returns The major version number of the OpenGL context
	inline unsigned int glMajorVersion() const { return glMajorVersion_; }
	/// \returns The minor version number of the OpenGL context
	inline unsigned int glMinorVersion() const { return glMinorVersion_; }
	/// \returns True if the OpenGL context is going to be a debug context
	inline bool glDebugContext() const { return withGlDebug_; }
	/// \returns The update time in seconds for the profile text nodes
	inline float profileTextUpdateTime() const { return profileTextUpdateTime_; }

	/// \returns The path for the application to load data from
	const nctl::String &dataPath() const;
	/// \returns The name of the log file
	inline const nctl::String &logFile() const { return logFile_; }
	/// \returns The logging level for messages printed on the console
	inline ILogger::LogLevel consoleLogLevel() const { return consoleLogLevel_; }
	/// \returns The logging level for messages written in the log file
	inline ILogger::LogLevel fileLogLevel() const { return fileLogLevel_; }
	/// \returns The interval for frame timer accumulation average and log
	inline float frameTimerLogInterval() const { return frameTimerLogInterval_; }
	/// \returns The screen width
	inline unsigned int xResolution() const { return xResolution_; }
	/// \returns The screen height
	inline unsigned int yResolution() const { return yResolution_; }
	/// \returns True if the screen is going to be in fullscreen mode
	inline bool inFullscreen() const { return inFullscreen_; }
	/// \returns True if the window is going to be resizable
	inline bool isResizable() const { return isResizable_; }
	/// \returns The window title
	inline const nctl::String &windowTitle() const { return windowTitle_; }
	/// \returns The window icon filename
	inline const nctl::String &windowIconFilename() const { return windowIconFilename_; }
	/// \returns The font texture filename for profiler information text
	inline const nctl::String &fontTexFilename() const { return fontTexFilename_; }
	/// \returns The font FNT filename for profiler information text
	inline const nctl::String &fontFntFilename() const { return fontFntFilename_; }
	/// \returns The maximum size in bytes for each VBO collecting geometry data
	inline unsigned long vboSize() const { return vboSize_; }
	/// \returns The maximum size in bytes for each IBO collecting index data
	inline unsigned long iboSize() const { return iboSize_; }
	/// \returns The maximum size for the pool of VAOs
	inline unsigned int vaoPoolSize() const { return vaoPoolSize_; }
	/// \returns True if the profiler graphs are enabled
	inline bool withProfilerGraphs() const { return withProfilerGraphs_; }
	/// \returns True if the information text is enabled
	inline bool withInfoText() const { return withInfoText_; }
	/// \returns True if the audio subsystem is enabled
	inline bool withAudio() const { return withAudio_; }
	/// \returns True if the threading subsystem is enabled
	inline bool withThreads() const { return withThreads_; }
	/// \returns True if the scenegraph based rendering is enabled
	inline bool withScenegraph() const { return withScenegraph_; }
	/// \returns True if the vertical synchronization is enabled
	inline bool withVSync() const { return withVSync_; }
	/// \returns True if the OpenGL debug context is enabled
	inline bool withGlDebug() const { return withGlDebug_; }

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
	/// Sets the window resizable flag
	void setResizable(bool resizable);
	/// Sets the window title
	void setWindowTitle(const nctl::String &windowTitle);
	/// Sets the window icon filename
	void setWindowIconFilename(const nctl::String &windowIconFilename);
	/// Sets the font texture filename for profiler information text
	void setFontTexFilename(const nctl::String &fontTexFilename);
	/// Sets the font FNT filename for profiler information text
	void setFontFntFilename(const nctl::String &fontFntFilename);
	/// Sets the maximum size in bytes for each VBO collecting geometry data
	void setVboSize(unsigned long vboSize);
	/// Sets the maximum size in bytes for each IBO collecting index data
	void setIboSize(unsigned long iboSize);
	/// Sets the maximum size for the pool of VAOs
	void setVaoPoolSize(unsigned int vaoPoolSize);
	/// Enables the profiler graphs
	void enableProfilerGraphs(bool shouldEnable);
	/// Enables the information text
	void enableInfoText(bool shouldEnable);
	/// Enables the audio subsystem
	void enableAudio(bool shouldEnable);
	/// Enables the threading subsystem
	void enableThreads(bool shouldEnable);
	/// Enables the scenegraph based rendering
	void enableScenegraph(bool shouldEnable);
	/// Enables vertical synchronization
	void enableVSync(bool shouldEnable);
	/// Enables OpenGL debug context
	void enableGlDebug(bool shouldEnable);

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
	bool isResizable_;
	nctl::String windowTitle_;
	nctl::String windowIconFilename_;
	nctl::String fontTexFilename_;
	nctl::String fontFntFilename_;
	unsigned long vboSize_;
	unsigned long iboSize_;
	unsigned int vaoPoolSize_;
	bool withProfilerGraphs_;
	bool withInfoText_;
	bool withAudio_;
	bool withThreads_;
	bool withScenegraph_;
	bool withVSync_;
	bool withGlDebug_;
};

}

#endif
