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

	const char *EnvVariablePrefix = "NCINE_APPCFG_";

	const char *EnvVariableLogFile = "LOG_FILE";
	const char *EnvVariableConsoleLogLevel = "CONSOLE_LOG_LEVEL";
	const char *EnvVariableFileLogLevel = "FILE_LOG_LEVEL";
	const char *EnvVariableFrameTimerLogInterval = "FRAME_TIMER_LOG_INTERVAL";

	const char *EnvVariableResolution = "RESOLUTION";
	const char *EnvVariableRefreshRate = "REFRESH_RATE";
	const char *EnvVariableWindowPosition = "WINDOW_POSITION";
	const char *EnvVariableFullScreen = "FULLSCREEN";
	const char *EnvVariableResizable = "RESIZABLE";
	const char *EnvVariableWindowScaling = "WINDOW_SCALING";
	const char *EnvVariableFrameLimit = "FRAME_LIMIT";

	const char *EnvVariableWindowTitle = "WINDOW_TITLE";
	const char *EnvVariableWindowIconFilename = "WINDOW_ICON_FILENAME";

	const char *EnvVariableBufferMapping = "BUFFER_MAPPING";
	const char *EnvVariableDeferShaderQueries = "DEFER_SHADER_QUERIES";
	const char *EnvVariableFixedBatchSize = "FIXED_BATCH_SIZE";
	const char *EnvVariableBinaryShaderCache = "BINARY_SHADER_CACHE";
	const char *EnvVariableShaderCacheDirname = "SHADER_CACHE_DIRNAME";
	const char *EnvVariableCompileBatchedShadersTwice = "COMPILE_BATCHED_SHADERS_TWICE";

	const char *EnvVariableVboSize = "VBO_SIZE";
	const char *EnvVariableIboSize = "IBO_SIZE";
	const char *EnvVariableVaoPoolSize = "VAO_POOL_SIZE";
	const char *EnvVariableRenderCommandPoolSize = "RENDER_COMMAND_POOL_SIZE";

	const char *EnvVariableOutputAudioFrequency = "OUTPUT_AUDIO_FREQUENCY";
	const char *EnvVariableMonoAudioSources = "MONO_AUDIO_SOURCES";
	const char *EnvVariableStereoAudioSources = "STEREO_AUDIO_SOURCES";

	const char *EnvVariableDebugOverlay = "DEBUG_OVERLAY";
	const char *EnvVariableAudio = "AUDIO";
	const char *EnvVariableJobSystem = "JOBSYSTEM";
	const char *EnvVariableNumThreads = "NUM_THREADS";
	const char *EnvVariableScenegraph = "SCENEGRAPH";
	const char *EnvVariableVSync = "VSYNC";
	const char *EnvVariableGlDebugContext = "GL_DEBUG_CONTEXT";
	const char *EnvVariableConsoleColors = "CONSOLE_COLORS";

	const char *EnvVariableDataPath = "DATA_PATH";

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

