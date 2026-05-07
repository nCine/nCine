#include <cstdio> // for sscanf()
#include "AppConfiguration.h"
#include "FileSystem.h"
#include <nctl/CString.h>
#include <nctl/StaticString.h>

#if defined(__ANDROID__)
	#include <ncine/AssetFile.h>
#endif

namespace ncine {

namespace {
	// Root
	#define ENV_PREFIX "NCINE_APPCFG_"

	// Domains
	#define ENV_LOGGING "LOGGING_"
	#define ENV_WINDOW "WINDOW_"
	#define ENV_GRAPHICS "GRAPHICS_"
	#define ENV_OPENGL "OPENGL_"
	#define ENV_AUDIO "AUDIO_"
	#define ENV_JOBSYSTEM "JOBSYSTEM_"
	#define ENV_FEATURES "FEATURES_"

	// Leaf keys
	#define ENV_ENABLED "ENABLED"
	// ----- Logging -----
	#define ENV_FILE "FILE"
	#define ENV_CONSOLE_LEVEL "CONSOLE_LEVEL"
	#define ENV_FILE_LEVEL "FILE_LEVEL"
	#define ENV_FRAME_TIMER_INTERVAL "FRAME_TIMER_INTERVAL"
	#define ENV_CONSOLE_COLORS "CONSOLE_COLORS"

	// ----- Window -----
	#define ENV_RESOLUTION "RESOLUTION"
	#define ENV_REFRESH_RATE "REFRESH_RATE"
	#define ENV_POSITION "POSITION"
	#define ENV_FULLSCREEN "FULLSCREEN"
	#define ENV_RESIZABLE "RESIZABLE"
	#define ENV_SCALING "SCALING"

	#define ENV_TITLE "TITLE"
	#define ENV_ICON_FILENAME "ICON_FILENAME"

	// ----- Graphics -----
	#define ENV_VSYNC "VSYNC"
	#define ENV_FRAME_LIMIT "FRAME_LIMIT"

	// ----- Graphics.OpenGL -----
	#define ENV_DEBUG_CONTEXT "DEBUG_CONTEXT"
	#define ENV_BUFFER_MAPPING "BUFFER_MAPPING"

	#define ENV_VBO_SIZE "VBO_SIZE"
	#define ENV_IBO_SIZE "IBO_SIZE"
	#define ENV_VAO_POOL_SIZE "VAO_POOL_SIZE"
	#define ENV_RENDER_COMMAND_POOL_SIZE "RENDER_COMMAND_POOL_SIZE"

	#define ENV_DEFER_SHADER_QUERIES "DEFER_SHADER_QUERIES"
	#define ENV_FIXED_BATCH_SIZE "FIXED_BATCH_SIZE"
	#define ENV_BINARY_SHADER_CACHE "BINARY_SHADER_CACHE"
	#define ENV_SHADER_CACHE_DIRNAME "SHADER_CACHE_DIRNAME"
	#define ENV_COMPILE_BATCHED_SHADERS_TWICE "COMPILE_BATCHED_SHADERS_TWICE"

	// ----- Audio -----
	#define ENV_FREQUENCY "FREQUENCY"
	#define ENV_MONO_SOURCES "MONO_SOURCES"
	#define ENV_STEREO_SOURCES "STEREO_SOURCES"

	// ----- Job System -----
	#define ENV_NUM_THREADS "NUM_THREADS"

	// ----- Features -----
	#define ENV_SCENEGRAPH "SCENEGRAPH"
	#define ENV_DEBUG_OVERLAY "DEBUG_OVERLAY"

	// -----
	#define ENV_DATA_PATH "DATA_PATH"

	// Helper
	#define ENV2(a, b) ENV_PREFIX a b
	#define ENV3(a, b, c) ENV_PREFIX a b c

	bool readBoolEnvVar(const char *envVarName, bool currentValue)
	{
		const char *envString = getenv(envVarName);
		if (envString != nullptr && nctl::strnlen(envString, 2) > 0)
		{
			int value = currentValue ? 1 : 0;
			if (sscanf(envString, "%1d", &value) == 1 && (value == 0 || value == 1))
				return static_cast<bool>(value);
		}
		return currentValue;
	}

	unsigned int readUintEnvVar(const char *envVarName, unsigned int currentValue)
	{
		const char *envString = getenv(envVarName);
		if (envString != nullptr && nctl::strnlen(envString, 16) > 0)
		{
			unsigned int value = currentValue;
			if (sscanf(envString, "%u", &value) == 1)
				return value;
		}
		return currentValue;
	}

	unsigned long int readUlongEnvVar(const char *envVarName, unsigned long int currentValue)
	{
		const char *envString = getenv(envVarName);
		if (envString != nullptr && nctl::strnlen(envString, 32) > 0)
		{
			unsigned long int value = currentValue;
			if (sscanf(envString, "%lu", &value) == 1)
				return value;
		}
		return currentValue;
	}

