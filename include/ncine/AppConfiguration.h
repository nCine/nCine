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

	// User configurable compile-time variables

	/// The path for the application to load data from
	nctl::String logFile;
	/// The logging level for messages printed on the console
	ILogger::LogLevel consoleLogLevel;
	/// The logging level for messages written in the log file
	ILogger::LogLevel fileLogLevel;
	/// The interval for frame timer accumulation average and log
	float frameTimerLogInterval;

	/// The screen width
	unsigned int xResolution;
	/// The screen height
	unsigned int yResolution;
	/// The flag is `true` if the screen is going to be in fullscreen mode
	bool inFullscreen;
	/// The flag is `true` if the window is going to be resizable
	bool isResizable;

	/// The window title
	nctl::String windowTitle;
	/// The window icon filename
	nctl::String windowIconFilename;
	/// The font texture filename for profiler information text
	nctl::String fontTexFilename;
	/// The font FNT filename for profiler information text
	nctl::String fontFntFilename;

	/// The maximum size in bytes for each VBO collecting geometry data
	unsigned long vboSize;
	/// The maximum size in bytes for each IBO collecting index data
	unsigned long iboSize;
	/// The maximum size for the pool of VAOs
	unsigned int vaoPoolSize;

	/// The flag is `true` if the profiler graphs are enabled
	bool withProfilerGraphs;
	/// The flag is `true` if the information text is enabled
	bool withInfoText;
	/// The flag is `true` if the audio subsystem is enabled
	bool withAudio;
	/// The flag is `true` if the threading subsystem is enabled
	bool withThreads;
	/// The flag is `true` if the scenegraph based rendering is enabled
	bool withScenegraph;
	/// The flag is `true` if the vertical synchronization is enabled
	bool withVSync;
	/// The flag is `true` if the OpenGL debug context is enabled
	bool withGlDebugContext;

	/// \returns The path for the application to load data from
	const nctl::String &dataPath() const;
	/// \returns The path for the application to load data from
	nctl::String &dataPath();

	/// Sets the screen resolution
	/*! \note If either \p x or \p y are zero then the screen resolution will not be changed. */
	void setResolution(unsigned int x, unsigned int y);

	/// \returns True if the OpenGL profile is going to be core
	inline bool glCoreProfile() const { return glCoreProfile_; }
	/// \returns True if the OpenGL context is going to be forward compatible
	inline bool glForwardCompatible() const { return glForwardCompatible_; }
	/// \returns The major version number of the OpenGL context
	inline unsigned int glMajorVersion() const { return glMajorVersion_; }
	/// \returns The minor version number of the OpenGL context
	inline unsigned int glMinorVersion() const { return glMinorVersion_; }
	/// \returns The update time in seconds for the profile text nodes
	inline float profileTextUpdateTime() const { return profileTextUpdateTime_; }

  private:
	// Pre-configured compile-time variables
	const bool glCoreProfile_;
	const bool glForwardCompatible_;
	const unsigned int glMajorVersion_;
	const unsigned int glMinorVersion_;
	const float profileTextUpdateTime_;
};

}

#endif