AppConfiguration::OldValues::OldValues(const AppConfiguration &appCfg)
    : logFile(appCfg.logFile.capacity()),
      consoleLogLevel(appCfg.consoleLogLevel),
      fileLogLevel(appCfg.fileLogLevel),
      frameTimerLogInterval(appCfg.frameTimerLogInterval),

      resolution(appCfg.resolution),
      refreshRate(appCfg.refreshRate),
      windowPosition(appCfg.windowPosition),
      fullScreen(appCfg.fullScreen),
      resizable(appCfg.resizable),
      windowScaling(appCfg.windowScaling),
      frameLimit(appCfg.frameLimit),

      windowTitle(appCfg.windowTitle.capacity()),
      windowIconFilename(appCfg.windowIconFilename.capacity()),

      useBufferMapping(appCfg.useBufferMapping),
      deferShaderQueries(appCfg.deferShaderQueries),
      fixedBatchSize(appCfg.fixedBatchSize),
      useBinaryShaderCache(appCfg.useBinaryShaderCache),
      shaderCacheDirname(appCfg.shaderCacheDirname.capacity()),
      compileBatchedShadersTwice(appCfg.compileBatchedShadersTwice),

      vboSize(appCfg.vboSize),
      iboSize(appCfg.iboSize),
      vaoPoolSize(appCfg.vaoPoolSize),
      renderCommandPoolSize(appCfg.renderCommandPoolSize),

      outputAudioFrequency(appCfg.outputAudioFrequency),
      monoAudioSources(appCfg.monoAudioSources),
      stereoAudioSources(appCfg.stereoAudioSources),

      withDebugOverlay(appCfg.withDebugOverlay),
      withAudio(appCfg.withAudio),
      withJobSystem(appCfg.withJobSystem),
      numThreads(appCfg.numThreads),
      withScenegraph(appCfg.withScenegraph),
      withVSync(appCfg.withVSync),
      withGlDebugContext(appCfg.withGlDebugContext),
      withConsoleColors(appCfg.withConsoleColors),

      dataPath(appCfg.dataPath().capacity())
{
}

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
      withJobSystem(true),
      numThreads(0),
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
      argv_(nullptr),

      old(*this)
{
	logFile = "ncine_log.txt";
	windowTitle = "nCine";
	windowIconFilename = "icons/icon48.png";
	shaderCacheDirname = "nCineShaderCache";

#if defined(WITH_CRASHPAD)
	fileLogLevel = ILogger::LogLevel::DEBUG;
#endif

#if defined(__ANDROID__)
	dataPath() = AssetFile::Prefix;
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

#if !defined(WITH_SCENEGRAPH)
	withScenegraph = false;
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

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AppConfiguration::logFields() const
{
#if !defined(WITH_OPENGLES) && !defined(__EMSCRIPTEN__)
	LOGD_X("OpenGL Core: %s", glCoreProfile_ ? "true" : "false");
	LOGD_X("OpenGL Forward: %s", glForwardCompatible_ ? "true" : "false");
#endif
	LOGD_X("GL Major.Minor: %d.%d", glMajorVersion_, glMinorVersion_);

	LOGD_X("Data Path: \"%s\"", dataPath().data());
	LOGD_X("Log File: \"%s\"", logFile.data());
	LOGD_X("Console Log Level: %d", static_cast<int>(consoleLogLevel));
	LOGD_X("File Log Level: %d", static_cast<int>(fileLogLevel));
	LOGD_X("Frametimer Log Interval: %f s", frameTimerLogInterval);
	LOGD_X("Profile Text Update Time: %f s", profileTextUpdateTime());

	LOGD_X("Resolution: %d x %d", resolution.x, resolution.y);
	LOGD_X("Refresh Rate: %f Hz", refreshRate);

	nctl::StaticString<256> auxString;
	auxString.assign("Window Position: ");
	if (windowPosition.x == WindowPositionIgnore)
		auxString.append("Ignore x ");
	else
		auxString.formatAppend("%d x ", windowPosition.x);
	if (windowPosition.y == WindowPositionIgnore)
		auxString.append("Ignore");
	else
		auxString.formatAppend("%d", windowPosition.y);
	LOGD_X("%s", auxString.data());

	LOGD_X("Full Screen: %s", fullScreen ? "true" : "false");
	LOGD_X("Resizable: %s", resizable ? "true" : "false");
	LOGD_X("Window Scaling: %s", windowScaling ? "true" : "false");
	LOGD_X("Frame Limit: %u", frameLimit);

	LOGD_X("Window title: \"%s\"", windowTitle.data());
	LOGD_X("Window icon: \"%s\"", windowIconFilename.data());

	LOGD_X("Buffer Mapping: %s", useBufferMapping ? "true" : "false");
	LOGD_X("Defer Shader Queries: %s", deferShaderQueries ? "true" : "false");
#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
	LOGD_X("Fixed Batch Size: %u", fixedBatchSize);
#endif
	LOGD_X("Binary Shader Cache: %s", useBinaryShaderCache ? "true" : "false");
	LOGD_X("Shader Cache Directory Name: \"%s\"", shaderCacheDirname.data());
	LOGD_X("Compile Batched Shaders Twice: %s", compileBatchedShadersTwice ? "true" : "false");

	LOGD_X("VBO Size: %lu bytes", vboSize);
	LOGD_X("IBO Size: %lu bytes", iboSize);
	LOGD_X("VAO Pool Size: %u", vaoPoolSize);
	LOGD_X("RenderCommand Pool Size: %u", renderCommandPoolSize);

	LOGD_X("Output Audio Frequency: %u Hz", outputAudioFrequency);
	LOGD_X("Mono Audio Sources: %u", monoAudioSources);
	LOGD_X("Stereo Audio Sources: %u", stereoAudioSources);

	LOGD_X("Debug Overlay: %s", withDebugOverlay ? "true" : "false");
	LOGD_X("Audio: %s", withAudio ? "true" : "false");
	LOGD_X("Job System: %s", withJobSystem ? "true" : "false");
	LOGD_X("Number of Threads: %u", numThreads);
	LOGD_X("Scenegraph: %s", withScenegraph ? "true" : "false");
	LOGD_X("VSync: %s", withVSync ? "true" : "false");
	LOGD_X("GL Debug Context: %s", withGlDebugContext ? "true" : "false");
	LOGD_X("Console Colors: %s", withConsoleColors ? "true" : "false");

	for (int i = 0; i < argc_; i++)
		LOGD_X("argv[%u]: \"%s\"", i, argv_[i]);
}

void AppConfiguration::readEnvVariables()
{
	nctl::StaticString<64> varName;

	// NCINE_APPCFG_LOG_FILE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableLogFile);
	old.logFile = logFile;
	readStringEnvVar(varName.data(), logFile);

	// NCINE_APPCFG_CONSOLE_LOG_LEVEL
	varName = EnvVariablePrefix;
	varName.append(EnvVariableConsoleLogLevel);
	old.consoleLogLevel = consoleLogLevel;
	consoleLogLevel = readLogLevelEnvVar(varName.data(), consoleLogLevel);

	// NCINE_APPCFG_FILE_LOG_LEVEL
	varName = EnvVariablePrefix;
	varName.append(EnvVariableFileLogLevel);
	old.fileLogLevel = fileLogLevel;
	fileLogLevel = readLogLevelEnvVar(varName.data(), fileLogLevel);

	// NCINE_APPCFG_FRAME_TIMER_LOG_INTERVAL
	varName = EnvVariablePrefix;
	varName.append(EnvVariableFrameTimerLogInterval);
	old.frameTimerLogInterval = frameTimerLogInterval;
	frameTimerLogInterval = readFloatEnvVar(varName.data(), frameTimerLogInterval);
	if (frameTimerLogInterval < 0.0f)
		frameTimerLogInterval = old.frameTimerLogInterval;

	// ----------------------------------------------------------------

	// NCINE_APPCFG_RESOLUTION
	varName = EnvVariablePrefix;
	varName.append(EnvVariableResolution);
	old.resolution = resolution;
	resolution = readVector2iEnvVar(varName.data(), resolution);

	// NCINE_APPCFG_REFRESH_RATE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableRefreshRate);
	old.refreshRate = refreshRate;
	refreshRate = readFloatEnvVar(varName.data(), refreshRate);
	if (refreshRate < 0.0f)
		refreshRate = old.refreshRate;

	// NCINE_APPCFG_WINDOW_POSITION
	varName = EnvVariablePrefix;
	varName.append(EnvVariableWindowPosition);
	old.windowPosition = windowPosition;
	windowPosition = readVector2iEnvVar(varName.data(), windowPosition);

	// NCINE_APPCFG_FULLSCREEN
	varName = EnvVariablePrefix;
	varName.append(EnvVariableFullScreen);
	old.fullScreen = fullScreen;
	fullScreen = readBoolEnvVar(varName.data(), fullScreen);

	// NCINE_APPCFG_RESIZABLE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableResizable);
	old.resizable = resizable;
	resizable = readBoolEnvVar(varName.data(), resizable);

	// NCINE_APPCFG_WINDOW_SCALING
	varName = EnvVariablePrefix;
	varName.append(EnvVariableWindowScaling);
	old.windowScaling = windowScaling;
	windowScaling = readBoolEnvVar(varName.data(), windowScaling);

	// NCINE_APPCFG_FRAME_LIMIT
	varName = EnvVariablePrefix;
	varName.append(EnvVariableFrameLimit);
	old.frameLimit = frameLimit;
	frameLimit = readUintEnvVar(varName.data(), frameLimit);

	// ----------------------------------------------------------------

	// NCINE_APPCFG_WINDOW_TITLE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableWindowTitle);
	old.windowTitle = windowTitle;
	readStringEnvVar(varName.data(), windowTitle);

	// NCINE_APPCFG_WINDOW_ICON_FILENAME
	varName = EnvVariablePrefix;
	varName.append(EnvVariableWindowIconFilename);
	old.windowIconFilename = windowIconFilename;
	readStringEnvVar(varName.data(), windowIconFilename);

	// ----------------------------------------------------------------

	// NCINE_APPCFG_BUFFER_MAPPING
	varName = EnvVariablePrefix;
	varName.append(EnvVariableBufferMapping);
	old.useBufferMapping = useBufferMapping;
	useBufferMapping = readBoolEnvVar(varName.data(), useBufferMapping);

	// NCINE_APPCFG_DEFER_SHADER_QUERIES
	varName = EnvVariablePrefix;
	varName.append(EnvVariableDeferShaderQueries);
	old.deferShaderQueries = deferShaderQueries;
	deferShaderQueries = readBoolEnvVar(varName.data(), deferShaderQueries);

	// NCINE_APPCFG_FIXED_BATCH_SIZE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableFixedBatchSize);
	old.fixedBatchSize = fixedBatchSize;
	fixedBatchSize = readUintEnvVar(varName.data(), fixedBatchSize);

	// NCINE_APPCFG_BINARY_SHADER_CACHE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableBinaryShaderCache);
	old.useBinaryShaderCache = useBinaryShaderCache;
	useBinaryShaderCache = readBoolEnvVar(varName.data(), useBinaryShaderCache);

	// NCINE_APPCFG_SHADER_CACHE_DIRNAME
	varName = EnvVariablePrefix;
	varName.append(EnvVariableShaderCacheDirname);
	old.shaderCacheDirname = shaderCacheDirname;
	readStringEnvVar(varName.data(), shaderCacheDirname);

	// NCINE_APPCFG_COMPILE_BATCHED_SHADERS_TWICE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableCompileBatchedShadersTwice);
	old.compileBatchedShadersTwice = compileBatchedShadersTwice;
	compileBatchedShadersTwice = readBoolEnvVar(varName.data(), compileBatchedShadersTwice);

	// ----------------------------------------------------------------

	// NCINE_APPCFG_VBO_SIZE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableVboSize);
	old.vboSize = vboSize;
	vboSize = readUlongEnvVar(varName.data(), vboSize);
	if (vboSize == 0)
		vboSize = old.vboSize;

	// NCINE_APPCFG_IBO_SIZE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableIboSize);
	old.iboSize = iboSize;
	iboSize = readUlongEnvVar(varName.data(), iboSize);
	if (iboSize == 0)
		iboSize = old.iboSize;

	// NCINE_APPCFG_VAO_POOL_SIZE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableVaoPoolSize);
	old.vaoPoolSize = vaoPoolSize;
	vaoPoolSize = readUintEnvVar(varName.data(), vaoPoolSize);
	if (iboSize == 0)
		iboSize = old.iboSize;

	// NCINE_APPCFG_RENDER_COMMAND_POOL_SIZE
	varName = EnvVariablePrefix;
	varName.append(EnvVariableRenderCommandPoolSize);
	old.renderCommandPoolSize = renderCommandPoolSize;
	renderCommandPoolSize = readUintEnvVar(varName.data(), renderCommandPoolSize);

	// ----------------------------------------------------------------

	// NCINE_APPCFG_OUTPUT_AUDIO_FREQUENCY
	varName = EnvVariablePrefix;
	varName.append(EnvVariableOutputAudioFrequency);
	old.outputAudioFrequency = outputAudioFrequency;
	outputAudioFrequency = readUintEnvVar(varName.data(), outputAudioFrequency);

	// NCINE_APPCFG_MONO_AUDIO_SOURCES
	varName = EnvVariablePrefix;
	varName.append(EnvVariableMonoAudioSources);
	old.monoAudioSources = monoAudioSources;
	monoAudioSources = readUintEnvVar(varName.data(), monoAudioSources);

	// NCINE_APPCFG_STEREO_AUDIO_SOURCES
	varName = EnvVariablePrefix;
	varName.append(EnvVariableStereoAudioSources);
	old.stereoAudioSources = stereoAudioSources;
	stereoAudioSources = readUintEnvVar(varName.data(), stereoAudioSources);

	// ----------------------------------------------------------------

	// NCINE_APPCFG_DEBUG_OVERLAY
	varName = EnvVariablePrefix;
	varName.append(EnvVariableDebugOverlay);
	old.withDebugOverlay = withDebugOverlay;
	withDebugOverlay = readBoolEnvVar(varName.data(), withDebugOverlay);

	// NCINE_APPCFG_AUDIO
	varName = EnvVariablePrefix;
	varName.append(EnvVariableAudio);
	old.withAudio = withAudio;
	withAudio = readBoolEnvVar(varName.data(), withAudio);

	// NCINE_APPCFG_JOBSYSTEM
	varName = EnvVariablePrefix;
	varName.append(EnvVariableJobSystem);
	old.withJobSystem = withJobSystem;
	withJobSystem = readBoolEnvVar(varName.data(), withJobSystem);

	// NCINE_APPCFG_NUM_THREADS
	varName = EnvVariablePrefix;
	varName.append(EnvVariableNumThreads);
	old.numThreads = numThreads;
	numThreads = readUintEnvVar(varName.data(), numThreads);

	// NCINE_APPCFG_SCENEGRAPH
	varName = EnvVariablePrefix;
	varName.append(EnvVariableScenegraph);
	old.withScenegraph = withScenegraph;
	withScenegraph = readBoolEnvVar(varName.data(), withScenegraph);

	// NCINE_APPCFG_VSYNC
	varName = EnvVariablePrefix;
	varName.append(EnvVariableVSync);
	old.withVSync = withVSync;
	withVSync = readBoolEnvVar(varName.data(), withVSync);

	// NCINE_APPCFG_GL_DEBUG_CONTEXT
	varName = EnvVariablePrefix;
	varName.append(EnvVariableGlDebugContext);
	old.withGlDebugContext = withGlDebugContext;
	withGlDebugContext = readBoolEnvVar(varName.data(), withGlDebugContext);

	// NCINE_APPCFG_CONSOLE_COLORS
	varName = EnvVariablePrefix;
	varName.append(EnvVariableConsoleColors);
	old.withConsoleColors = withConsoleColors;
	withConsoleColors = readBoolEnvVar(varName.data(), withConsoleColors);

	// ----------------------------------------------------------------

	// NCINE_APPCFG_DATA_PATH
	varName = EnvVariablePrefix;
	varName.append(EnvVariableDataPath);
	old.dataPath = dataPath();
	readStringEnvVar(varName.data(), dataPath());
}

void AppConfiguration::logEnvVariables() const
{
	if (logFile != old.logFile)
	{
		LOGI_X("%s%s=\"%s\" overrides compiled value \"%s\"",
		       EnvVariablePrefix, EnvVariableLogFile, logFile.data(), old.logFile.data());
	}

	if (consoleLogLevel != old.consoleLogLevel)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableConsoleLogLevel, consoleLogLevel, old.consoleLogLevel);
	}

	if (fileLogLevel != old.fileLogLevel)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableFileLogLevel, fileLogLevel, old.fileLogLevel);
	}

	if (frameTimerLogInterval != old.frameTimerLogInterval)
	{
		LOGI_X("%s%s=%f overrides compiled value %f",
		       EnvVariablePrefix, EnvVariableFrameTimerLogInterval, frameTimerLogInterval, old.frameTimerLogInterval);
	}

	// ----------------------------------------------------------------

	if (resolution != old.resolution)
	{
		LOGI_X("%s%s=%dx%d overrides compiled value %dx%d",
		       EnvVariablePrefix, EnvVariableResolution, resolution.x, resolution.y, old.resolution.x, old.resolution.y);
	}

	if (refreshRate != old.refreshRate)
	{
		LOGI_X("%s%s=%f overrides compiled value %f",
		       EnvVariablePrefix, EnvVariableRefreshRate, refreshRate, old.refreshRate);
	}

	if (windowPosition != old.windowPosition)
	{
		LOGI_X("%s%s=%dx%d overrides compiled value %dx%d",
		       EnvVariablePrefix, EnvVariableWindowPosition, windowPosition.x, windowPosition.y, old.windowPosition.x, old.windowPosition.y);
	}

	if (fullScreen != old.fullScreen)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableFullScreen, fullScreen, old.fullScreen);
	}

	if (resizable != old.resizable)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableResizable, resizable, old.resizable);
	}

	if (windowScaling != old.windowScaling)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableWindowScaling, windowScaling, old.windowScaling);
	}

	if (frameLimit != old.frameLimit)
	{
		LOGI_X("%s%s=%u overrides compiled value %u",
		       EnvVariablePrefix, EnvVariableFrameLimit, frameLimit, old.frameLimit);
	}

	// ----------------------------------------------------------------

	if (windowTitle != old.windowTitle)
	{
		LOGI_X("%s%s=\"%s\" overrides compiled value \"%s\"",
		       EnvVariablePrefix, EnvVariableWindowTitle, windowTitle.data(), old.windowTitle.data());
	}

	if (windowIconFilename != old.windowIconFilename)
	{
		LOGI_X("%s%s=\"%s\" overrides compiled value \"%s\"",
		       EnvVariablePrefix, EnvVariableWindowIconFilename, windowIconFilename.data(), old.windowIconFilename.data());
	}

	// ----------------------------------------------------------------

	if (useBufferMapping != old.useBufferMapping)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableBufferMapping, useBufferMapping, old.useBufferMapping);
	}

	if (deferShaderQueries != old.deferShaderQueries)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableDeferShaderQueries, deferShaderQueries, old.deferShaderQueries);
	}

	if (fixedBatchSize != old.fixedBatchSize)
	{
		LOGI_X("%s%s=%u overrides compiled value %u",
		       EnvVariablePrefix, EnvVariableFixedBatchSize, fixedBatchSize, old.fixedBatchSize);
	}

	if (useBinaryShaderCache != old.useBinaryShaderCache)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableBinaryShaderCache, useBinaryShaderCache, old.useBinaryShaderCache);
	}

	if (shaderCacheDirname != old.shaderCacheDirname)
	{
		LOGI_X("%s%s=\"%s\" overrides compiled value \"%s\"",
		       EnvVariablePrefix, EnvVariableShaderCacheDirname, shaderCacheDirname.data(), old.shaderCacheDirname.data());
	}

	if (compileBatchedShadersTwice != old.compileBatchedShadersTwice)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableCompileBatchedShadersTwice, compileBatchedShadersTwice, old.compileBatchedShadersTwice);
	}

	// ----------------------------------------------------------------

	if (vboSize != old.vboSize)
	{
		LOGI_X("%s%s=%lu overrides compiled value %lu",
		       EnvVariablePrefix, EnvVariableVboSize, vboSize, old.vboSize);
	}

	if (iboSize != old.iboSize)
	{
		LOGI_X("%s%s=%lu overrides compiled value %lu",
		       EnvVariablePrefix, EnvVariableIboSize, iboSize, old.iboSize);
	}

	if (vaoPoolSize != old.vaoPoolSize)
	{
		LOGI_X("%s%s=%u overrides compiled value %u",
		       EnvVariablePrefix, EnvVariableVaoPoolSize, vaoPoolSize, old.vaoPoolSize);
	}

	if (renderCommandPoolSize != old.renderCommandPoolSize)
	{
		LOGI_X("%s%s=%u overrides compiled value %u",
		       EnvVariablePrefix, EnvVariableRenderCommandPoolSize, renderCommandPoolSize, old.renderCommandPoolSize);
	}

	// ----------------------------------------------------------------

	if (outputAudioFrequency != old.outputAudioFrequency)
	{
		LOGI_X("%s%s=%u overrides compiled value %u",
		       EnvVariablePrefix, EnvVariableOutputAudioFrequency, outputAudioFrequency, old.outputAudioFrequency);
	}

	if (monoAudioSources != old.monoAudioSources)
	{
		LOGI_X("%s%s=%u overrides compiled value %u",
		       EnvVariablePrefix, EnvVariableMonoAudioSources, monoAudioSources, old.monoAudioSources);
	}

	if (stereoAudioSources != old.stereoAudioSources)
	{
		LOGI_X("%s%s=%u overrides compiled value %u",
		       EnvVariablePrefix, EnvVariableStereoAudioSources, stereoAudioSources, old.stereoAudioSources);
	}

	// ----------------------------------------------------------------

	if (withDebugOverlay != old.withDebugOverlay)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableDebugOverlay, withDebugOverlay, old.withDebugOverlay);
	}

	if (withAudio != old.withAudio)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableAudio, withAudio, old.withAudio);
	}

	if (withJobSystem != old.withJobSystem)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableJobSystem, withJobSystem, old.withJobSystem);
	}

	if (numThreads != old.numThreads)
	{
		LOGI_X("%s%s=%u overrides compiled value %u",
		       EnvVariablePrefix, EnvVariableNumThreads, numThreads, old.numThreads);
	}

	if (withScenegraph != old.withScenegraph)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableScenegraph, withScenegraph, old.withScenegraph);
	}

	if (withVSync != old.withVSync)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableVSync, withVSync, old.withVSync);
	}

	if (withGlDebugContext != old.withGlDebugContext)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableGlDebugContext, withGlDebugContext, old.withGlDebugContext);
	}

	if (withConsoleColors != old.withConsoleColors)
	{
		LOGI_X("%s%s=%d overrides compiled value %d",
		       EnvVariablePrefix, EnvVariableConsoleColors, withConsoleColors, old.withConsoleColors);
	}

	// ----------------------------------------------------------------

	if (fs::baseName(dataPath().data()) != fs::baseName(old.dataPath.data()))
	{
		LOGI_X("%s%s=\"%s\" overrides compiled value \"%s\"",
		       EnvVariablePrefix, EnvVariableDataPath, dataPath().data(), old.dataPath.data());
	}
}

}