	float readFloatEnvVar(const char *envVarName, float currentValue)
	{
		const char *envString = getenv(envVarName);
		if (envString != nullptr && nctl::strnlen(envString, 32) > 0)
		{
			float value = currentValue;
			if (sscanf(envString, "%f", &value) == 1)
				return value;
		}
		return currentValue;
	}

	void readStringEnvVar(const char *envVarName, nctl::String &target)
	{
		const char *envString = getenv(envVarName);
		if (envString != nullptr)
		{
			const size_t envLength = nctl::strnlen(envString, target.capacity() + 1);
			if (envLength > 0 && envLength <= target.capacity())
				target = envString;
		}
	}

	ILogger::LogLevel readLogLevelEnvVar(const char *envVarName, ILogger::LogLevel currentValue)
	{
		const char *envString = getenv(envVarName);
		if (envString != nullptr && nctl::strnlen(envString, 2) > 0)
		{
			int value = static_cast<int>(currentValue);
			if (sscanf(envString, "%d", &value) == 1 &&
			    value >= static_cast<int>(ILogger::LogLevel::OFF) &&
			    value <= static_cast<int>(ILogger::LogLevel::FATAL))
			{
				return static_cast<ILogger::LogLevel>(value);
			}
		}
		return currentValue;
	}

	Vector2i readVector2iEnvVar(const char *envVarName, const Vector2i &currentValue)
	{
		const char *envString = getenv(envVarName);
		if (envString != nullptr && nctl::strnlen(envString, 32) > 0)
		{
			Vector2i value = currentValue;
			int scanned = 0;

			scanned = sscanf(envString, "%dx%d", &value.x, &value.y);
			if (scanned == 2)
				return value;

			scanned = sscanf(envString, "%d,%d", &value.x, &value.y);
			if (scanned == 2)
				return value;
		}
		return currentValue;
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AppConfiguration::Graphics::OpenGLCapabilities::OpenGLCapabilities()
    : coreProfile(true),
      forwardCompatible(true),
#if defined(WITH_OPENGLES) || defined(__EMSCRIPTEN__)
      majorVersion(3),
      minorVersion(0)
#else
      majorVersion(3),
      minorVersion(3)
#endif
{
}

AppConfiguration::AppConfiguration()
    : profileTextUpdateTime(0.2f),
      argc_(0),
      argv_(nullptr)
{
	applyDefaults();
	old_.assign(*this);
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

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AppConfiguration::OldValues::assign(const AppConfiguration &appCfg)
{
	logging = appCfg.logging;
	window = appCfg.window;
	graphics.vsync = appCfg.graphics.vsync;
	graphics.frameLimit = appCfg.graphics.frameLimit;
	graphics.opengl = appCfg.graphics.opengl;
	audio = appCfg.audio;
	jobSystem = appCfg.jobSystem;
	features = appCfg.features;
	dataPath = appCfg.dataPath();
}

void AppConfiguration::applyDefaults()
{
	// ----- Logging -----
	logging.file = "ncine_log.txt";

#ifdef NCINE_DEBUG
	logging.consoleLevel = ILogger::LogLevel::INFO;
#else
	#ifdef _WIN32
	// Disable console logging on Windows to avoid allocating a console
	logging.consoleLevel = ILogger::LogLevel::OFF;
	#else
	logging.consoleLevel = ILogger::LogLevel::ERROR;
	#endif
#endif

#if defined(WITH_CRASHPAD)
	logging.fileLevel = ILogger::LogLevel::DEBUG;
#endif

	// ----- Window -----
#if defined(__linux__) && defined(WITH_SDL)
	// DPI queries do not seem to work reliably on X11 with SDL2
	window.scaling = false;
#endif
	window.title = "nCine";
	window.iconFilename = "icons/icon48.png";

	// ----- Graphics.OpenGL -----
#if defined(WITH_IMGUI) || defined(WITH_NUKLEAR)
	graphics.opengl.vboSize = 512 * 1024;
	graphics.opengl.iboSize = 128 * 1024;
#endif

#if defined(__EMSCRIPTEN__)
	graphics.opengl.fixedBatchSize = 10;
#elif defined(WITH_ANGLE)
	graphics.opengl.fixedBatchSize = 64;
#endif

#if defined(WITH_ANGLE) || defined(__ANDROID__)
	graphics.opengl.useBinaryShaderCache = true;
#endif
	graphics.opengl.shaderCacheDirname = "nCineShaderCache";

	// ------ Features -----
#if !defined(WITH_SCENEGRAPH)
	features.scenegraph = false;
#endif

	// ------
#if defined(__ANDROID__)
	dataPath() = AssetFile::Prefix;
#elif defined(__EMSCRIPTEN__)
	dataPath() = "/";
#endif

	// This method should be called a second time after the `onPreInit()` callback
	applyOverrides();
}

void AppConfiguration::applyOverrides()
{
	// ----- Graphics.OpenGL -----
#if defined(__EMSCRIPTEN__)
	// Always disable mapping on Emscripten as it is not supported by WebGL 2
	graphics.opengl.useBufferMapping = false;

	// Accessing binary representations of compiled shader programs is not supported by WebGL 2
	graphics.opengl.useBinaryShaderCache = false;
#endif
}

void AppConfiguration::logFields() const
{
	// ----- Logging -----
	LOGD("Logging Configuration");
	LOGD_X("- File: \"%s\"", logging.file.data());
	LOGD_X("- Console Level: %d", static_cast<int>(logging.consoleLevel));
	LOGD_X("- File Level: %d", static_cast<int>(logging.fileLevel));
	LOGD_X("- Frametimer Interval: %.2f s", logging.frameTimerInterval);
	LOGD_X("- Console Colors: %s", logging.consoleColors ? "true" : "false");

	// ----- Window -----
	LOGD("Window Configuration");
	LOGD_X("- Resolution: %d x %d", window.resolution.x, window.resolution.y);
	LOGD_X("- Refresh Rate: %.2f Hz", window.refreshRate);

	nctl::StaticString<128> auxString;
	auxString.assign("- Position: ");
	if (window.position.x == Window::IgnorePosition)
		auxString.append("Ignore x ");
	else
		auxString.formatAppend("%d x ", window.position.x);
	if (window.position.y == Window::IgnorePosition)
		auxString.append("Ignore");
	else
		auxString.formatAppend("%d", window.position.y);
	LOGD_X("%s", auxString.data());

	LOGD_X("- Fullscreen: %s", window.fullscreen ? "true" : "false");
	LOGD_X("- Resizable: %s", window.resizable ? "true" : "false");
	LOGD_X("- Scaling: %s", window.scaling ? "true" : "false");

	LOGD_X("- Title: \"%s\"", window.title.data());
	LOGD_X("- Icon File: \"%s\"", window.iconFilename.data());

	// ----- Graphics -----
	LOGD("Graphics Configuration");
	LOGD_X("- V-Sync: %s", graphics.vsync ? "true" : "false");
	LOGD_X("- Frame Limit: %u", graphics.frameLimit);
	// ----- Graphics.OpenGLCapabilities -----
	LOGD("- OpenGL Capabilities");
#if !defined(WITH_OPENGLES) && !defined(__EMSCRIPTEN__)
	LOGD_X("  - Core Profile: %s", graphics.openglCaps.coreProfile ? "true" : "false");
	LOGD_X("  - Forward Compatible: %s", graphics.openglCaps.forwardCompatible ? "true" : "false");
#endif
	LOGD_X("  - Version: %d.%d", graphics.openglCaps.majorVersion, graphics.openglCaps.minorVersion);
	// ----- Graphics.OpenGL -----
	LOGD("- OpenGL Configuration");
	LOGD_X("  - OpenGL Debug Context: %s", graphics.opengl.debugContext ? "true" : "false");
	LOGD_X("  - Buffer Mapping: %s", graphics.opengl.useBufferMapping ? "true" : "false");

	LOGD_X("  - VBO Size: %lu bytes", graphics.opengl.vboSize);
	LOGD_X("  - IBO Size: %lu bytes", graphics.opengl.iboSize);
	LOGD_X("  - VAO Pool Size: %u", graphics.opengl.vaoPoolSize);
	LOGD_X("  - RenderCommand Pool Size: %u", graphics.opengl.renderCommandPoolSize);

	LOGD_X("  - Defer Shader Queries: %s", graphics.opengl.deferShaderQueries ? "true" : "false");
#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
	LOGD_X("  - Fixed Batch Size: %u", graphics.opengl.fixedBatchSize);
#endif
	LOGD_X("  - Binary Shader Cache: %s", graphics.opengl.useBinaryShaderCache ? "true" : "false");
	LOGD_X("  - Shader Cache Directory Name: \"%s\"", graphics.opengl.shaderCacheDirname.data());
	LOGD_X("  - Compile Batched Shaders Twice: %s", graphics.opengl.compileBatchedShadersTwice ? "true" : "false");

	// ----- Audio -----
	LOGD("Audio Configuration");
	LOGD_X("- Enabled: %s", audio.enabled ? "true" : "false");
	LOGD_X("- Frequency: %u Hz", audio.frequency);
	LOGD_X("- Mono Sources: %u", audio.monoSources);
	LOGD_X("- Stereo Sources: %u", audio.stereoSources);

	// ----- Job System -----
	LOGD("Job System Configuration");
	LOGD_X("- Enabled: %s", jobSystem.enabled ? "true" : "false");
	LOGD_X("- Number of Threads: %u", jobSystem.numThreads);

	// ----- Features -----
	LOGD("Features Configuration");
	LOGD_X("- Scenegraph: %s", features.scenegraph ? "true" : "false");
	LOGD_X("- Debug Overlay: %s", features.debugOverlay ? "true" : "false");

	// -----
	LOGD("Additional Configuration");
	LOGD_X("- Profile Text Update Time: %.2f s", profileTextUpdateTime);
	LOGD_X("- Data Path: \"%s\"", dataPath().data());

	for (int i = 0; i < argc_; i++)
		LOGD_X("- argv[%u]: \"%s\"", i, argv_[i]);
}

void AppConfiguration::readEnvVariables()
{
	// ----- Logging -----
	// NCINE_APPCFG_LOGGING_FILE
	old_.logging.file = logging.file;
	constexpr const char EnvLoggingFile[] = ENV2(ENV_LOGGING, ENV_FILE);
	readStringEnvVar(EnvLoggingFile, logging.file);

	// NCINE_APPCFG_LOGGING_CONSOLE_LEVEL
	old_.logging.consoleLevel = logging.consoleLevel;
	constexpr const char EnvLoggingConsoleLevel[] = ENV2(ENV_LOGGING, ENV_CONSOLE_LEVEL);
	logging.consoleLevel = readLogLevelEnvVar(EnvLoggingConsoleLevel, logging.consoleLevel);

	// NCINE_APPCFG_LOGGING_FILE_LEVEL
	old_.logging.fileLevel = logging.fileLevel;
	constexpr const char EnvLoggingFileLevel[] = ENV2(ENV_LOGGING, ENV_FILE_LEVEL);
	logging.fileLevel = readLogLevelEnvVar(EnvLoggingFileLevel, logging.fileLevel);

	// NCINE_APPCFG_LOGGING_FRAME_TIMER_INTERVAL
	old_.logging.frameTimerInterval = logging.frameTimerInterval;
	constexpr const char EnvLoggingFrameTimerInterval[] = ENV2(ENV_LOGGING, ENV_FRAME_TIMER_INTERVAL);
	logging.frameTimerInterval = readFloatEnvVar(EnvLoggingFrameTimerInterval, logging.frameTimerInterval);
	if (logging.frameTimerInterval < 0.0f)
		logging.frameTimerInterval = old_.logging.frameTimerInterval;

	// NCINE_APPCFG_LOGGING_CONSOLE_COLORS
	old_.logging.consoleColors = logging.consoleColors;
	constexpr const char EnvLoggingConsoleColors[] = ENV2(ENV_LOGGING, ENV_CONSOLE_COLORS);
	logging.consoleColors = readBoolEnvVar(EnvLoggingConsoleColors, logging.consoleColors);

	// ----------------------------------------------------------------
	// ----- Window -----

	// NCINE_APPCFG_WINDOW_RESOLUTION
	old_.window.resolution = window.resolution;
	constexpr const char EnvWindowResolution[] = ENV2(ENV_WINDOW, ENV_RESOLUTION);
	window.resolution = readVector2iEnvVar(EnvWindowResolution, window.resolution);

	// NCINE_APPCFG_WINDOW_REFRESH_RATE
	old_.window.refreshRate = window.refreshRate;
	constexpr const char EnvWindowRefreshRate[] = ENV2(ENV_WINDOW, ENV_REFRESH_RATE);
	window.refreshRate = readFloatEnvVar(EnvWindowRefreshRate, window.refreshRate);
	if (window.refreshRate < 0.0f)
		window.refreshRate = old_.window.refreshRate;

	// NCINE_APPCFG_WINDOW_POSITION
	old_.window.position = window.position;
	constexpr const char EnvWindowPosition[] = ENV2(ENV_WINDOW, ENV_POSITION);
	window.position = readVector2iEnvVar(EnvWindowPosition, window.position);

	// NCINE_APPCFG_WINDOW_FULLSCREEN
	old_.window.fullscreen = window.fullscreen;
	constexpr const char EnvWindowFullscreen[] = ENV2(ENV_WINDOW, ENV_FULLSCREEN);
	window.fullscreen = readBoolEnvVar(EnvWindowFullscreen, window.fullscreen);

	// NCINE_APPCFG_WINDOW_RESIZABLE
	old_.window.resizable = window.resizable;
	constexpr const char EnvWindowResizable[] = ENV2(ENV_WINDOW, ENV_RESIZABLE);
	window.resizable = readBoolEnvVar(EnvWindowResizable, window.resizable);

	// NCINE_APPCFG_WINDOW_SCALING
	old_.window.scaling = window.scaling;
	constexpr const char EnvWindowScaling[] = ENV2(ENV_WINDOW, ENV_SCALING);
	window.scaling = readBoolEnvVar(EnvWindowScaling, window.scaling);

	// ----------------------------------------------------------------

	// NCINE_APPCFG_WINDOW_TITLE
	old_.window.title = window.title;
	constexpr const char EnvWindowTitle[] = ENV2(ENV_WINDOW, ENV_TITLE);
	readStringEnvVar(EnvWindowTitle, window.title);

	// NCINE_APPCFG_WINDOW_ICON_FILENAME
	old_.window.iconFilename = window.iconFilename;
	constexpr const char EnvWindowIcon[] = ENV2(ENV_WINDOW, ENV_ICON_FILENAME);
	readStringEnvVar(EnvWindowIcon, window.iconFilename);

	// ----------------------------------------------------------------
	// ----- Graphics -----

	// NCINE_APPCFG_GRAPHICS_VSYNC
	old_.graphics.vsync = graphics.vsync;
	constexpr const char EnvGraphicsVsync[] = ENV2(ENV_GRAPHICS, ENV_VSYNC);
	graphics.vsync = readBoolEnvVar(EnvGraphicsVsync, graphics.vsync);

	// NCINE_APPCFG_GRAPHICS_FRAME_LIMIT
	old_.graphics.frameLimit = graphics.frameLimit;
	constexpr const char EnvGraphicsFrameLimit[] = ENV2(ENV_GRAPHICS, ENV_FRAME_LIMIT);
	graphics.frameLimit = readUintEnvVar(EnvGraphicsFrameLimit, graphics.frameLimit);

	// ----------------------------------------------------------------
	// ----- Graphics.OpenGL -----

	// NCINE_APPCFG_GRAPHICS_OPENGL_DEBUG_CONTEXT
	old_.graphics.opengl.debugContext = graphics.opengl.debugContext;
	constexpr const char EnvGLDebugContext[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_DEBUG_CONTEXT);
	graphics.opengl.debugContext = readBoolEnvVar(EnvGLDebugContext, graphics.opengl.debugContext);

	// NCINE_APPCFG_GRAPHICS_OPENGL_BUFFER_MAPPING
	old_.graphics.opengl.useBufferMapping = graphics.opengl.useBufferMapping;
	constexpr const char EnvGLBufferMapping[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_BUFFER_MAPPING);
	graphics.opengl.useBufferMapping = readBoolEnvVar(EnvGLBufferMapping, graphics.opengl.useBufferMapping);

	// ----------------------------------------------------------------

	// NCINE_APPCFG_GRAPHICS_OPENGL_VBO_SIZE
	old_.graphics.opengl.vboSize = graphics.opengl.vboSize;
	constexpr const char EnvGLVboSize[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_VBO_SIZE);
	graphics.opengl.vboSize = readUlongEnvVar(EnvGLVboSize, graphics.opengl.vboSize);
	if (graphics.opengl.vboSize == 0)
		graphics.opengl.vboSize = old_.graphics.opengl.vboSize;

	// NCINE_APPCFG_GRAPHICS_OPENGL_IBO_SIZE
	old_.graphics.opengl.iboSize = graphics.opengl.iboSize;
	constexpr const char EnvGLIboSize[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_IBO_SIZE);
	graphics.opengl.iboSize = readUlongEnvVar(EnvGLIboSize, graphics.opengl.iboSize);
	if (graphics.opengl.iboSize == 0)
		graphics.opengl.iboSize = old_.graphics.opengl.iboSize;

	// NCINE_APPCFG_GRAPHICS_OPENGL_VAO_POOL_SIZE
	old_.graphics.opengl.vaoPoolSize = graphics.opengl.vaoPoolSize;
	constexpr const char EnvGLVaoPoolSize[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_VAO_POOL_SIZE);
	graphics.opengl.vaoPoolSize = readUintEnvVar(EnvGLVaoPoolSize, graphics.opengl.vaoPoolSize);

	// NCINE_APPCFG_GRAPHICS_OPENGL_RENDER_COMMAND_POOL_SIZE
	old_.graphics.opengl.renderCommandPoolSize = graphics.opengl.renderCommandPoolSize;
	constexpr const char EnvGLRenderCmdPool[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_RENDER_COMMAND_POOL_SIZE);
	graphics.opengl.renderCommandPoolSize = readUintEnvVar(EnvGLRenderCmdPool, graphics.opengl.renderCommandPoolSize);

	// ----------------------------------------------------------------

	// NCINE_APPCFG_GRAPHICS_OPENGL_DEFER_SHADER_QUERIES
	old_.graphics.opengl.deferShaderQueries = graphics.opengl.deferShaderQueries;
	constexpr const char EnvGLDeferQueries[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_DEFER_SHADER_QUERIES);
	graphics.opengl.deferShaderQueries = readBoolEnvVar(EnvGLDeferQueries, graphics.opengl.deferShaderQueries);

	// NCINE_APPCFG_GRAPHICS_OPENGL_FIXED_BATCH_SIZE
	old_.graphics.opengl.fixedBatchSize = graphics.opengl.fixedBatchSize;
	constexpr const char EnvGLFixedBatch[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_FIXED_BATCH_SIZE);
	graphics.opengl.fixedBatchSize = readUintEnvVar(EnvGLFixedBatch, graphics.opengl.fixedBatchSize);

	// NCINE_APPCFG_GRAPHICS_OPENGL_BINARY_SHADER_CACHE
	old_.graphics.opengl.useBinaryShaderCache = graphics.opengl.useBinaryShaderCache;
	constexpr const char EnvGLBinaryCache[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_BINARY_SHADER_CACHE);
	graphics.opengl.useBinaryShaderCache = readBoolEnvVar(EnvGLBinaryCache, graphics.opengl.useBinaryShaderCache);

	// NCINE_APPCFG_GRAPHICS_OPENGL_SHADER_CACHE_DIRNAME
	old_.graphics.opengl.shaderCacheDirname = graphics.opengl.shaderCacheDirname;
	constexpr const char EnvGLCacheDir[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_SHADER_CACHE_DIRNAME);
	readStringEnvVar(EnvGLCacheDir, graphics.opengl.shaderCacheDirname);

	// NCINE_APPCFG_GRAPHICS_OPENGL_COMPILE_BATCHED_SHADERS_TWICE
	old_.graphics.opengl.compileBatchedShadersTwice = graphics.opengl.compileBatchedShadersTwice;
	constexpr const char EnvGLCompileTwice[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_COMPILE_BATCHED_SHADERS_TWICE);
	graphics.opengl.compileBatchedShadersTwice = readBoolEnvVar(EnvGLCompileTwice, graphics.opengl.compileBatchedShadersTwice);

	// ----------------------------------------------------------------
	// ----- Audio -----

	// NCINE_APPCFG_AUDIO_ENABLED
	old_.audio.enabled = audio.enabled;
	constexpr const char EnvAudioEnabled[] = ENV2(ENV_AUDIO, ENV_ENABLED);
	audio.enabled = readBoolEnvVar(EnvAudioEnabled, audio.enabled);

	// NCINE_APPCFG_AUDIO_FREQUENCY
	old_.audio.frequency = audio.frequency;
	constexpr const char EnvAudioFrequency[] = ENV2(ENV_AUDIO, ENV_FREQUENCY);
	audio.frequency = readUintEnvVar(EnvAudioFrequency, audio.frequency);

	// NCINE_APPCFG_AUDIO_MONO_SOURCES
	old_.audio.monoSources = audio.monoSources;
	constexpr const char EnvAudioMono[] = ENV2(ENV_AUDIO, ENV_MONO_SOURCES);
	audio.monoSources = readUintEnvVar(EnvAudioMono, audio.monoSources);

	// NCINE_APPCFG_AUDIO_STEREO_SOURCES
	old_.audio.stereoSources = audio.stereoSources;
	constexpr const char EnvAudioStereo[] = ENV2(ENV_AUDIO, ENV_STEREO_SOURCES);
	audio.stereoSources = readUintEnvVar(EnvAudioStereo, audio.stereoSources);

	// ----------------------------------------------------------------
	// ----- Job System -----

	// NCINE_APPCFG_JOBSYSTEM_ENABLED
	old_.jobSystem.enabled = jobSystem.enabled;
	constexpr const char EnvJobEnabled[] = ENV2(ENV_JOBSYSTEM, ENV_ENABLED);
	jobSystem.enabled = readBoolEnvVar(EnvJobEnabled, jobSystem.enabled);

	// NCINE_APPCFG_JOBSYSTEM_NUM_THREADS
	old_.jobSystem.numThreads = jobSystem.numThreads;
	constexpr const char EnvJobThreads[] = ENV2(ENV_JOBSYSTEM, ENV_NUM_THREADS);
	jobSystem.numThreads = readUintEnvVar(EnvJobThreads, jobSystem.numThreads);

	// ----------------------------------------------------------------
	// ----- Features -----

	// NCINE_APPCFG_FEATURES_SCENEGRAPH
	old_.features.scenegraph = features.scenegraph;
	constexpr const char EnvFeatureScenegraph[] = ENV2(ENV_FEATURES, ENV_SCENEGRAPH);
	features.scenegraph = readBoolEnvVar(EnvFeatureScenegraph, features.scenegraph);

	// NCINE_APPCFG_FEATURES_DEBUG_OVERLAY
	old_.features.debugOverlay = features.debugOverlay;
	constexpr const char EnvFeatureDebugOverlay[] = ENV2(ENV_FEATURES, ENV_DEBUG_OVERLAY);
	features.debugOverlay = readBoolEnvVar(EnvFeatureDebugOverlay, features.debugOverlay);

	// ----------------------------------------------------------------

	// NCINE_APPCFG_DATA_PATH
	old_.dataPath = dataPath();
	constexpr const char EnvDataPath[] = ENV2(ENV_PREFIX, ENV_DATA_PATH);
	readStringEnvVar(EnvDataPath, dataPath());
}

void AppConfiguration::logEnvVariables() const
{
	// ----- Logging -----
	if (logging.file != old_.logging.file)
	{
		constexpr const char Name[] = ENV2(ENV_LOGGING, ENV_FILE);
		LOGI_X("%s=\"%s\" overrides compiled value \"%s\"",
		       Name, logging.file.data(), old_.logging.file.data());
	}

	if (logging.consoleLevel != old_.logging.consoleLevel)
	{
		constexpr const char Name[] = ENV2(ENV_LOGGING, ENV_CONSOLE_LEVEL);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, logging.consoleLevel, old_.logging.consoleLevel);
	}

	if (logging.fileLevel != old_.logging.fileLevel)
	{
		constexpr const char Name[] = ENV2(ENV_LOGGING, ENV_FILE_LEVEL);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, logging.fileLevel, old_.logging.fileLevel);
	}

	if (logging.frameTimerInterval != old_.logging.frameTimerInterval)
	{
		constexpr const char Name[] = ENV2(ENV_LOGGING, ENV_FRAME_TIMER_INTERVAL);
		LOGI_X("%s=%f overrides compiled value %f",
		       Name, logging.frameTimerInterval, old_.logging.frameTimerInterval);
	}

	if (logging.consoleColors != old_.logging.consoleColors)
	{
		constexpr const char Name[] = ENV2(ENV_LOGGING, ENV_CONSOLE_COLORS);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, logging.consoleColors, old_.logging.consoleColors);
	}

