#ifndef CLASS_NCINE_APPCONFIGURATION
#define CLASS_NCINE_APPCONFIGURATION

#include "ncString.h"
#include "ILogger.h"

namespace ncine {

/// A class that stores the settings for an nCine application
class DLL_PUBLIC AppConfiguration
{
  public:
	// Empty constructor setting the defaults
	AppConfiguration();

	// Sets the path for the application to load data from
	void setDataPath(const String& dataPath);
	// Sets the name of the log file
	void setLogFile(const String& logFile);
	// Sets the logging level for messages printed on the console
	void setConsoleLogLevel(ILogger::LogLevel logLevel);
	// Sets the logging level for messages written in the log file
	void setFileLogLevel(ILogger::LogLevel logLevel);
	// Sets the interval for frame timer accumulation average and log
	void setFrameTimerLogInterval(float logInterval);
	// Sets the screen resolution
	void setResolution(unsigned int x, unsigned int y);
	// Sets the fullscreen mode
	void setFullScreen(bool inFullscreen);
	// Sets the window title
	void setWindowTitle(const String& windowTitle);
	// Sets the font texture filename for profiler information text
	void setFontTexFilename(const String& fontTexFilename);
	// Sets the font FNT filename for profiler information text
	void setFontFntFilename_(const String& fontFntFilename);
	// Enables the profiler graphs
	void enableProfilerGraphs(bool shouldEnable);
	// Enables the profiler information text
	void enableProfilerText(bool shouldEnable);
	// Enables the audio subsystem
	void enableAudio(bool shouldEnable);
	// Enables the threading subsystem
	void enableThreads(bool shouldEnable);
	// Enables the scenegraph based rendering
	void enableScenegraph(bool shouldEnable);

  private:
	// Pre-configured compile-time variables
	const float profileTextUpdateTime_;

	// User configurable compile-time variables
	String logFile_;
	ILogger::LogLevel consoleLogLevel_;
	ILogger::LogLevel fileLogLevel_;
	float frameTimerLogInterval_;
	unsigned int xResolution_;
	unsigned int yResolution_;
	bool inFullscreen_;
	String windowTitle_;
	String fontTexFilename_;
	String fontFntFilename_;
	bool withProfilerGraphs_;
	bool withProfilerText_;
	bool withAudio_;
	bool withThreads_;
	bool withScenegraph_;

	friend class Application;
#ifdef __ANDROID__
	friend class AndroidApplication;
#endif
};

}

#endif