#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaAppConfiguration.h"
#include "LuaUtils.h"
#include "LuaVector2Utils.h"
#include "AppConfiguration.h"

namespace ncine {

namespace LuaNames {
namespace AppConfiguration {
	static const char *WindowPositionIgnore = "window_position_ignore";

	static const char *dataPath = "data_path";
	static const char *logFile = "log_file";
	static const char *consoleLogLevel = "console_log_level";
	static const char *fileLogLevel = "file_log_level";
	static const char *frameTimerLogInterval = "log_interval";

	static const char *resolution = "resolution";
	static const char *refreshRate = "refresh_rate";
	static const char *windowPosition = "window_position";
	static const char *fullScreen = "full_screen";
	static const char *resizable = "resizable";
	static const char *windowScaling = "window_scaling";
	static const char *frameLimit = "frame_limit";

	static const char *windowTitle = "window_title";
	static const char *windowIconFilename = "window_icon";

	static const char *useBufferMapping = "buffer_mapping";
	static const char *deferShaderQueries = "defer_shader_queries";
	static const char *fixedBatchSize = "fixed_batch_size";
	static const char *useBinaryShaderCache = "binary_shader_cache";
	static const char *shaderCacheDirname = "shader_cache_dirname";
	static const char *compileBatchedShadersTwice = "compile_batched_shaders_twice";
	static const char *vboSize = "vbo_size";
	static const char *iboSize = "ibo_size";
	static const char *vaoPoolSize = "vao_pool_size";
	static const char *renderCommandPoolSize = "rendercommand_pool_size";

	static const char *outputAudioFrequency = "output_audio_frequency";
	static const char *monoAudioSources = "mono_audio_sources";
	static const char *stereoAudioSources = "stereo_audio_sources";

	static const char *withDebugOverlay = "debug_overlay";
	static const char *withAudio = "audio";
	static const char *withThreads = "threads";
	static const char *withScenegraph = "scenegraph";
	static const char *withVSync = "vsync";
	static const char *withGlDebugContext = "gl_debug_context";
	static const char *withConsoleColors = "console_colors";

	static const char *glCoreProfile = "opengl_core_profile";
	static const char *glForwardCompatible = "opengl_forward_compatible";
	static const char *glMajorVersion = "opengl_major_version";
	static const char *glMinorVersion = "opengl_minor_version";
	static const char *profileTextUpdateTime = "profile_text_update_time";
	static const char *argc = "argc";
	static const char *argv = "argv";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAppConfiguration::exposeConstants(lua_State *L)
{
	lua_pushinteger(L, AppConfiguration::WindowPositionIgnore);
	lua_setfield(L, -2, LuaNames::AppConfiguration::WindowPositionIgnore);
}

void LuaAppConfiguration::push(lua_State *L, const AppConfiguration &appCfg)
{
	lua_createtable(L, 0, 40);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::dataPath, appCfg.dataPath().data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::logFile, appCfg.logFile.data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::consoleLogLevel, static_cast<int64_t>(appCfg.consoleLogLevel));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::fileLogLevel, static_cast<int64_t>(appCfg.fileLogLevel));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::frameTimerLogInterval, appCfg.frameTimerLogInterval);

	LuaVector2iUtils::pushField(L, LuaNames::AppConfiguration::resolution, appCfg.resolution);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::refreshRate, appCfg.refreshRate);
	LuaVector2iUtils::pushField(L, LuaNames::AppConfiguration::windowPosition, appCfg.windowPosition);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::fullScreen, appCfg.fullScreen);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::resizable, appCfg.resizable);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::windowScaling, appCfg.windowScaling);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::frameLimit, appCfg.frameLimit);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::windowTitle, appCfg.windowTitle.data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::windowIconFilename, appCfg.windowIconFilename.data());

	LuaUtils::pushField(L, LuaNames::AppConfiguration::useBufferMapping, appCfg.useBufferMapping);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::deferShaderQueries, appCfg.deferShaderQueries);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::fixedBatchSize, appCfg.fixedBatchSize);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::useBinaryShaderCache, appCfg.useBinaryShaderCache);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::shaderCacheDirname, appCfg.shaderCacheDirname.data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::compileBatchedShadersTwice, appCfg.compileBatchedShadersTwice);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::vboSize, static_cast<int64_t>(appCfg.vboSize));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::iboSize, static_cast<int64_t>(appCfg.iboSize));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::vaoPoolSize, appCfg.vaoPoolSize);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::renderCommandPoolSize, appCfg.renderCommandPoolSize);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::outputAudioFrequency, appCfg.outputAudioFrequency);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::monoAudioSources, appCfg.monoAudioSources);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::stereoAudioSources, appCfg.stereoAudioSources);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::withDebugOverlay, appCfg.withDebugOverlay);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withAudio, appCfg.withAudio);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withThreads, appCfg.withThreads);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withScenegraph, appCfg.withScenegraph);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withVSync, appCfg.withVSync);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withGlDebugContext, appCfg.withGlDebugContext);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withConsoleColors, appCfg.withConsoleColors);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::glCoreProfile, appCfg.glCoreProfile());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::glForwardCompatible, appCfg.glForwardCompatible());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::glMajorVersion, appCfg.glMajorVersion());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::glMinorVersion, appCfg.glMinorVersion());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::profileTextUpdateTime, appCfg.profileTextUpdateTime());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::argc, appCfg.argc());

	if (appCfg.argc() > 0)
	{
		lua_createtable(L, appCfg.argc(), 0);
		for (int i = 0; i < appCfg.argc(); i++)
		{
			lua_pushstring(L, appCfg.argv(i));
			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
		lua_setfield(L, -2, LuaNames::AppConfiguration::argv);
	}
}

