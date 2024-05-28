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
	#ifdef _WIN32
      // Disable console logging on Windows to avoid allocating a console
      consoleLogLevel(ILogger::LogLevel::OFF),
	#else
      consoleLogLevel(ILogger::LogLevel::ERROR),
	#endif
#endif
      fileLogLevel(ILogger::LogLevel::OFF),
      frameTimerLogInterval(5.0f),
      resolution(1280, 720),
      refreshRate(0.0f),
      windowPosition(WindowPositionIgnore, WindowPositionIgnore),
      fullScreen(false),
      resizable(false),
      windowScaling(true),
      frameLimit(0),
      windowTitle(128),
      windowIconFilename(128),
      useBufferMapping(false),
      deferShaderQueries(true),
#if defined(__EMSCRIPTEN__)
      fixedBatchSize(10),
#elif defined(WITH_ANGLE)
      fixedBatchSize(64),
#else
      fixedBatchSize(0),
#endif
#if defined(WITH_ANGLE) || defined(__ANDROID__)
      useBinaryShaderCache(true),
#else
      useBinaryShaderCache(false),
#endif
      shaderCacheDirname(64),
      compileBatchedShadersTwice(true),
#if defined(WITH_IMGUI) || defined(WITH_NUKLEAR)
      vboSize(512 * 1024),
      iboSize(128 * 1024),
#else
      vboSize(64 * 1024),
      iboSize(8 * 1024),
#endif
      vaoPoolSize(16),
      renderCommandPoolSize(32),
      outputAudioFrequency(0),
      monoAudioSources(31),
      stereoAudioSources(1),
      withDebugOverlay(false),
      withAudio(true),
      withThreads(false),
      withScenegraph(true),
      withVSync(true),
      withGlDebugContext(false),
      withConsoleColors(true),

      // Compile-time variables
      glCoreProfile_(true),
      glForwardCompatible_(true),
#if defined(WITH_OPENGLES) || defined(__EMSCRIPTEN__)
      glMajorVersion_(3),
      glMinorVersion_(0),
#else
      glMajorVersion_(3),
      glMinorVersion_(3),
#endif
      profileTextUpdateTime_(0.2f),
      argc_(0),
      argv_(nullptr)
{
	logFile = "ncine_log.txt";
	windowTitle = "nCine";
	windowIconFilename = "icons/icon48.png";
	shaderCacheDirname = "nCineShaderCache";

#if defined(__ANDROID__)
	dataPath() = "asset::";
#elif defined(__EMSCRIPTEN__)
	dataPath() = "/";
	// Always disable mapping on Emscripten as it is not supported by WebGL 2
	useBufferMapping = false;
	// Accessing binary representations of compiled shader programs is not supported by WebGL 2
	useBinaryShaderCache = false;
#endif

#if defined(__linux__) && defined(WITH_SDL)
	// DPI queries do not seem to work reliably on X11 with SDL2
	windowScaling = false;
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

const char *AppConfiguration::argv(int index) const
{
	if (index < argc_)
		return argv_[index];
	return nullptr;
}

}
