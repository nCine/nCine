#include "AppConfiguration.h"
#include "IFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AppConfiguration::AppConfiguration()
	: logFile(128)
#ifdef NCINE_DEBUG
	, consoleLogLevel(ILogger::LogLevel::INFO)
#else
	, consoleLogLevel(ILogger::LogLevel::ERROR)
#endif
	, fileLogLevel(ILogger::LogLevel::OFF)
	, frameTimerLogInterval(5.0f)
	, xResolution(1280), yResolution(720)
	, inFullscreen(false)
	, isResizable(false)
	, windowTitle(128)
	, windowIconFilename(128)
	, fontTexFilename(128)
	, fontFntFilename(128)
#ifdef WITH_IMGUI
	, vboSize(512 * 1024)
	, iboSize(128 * 1024)
#else
	, vboSize(64 * 1024)
	, iboSize(8 * 1024)
#endif
	, vaoPoolSize(16)
#ifdef NCINE_DEBUG
	, withProfilerGraphs(true)
	, withInfoText(true)
#else
	, withProfilerGraphs(false)
	, withInfoText(false)
#endif
	, withAudio(true)
	, withThreads(false)
	, withScenegraph(true)
	, withVSync(true)
	, withGlDebugContext(false)

	// Compile-time variables
	, glCoreProfile_(true)
	, glForwardCompatible_(true)
#ifdef __ANDROID__
	, glMajorVersion_(3)
	, glMinorVersion_(0)
#else
	, glMajorVersion_(3)
	, glMinorVersion_(3)
#endif
	, profileTextUpdateTime_(0.2f)
{
	logFile = "ncine_log.txt";
#ifdef __ANDROID__
	fontTexFilename = "fonts/DroidSans32_256_ETC2.ktx";
#else
	windowTitle = "nCine";
	windowIconFilename = "icons/icon48.png";
	fontTexFilename = "fonts/DroidSans32_256.png";
#endif
	fontFntFilename = "fonts/DroidSans32_256.fnt";
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const nctl::String &AppConfiguration::dataPath() const
{
	return IFile::dataPath_;
}

/*! Allows to set the value of the static field `IFile::dataPath_`. */
nctl::String &AppConfiguration::dataPath()
{
	return IFile::dataPath_;
}

void AppConfiguration::setResolution(unsigned int x, unsigned int y)
{
	xResolution = x;
	yResolution = y;
}

}