	// ----------------------------------------------------------------
	// ----- Window -----

	if (window.resolution != old_.window.resolution)
	{
		constexpr const char Name[] = ENV2(ENV_WINDOW, ENV_RESOLUTION);
		LOGI_X("%s=%dx%d overrides compiled value %dx%d",
		       Name,
		       window.resolution.x, window.resolution.y,
		       old_.window.resolution.x, old_.window.resolution.y);
	}

	if (window.refreshRate != old_.window.refreshRate)
	{
		constexpr const char Name[] = ENV2(ENV_WINDOW, ENV_REFRESH_RATE);
		LOGI_X("%s=%f overrides compiled value %f",
		       Name, window.refreshRate, old_.window.refreshRate);
	}

	if (window.position != old_.window.position)
	{
		constexpr const char Name[] = ENV2(ENV_WINDOW, ENV_POSITION);
		LOGI_X("%s=%dx%d overrides compiled value %dx%d",
		       Name,
		       window.position.x, window.position.y,
		       old_.window.position.x, old_.window.position.y);
	}

	if (window.fullscreen != old_.window.fullscreen)
	{
		constexpr const char Name[] = ENV2(ENV_WINDOW, ENV_FULLSCREEN);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, window.fullscreen, old_.window.fullscreen);
	}

	if (window.resizable != old_.window.resizable)
	{
		constexpr const char Name[] = ENV2(ENV_WINDOW, ENV_RESIZABLE);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, window.resizable, old_.window.resizable);
	}

	if (window.scaling != old_.window.scaling)
	{
		constexpr const char Name[] = ENV2(ENV_WINDOW, ENV_SCALING);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, window.scaling, old_.window.scaling);
	}

	// ----------------------------------------------------------------

	if (window.title != old_.window.title)
	{
		constexpr const char Name[] = ENV2(ENV_WINDOW, ENV_TITLE);
		LOGI_X("%s=\"%s\" overrides compiled value \"%s\"",
		       Name, window.title.data(), old_.window.title.data());
	}

	if (window.iconFilename != old_.window.iconFilename)
	{
		constexpr const char Name[] = ENV2(ENV_WINDOW, ENV_ICON_FILENAME);
		LOGI_X("%s=\"%s\" overrides compiled value \"%s\"",
		       Name, window.iconFilename.data(), old_.window.iconFilename.data());
	}

	// ----------------------------------------------------------------
	// ----- Graphics -----

	if (graphics.vsync != old_.graphics.vsync)
	{
		constexpr const char Name[] = ENV2(ENV_GRAPHICS, ENV_VSYNC);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, graphics.vsync, old_.graphics.vsync);
	}

	if (graphics.frameLimit != old_.graphics.frameLimit)
	{
		constexpr const char Name[] = ENV2(ENV_GRAPHICS, ENV_FRAME_LIMIT);
		LOGI_X("%s=%u overrides compiled value %u",
		       Name, graphics.frameLimit, old_.graphics.frameLimit);
	}

	// ----------------------------------------------------------------
	// ----- Graphics.OpenGL -----

	if (graphics.opengl.debugContext != old_.graphics.opengl.debugContext)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_DEBUG_CONTEXT);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, graphics.opengl.debugContext, old_.graphics.opengl.debugContext);
	}

	if (graphics.opengl.useBufferMapping != old_.graphics.opengl.useBufferMapping)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_BUFFER_MAPPING);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, graphics.opengl.useBufferMapping, old_.graphics.opengl.useBufferMapping);
	}

	// ----------------------------------------------------------------

	if (graphics.opengl.vboSize != old_.graphics.opengl.vboSize)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_VBO_SIZE);
		LOGI_X("%s=%lu overrides compiled value %lu",
		       Name, graphics.opengl.vboSize, old_.graphics.opengl.vboSize);
	}

	if (graphics.opengl.iboSize != old_.graphics.opengl.iboSize)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_IBO_SIZE);
		LOGI_X("%s=%lu overrides compiled value %lu",
		       Name, graphics.opengl.iboSize, old_.graphics.opengl.iboSize);
	}

	if (graphics.opengl.vaoPoolSize != old_.graphics.opengl.vaoPoolSize)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_VAO_POOL_SIZE);
		LOGI_X("%s=%u overrides compiled value %u",
		       Name, graphics.opengl.vaoPoolSize, old_.graphics.opengl.vaoPoolSize);
	}

	if (graphics.opengl.renderCommandPoolSize != old_.graphics.opengl.renderCommandPoolSize)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_RENDER_COMMAND_POOL_SIZE);
		LOGI_X("%s=%u overrides compiled value %u",
		       Name, graphics.opengl.renderCommandPoolSize, old_.graphics.opengl.renderCommandPoolSize);
	}

	// ----------------------------------------------------------------

	if (graphics.opengl.deferShaderQueries != old_.graphics.opengl.deferShaderQueries)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_DEFER_SHADER_QUERIES);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, graphics.opengl.deferShaderQueries, old_.graphics.opengl.deferShaderQueries);
	}

	if (graphics.opengl.fixedBatchSize != old_.graphics.opengl.fixedBatchSize)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_FIXED_BATCH_SIZE);
		LOGI_X("%s=%u overrides compiled value %u",
		       Name, graphics.opengl.fixedBatchSize, old_.graphics.opengl.fixedBatchSize);
	}

	if (graphics.opengl.useBinaryShaderCache != old_.graphics.opengl.useBinaryShaderCache)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_BINARY_SHADER_CACHE);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, graphics.opengl.useBinaryShaderCache, old_.graphics.opengl.useBinaryShaderCache);
	}

	if (graphics.opengl.shaderCacheDirname != old_.graphics.opengl.shaderCacheDirname)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_SHADER_CACHE_DIRNAME);
		LOGI_X("%s=\"%s\" overrides compiled value \"%s\"",
		       Name, graphics.opengl.shaderCacheDirname.data(), old_.graphics.opengl.shaderCacheDirname.data());
	}

	if (graphics.opengl.compileBatchedShadersTwice != old_.graphics.opengl.compileBatchedShadersTwice)
	{
		constexpr const char Name[] = ENV3(ENV_GRAPHICS, ENV_OPENGL, ENV_COMPILE_BATCHED_SHADERS_TWICE);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, graphics.opengl.compileBatchedShadersTwice, old_.graphics.opengl.compileBatchedShadersTwice);
	}

	// ----------------------------------------------------------------
	// ----- Audio -----

	if (audio.enabled != old_.audio.enabled)
	{
		constexpr const char Name[] = ENV2(ENV_AUDIO, ENV_ENABLED);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, audio.enabled, old_.audio.enabled);
	}

	if (audio.frequency != old_.audio.frequency)
	{
		constexpr const char Name[] = ENV2(ENV_AUDIO, ENV_FREQUENCY);
		LOGI_X("%s=%u overrides compiled value %u",
		       Name, audio.frequency, old_.audio.frequency);
	}

	if (audio.monoSources != old_.audio.monoSources)
	{
		constexpr const char Name[] = ENV2(ENV_AUDIO, ENV_MONO_SOURCES);
		LOGI_X("%s=%u overrides compiled value %u",
		       Name, audio.monoSources, old_.audio.monoSources);
	}

	if (audio.stereoSources != old_.audio.stereoSources)
	{
		constexpr const char Name[] = ENV2(ENV_AUDIO, ENV_STEREO_SOURCES);
		LOGI_X("%s=%u overrides compiled value %u",
		       Name, audio.stereoSources, old_.audio.stereoSources);
	}

	// ----------------------------------------------------------------
	// ----- Job System -----

	if (jobSystem.enabled != old_.jobSystem.enabled)
	{
		constexpr const char Name[] = ENV2(ENV_JOBSYSTEM, ENV_ENABLED);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, jobSystem.enabled, old_.jobSystem.enabled);
	}

	if (jobSystem.numThreads != old_.jobSystem.numThreads)
	{
		constexpr const char Name[] = ENV2(ENV_JOBSYSTEM, ENV_NUM_THREADS);
		LOGI_X("%s=%u overrides compiled value %u",
		       Name, jobSystem.numThreads, old_.jobSystem.numThreads);
	}

	// ----------------------------------------------------------------
	// ----- Features -----

	if (features.scenegraph != old_.features.scenegraph)
	{
		constexpr const char Name[] = ENV2(ENV_FEATURES, ENV_SCENEGRAPH);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, features.scenegraph, old_.features.scenegraph);
	}

	if (features.debugOverlay != old_.features.debugOverlay)
	{
		constexpr const char Name[] = ENV2(ENV_FEATURES, ENV_DEBUG_OVERLAY);
		LOGI_X("%s=%d overrides compiled value %d",
		       Name, features.debugOverlay, old_.features.debugOverlay);
	}

	// ----------------------------------------------------------------

	if (fs::baseName(dataPath().data()) != fs::baseName(old_.dataPath.data()))
	{
		constexpr const char Name[] = ENV_PREFIX ENV_DATA_PATH;
		LOGI_X("%s=\"%s\" overrides compiled value \"%s\"",
		       Name, dataPath().data(), old_.dataPath.data());
	}
}

}