void LuaAppConfiguration::retrieveAndSet(lua_State *L, AppConfiguration &appCfg)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return;
	}

	const char *dataPath = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::dataPath);
	appCfg.dataPath() = dataPath;
	const char *logFile = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::logFile);
	appCfg.logFile = logFile;
	const ILogger::LogLevel consoleLogLevel = static_cast<ILogger::LogLevel>(LuaUtils::retrieveField<int64_t>(L, -1, LuaNames::AppConfiguration::consoleLogLevel));
	appCfg.consoleLogLevel = consoleLogLevel;
	const ILogger::LogLevel fileLogLevel = static_cast<ILogger::LogLevel>(LuaUtils::retrieveField<int64_t>(L, -1, LuaNames::AppConfiguration::fileLogLevel));
	appCfg.fileLogLevel = fileLogLevel;
	const float logInterval = LuaUtils::retrieveField<float>(L, -1, LuaNames::AppConfiguration::frameTimerLogInterval);
	appCfg.frameTimerLogInterval = logInterval;

	const Vector2i resolution = LuaVector2iUtils::retrieveTableField(L, -1, LuaNames::AppConfiguration::resolution);
	appCfg.resolution = resolution;
	const float refreshRate = LuaUtils::retrieveField<float>(L, -1, LuaNames::AppConfiguration::refreshRate);
	appCfg.refreshRate = refreshRate;
	const Vector2i windowPosition = LuaVector2iUtils::retrieveTableField(L, -1, LuaNames::AppConfiguration::windowPosition);
	appCfg.windowPosition = windowPosition;
	const bool fullScreen = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::fullScreen);
	appCfg.fullScreen = fullScreen;
	const bool resizable = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::resizable);
	appCfg.resizable = resizable;
	const bool windowScaling = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::windowScaling);
	appCfg.windowScaling = windowScaling;
	const unsigned int frameLimit = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::frameLimit);
	appCfg.frameLimit = frameLimit;

	const char *windowTitle = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::windowTitle);
	appCfg.windowTitle = windowTitle;
	const char *windowIconFilename = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::windowIconFilename);
	appCfg.windowIconFilename = windowIconFilename;

	const bool useBufferMapping = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::useBufferMapping);
	appCfg.useBufferMapping = useBufferMapping;
	const bool deferShaderQueries = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::deferShaderQueries);
	appCfg.deferShaderQueries = deferShaderQueries;
	const unsigned int fixedBatchSize = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::fixedBatchSize);
	appCfg.fixedBatchSize = fixedBatchSize;
	const bool useBinaryShaderCache = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::useBinaryShaderCache);
	appCfg.useBinaryShaderCache = useBinaryShaderCache;
	const char *shaderCacheDirname = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::shaderCacheDirname);
	appCfg.shaderCacheDirname = shaderCacheDirname;
	const bool compileBatchedShadersTwice = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::compileBatchedShadersTwice);
	appCfg.compileBatchedShadersTwice = compileBatchedShadersTwice;
	const unsigned long vboSize = LuaUtils::retrieveField<uint64_t>(L, -1, LuaNames::AppConfiguration::vboSize);
	appCfg.vboSize = vboSize;
	const unsigned long iboSize = LuaUtils::retrieveField<uint64_t>(L, -1, LuaNames::AppConfiguration::iboSize);
	appCfg.iboSize = iboSize;
	const unsigned int vaoPoolSize = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::vaoPoolSize);
	appCfg.vaoPoolSize = vaoPoolSize;
	const unsigned int renderCommandPoolSize = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::renderCommandPoolSize);
	appCfg.renderCommandPoolSize = renderCommandPoolSize;

	const unsigned int outputAudioFrequency = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::outputAudioFrequency);
	appCfg.outputAudioFrequency = outputAudioFrequency;
	const unsigned int monoAudioSources = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::monoAudioSources);
	appCfg.monoAudioSources = monoAudioSources;
	const unsigned int stereoAudioSources = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::stereoAudioSources);
	appCfg.stereoAudioSources = stereoAudioSources;

	const bool withDebugOverlay = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withDebugOverlay);
	appCfg.withDebugOverlay = withDebugOverlay;
	const bool withAudio = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withAudio);
	appCfg.withAudio = withAudio;
	const bool withThreads = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withThreads);
	appCfg.withThreads = withThreads;
	const bool withScenegraph = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withScenegraph);
	appCfg.withScenegraph = withScenegraph;
	const bool withVSync = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withVSync);
	appCfg.withVSync = withVSync;
	const bool withGlDebugContext = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withGlDebugContext);
	appCfg.withGlDebugContext = withGlDebugContext;
	const bool withConsoleColors = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withConsoleColors);
	appCfg.withConsoleColors = withConsoleColors;
}

}
