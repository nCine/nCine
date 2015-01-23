#include "AppConfiguration.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Empty constructor setting the defaults
AppConfiguration::AppConfiguration()
	: logFile_(256)
	, consoleLogLevel_(ILogger::LOG_VERBOSE)
	, fileLogLevel_(ILogger::LOG_OFF)
	, xResolution_(960), yResolution_(640)
	, inFullscreen_(false)
	, windowTitle_("nCine")
	, fontTexFilename_(256)
	, fontFntFilename_(256)
	, withProfilerGraphs_(true)
	, withProfilerText_(true)
	, withAudio_(true)
	, withThreads_(true)
	, withScenegraph_(true)
{
#ifdef __ANDROID__
	logFile_ = "/sdcard/ncine_log.txt";
	fileLogLevel_ = ILogger::LOG_VERBOSE;

//	fontTexFilename_ = "asset::trebuchet16_128.dds.mp3";
//	fontFntFilename_ = "asset::trebuchet16_128.fnt";
	fontTexFilename_ = "/sdcard/ncine/trebuchet32_256_4444.pvr";
	fontFntFilename_ = "/sdcard/ncine/trebuchet32_256.fnt";
//	fontTexFilename_ = "/sdcard/ncine/trebuchet16_128.dds";
//	fontFntFilename_ = "/sdcard/ncine/trebuchet16_128.fnt";
#else
	logFile_ = "ncine_log.txt";

	fontTexFilename_ = "fonts/trebuchet32_256.png";
	fontFntFilename_ = "fonts/trebuchet32_256.fnt";
#endif
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Sets the name of the log file
void AppConfiguration::setLogFile(const String& logFile)
{
	logFile_ = logFile;
}

/// Sets the logging level for messages printed on the console
void AppConfiguration::setConsoleLogLevel(ILogger::LogLevel logLevel)
{
	consoleLogLevel_ = logLevel;
}

/// Sets the logging level for messages written in the log file
void AppConfiguration::setFileLogLevel(ILogger::LogLevel logLevel)
{
	fileLogLevel_ = logLevel;
}

/// Sets the screen resolution
void AppConfiguration::setResolution(unsigned int x, unsigned int y)
{
	xResolution_ = x;
	yResolution_ = y;
}

/// Sets the fullscreen mode
void AppConfiguration::setFullScreen(bool inFullscreen)
{
	inFullscreen_ = inFullscreen;
}

/// Enables the profiler graphs
void AppConfiguration::enableProfilerGraphs(bool shouldEnable)
{
	withProfilerGraphs_ = shouldEnable;
}

/// Enables the profiler information text
void AppConfiguration::enableProfilerText(bool shouldEnable)
{
	withProfilerText_ = shouldEnable;
}

/// Enables the audio subsystem
void AppConfiguration::enableAudio(bool shouldEnable)
{
	withAudio_ = shouldEnable;
}

/// Enables the threading subsystem
void AppConfiguration::enableThreads(bool shouldEnable)
{
	withThreads_ = shouldEnable;
}

/// Enables the scenegraph based rendering
void AppConfiguration::enableScenegraph(bool shouldEnable)
{
	withScenegraph_ = shouldEnable;
}

}
