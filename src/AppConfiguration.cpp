#include "AppConfiguration.h"
#include "IFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AppConfiguration::AppConfiguration()
	: glCoreProfile_(true)
	, glForwardCompatible_(true)
#ifdef __ANDROID__
	, glMajorVersion_(2)
	, glMinorVersion_(0)
#else
	, glMajorVersion_(3)
	, glMinorVersion_(3)
#endif
	, profileTextUpdateTime_(0.2f)
	, logFile_(128)
	, consoleLogLevel_(ILogger::LogLevel::INFO)
	, fileLogLevel_(ILogger::LogLevel::OFF)
	, frameTimerLogInterval_(5.0f)
	, xResolution_(1280), yResolution_(720)
	, inFullscreen_(false)
	, windowTitle_(128)
	, windowIconFilename_(128)
	, fontTexFilename_(128)
	, fontFntFilename_(128)
	, withProfilerGraphs_(true)
	, withProfilerText_(true)
	, withAudio_(true)
	, withThreads_(false)
	, withScenegraph_(true)
	, withVSync_(true)
	, withGLDebug_(false)
{
	logFile_ = "ncine_log.txt";
#ifdef __ANDROID__
	fontTexFilename_ = "fonts/DroidSans32_256_888.ktx";
#else
	windowTitle_ = "nCine";
	windowIconFilename_ = "icons/icon48.png";
	fontTexFilename_ = "fonts/DroidSans32_256.png";
#endif
	fontFntFilename_ = "fonts/DroidSans32_256.fnt";
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! Directly sets the value of the static field `IFile::dataPath_`. */
void AppConfiguration::setDataPath(const nctl::String &dataPath)
{
	IFile::dataPath_ = dataPath;
}

/*! The data path will be prefixed automatically before loading. */
void AppConfiguration::setLogFile(const nctl::String &logFile)
{
	logFile_ = logFile;
}

void AppConfiguration::setConsoleLogLevel(ILogger::LogLevel logLevel)
{
	consoleLogLevel_ = logLevel;
}

void AppConfiguration::setFileLogLevel(ILogger::LogLevel logLevel)
{
	fileLogLevel_ = logLevel;
}

/*! \note A value of 0 disables frame timer logging. */
void AppConfiguration::setFrameTimerLogInterval(float logInterval)
{
	frameTimerLogInterval_ = logInterval;
}

void AppConfiguration::setResolution(unsigned int x, unsigned int y)
{
	xResolution_ = x;
	yResolution_ = y;
}

void AppConfiguration::setFullScreen(bool inFullscreen)
{
	inFullscreen_ = inFullscreen;
}

void AppConfiguration::setWindowTitle(const nctl::String &windowTitle)
{
	windowTitle_ = windowTitle;
}

void AppConfiguration::setWindowIconFilename(const nctl::String &windowIconFilename)
{
	windowIconFilename_ = windowIconFilename;
}

/*! The data path will be prefixed automatically before loading. */
void AppConfiguration::setFontTexFilename(const nctl::String &fontTexFilename)
{
	fontTexFilename_ = fontTexFilename;
}

/*! The data path will be prefixed automatically before loading. */
void AppConfiguration::setFontFntFilename_(const nctl::String &fontFntFilename)
{
	fontFntFilename_ = fontFntFilename;
}

void AppConfiguration::enableProfilerGraphs(bool shouldEnable)
{
	withProfilerGraphs_ = shouldEnable;
}

void AppConfiguration::enableProfilerText(bool shouldEnable)
{
	withProfilerText_ = shouldEnable;
}

void AppConfiguration::enableAudio(bool shouldEnable)
{
	withAudio_ = shouldEnable;
}

void AppConfiguration::enableThreads(bool shouldEnable)
{
	withThreads_ = shouldEnable;
}

void AppConfiguration::enableScenegraph(bool shouldEnable)
{
	withScenegraph_ = shouldEnable;
}

void AppConfiguration::enableVSync(bool shouldEnable)
{
	withVSync_ = shouldEnable;
}

void AppConfiguration::enableGLDebug(bool shouldEnable)
{
	withGLDebug_ = shouldEnable;
}

}
