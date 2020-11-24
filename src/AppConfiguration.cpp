#include "AppConfiguration.h"
#include "FileSystem.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AppConfiguration::AppConfiguration()
    : logFile(128),
#ifdef NCINE_DEBUG
      consoleLogLevel(ILogger::LogLevel::INFO),
#else
      consoleLogLevel(ILogger::LogLevel::ERROR),
#endif
      fileLogLevel(ILogger::LogLevel::OFF),
      frameTimerLogInterval(5.0f),
      resolution(1280, 720),
      inFullscreen(false),
      isResizable(false),
      frameLimit(0),
      windowTitle(128),
      windowIconFilename(128),
      useBufferMapping(false),
      deferShaderQueries(true),
      fixedBatchSize(10),
#if defined(WITH_IMGUI) || defined(WITH_NUKLEAR)
      vboSize(512 * 1024),
      iboSize(128 * 1024),
#else
      vboSize(64 * 1024),
      iboSize(8 * 1024),
#endif
      vaoPoolSize(16),
      withDebugOverlay(false),
      withAudio(true),
      withThreads(false),
      withScenegraph(true),
      withVSync(true),
      withGlDebugContext(false),

      // Compile-time variables
      glCoreProfile_(true),
      glForwardCompatible_(true),
#if defined(__ANDROID__) || defined(WITH_ANGLE) || defined(__EMSCRIPTEN__)
      glMajorVersion_(3),
      glMinorVersion_(0),
#else
      glMajorVersion_(3),
      glMinorVersion_(3),
#endif
      profileTextUpdateTime_(0.2f)
{
	logFile = "ncine_log.txt";
	windowTitle = "nCine";
	windowIconFilename = "icons/icon48.png";

#if defined(__ANDROID__)
	dataPath() = "asset::";
#elif defined(__EMSCRIPTEN__)
	dataPath() = "/";
	// Always disable mapping on Emscripten as it is not supported by WebGL 2
	useBufferMapping = false;
#endif
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const nctl::String &AppConfiguration::dataPath() const
{
	return fs::dataPath_;
}

/*! Allows to set the value of the static field `FileSystem::dataPath_`. */
nctl::String &AppConfiguration::dataPath()
{
	return fs::dataPath_;
}

}
