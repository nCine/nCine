#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaAppConfiguration.h"
#include "LuaClassWrapper.h"
#include "LuaUtils.h"
#include "LuaVector2Utils.h"
#include "AppConfiguration.h"

namespace ncine {

namespace LuaNames {
namespace AppConfiguration {
	static const char *dataPath = "data_path";
	static const char *logFile = "log_file";
	static const char *consoleLogLevel = "console_log_level";
	static const char *fileLogLevel = "file_log_level";
	static const char *frameTimerLogInterval = "log_interval";

	static const char *resolution = "resolution";
	static const char *inFullscreen = "fullscreen";
	static const char *isResizable = "resizable";
	static const char *frameLimit = "frame_limit";

	static const char *windowTitle = "window_title";
	static const char *windowIconFilename = "window_icon";

	static const char *useBufferMapping = "buffer_mapping";
	static const char *deferShaderQueries = "defer_shader_queries";
	static const char *fixedBatchSize = "fixed_batch_size";
	static const char *vboSize = "vbo_size";
	static const char *iboSize = "ibo_size";
	static const char *vaoPoolSize = "vao_pool_size";

	static const char *withDebugOverlay = "debug_overlay";
	static const char *withAudio = "audio";
	static const char *withThreads = "threads";
	static const char *withScenegraph = "scenegraph";
	static const char *withVSync = "vsync";
	static const char *withGlDebugContext = "gl_debug_context";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAppConfiguration::push(lua_State *L, const AppConfiguration &appCfg)
{
	lua_createtable(L, 24, 0);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::dataPath, appCfg.dataPath().data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::logFile, appCfg.logFile.data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::consoleLogLevel, static_cast<int64_t>(appCfg.consoleLogLevel));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::fileLogLevel, static_cast<int64_t>(appCfg.fileLogLevel));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::frameTimerLogInterval, appCfg.frameTimerLogInterval);

	LuaVector2iUtils::pushField(L, LuaNames::AppConfiguration::resolution, appCfg.resolution);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::inFullscreen, appCfg.inFullscreen);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::isResizable, appCfg.isResizable);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::frameLimit, appCfg.frameLimit);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::windowTitle, appCfg.windowTitle.data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::windowIconFilename, appCfg.windowIconFilename.data());

	LuaUtils::pushField(L, LuaNames::AppConfiguration::useBufferMapping, appCfg.useBufferMapping);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::deferShaderQueries, appCfg.deferShaderQueries);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::fixedBatchSize, appCfg.fixedBatchSize);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::vboSize, static_cast<int64_t>(appCfg.vboSize));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::iboSize, static_cast<int64_t>(appCfg.iboSize));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::vaoPoolSize, appCfg.vaoPoolSize);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::withDebugOverlay, appCfg.withDebugOverlay);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withAudio, appCfg.withAudio);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withThreads, appCfg.withThreads);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withScenegraph, appCfg.withScenegraph);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withVSync, appCfg.withVSync);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withGlDebugContext, appCfg.withGlDebugContext);
}

void LuaAppConfiguration::retrieveAndSet(lua_State *L, AppConfiguration &appCfg)
{
	if (lua_istable(L, -1) == false)
		luaL_argerror(L, -1, "Expecting a table");

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
	const bool inFullscreen = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::inFullscreen);
	appCfg.inFullscreen = inFullscreen;
	const bool isResizable = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::isResizable);
	appCfg.isResizable = isResizable;
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
	const unsigned long vboSize = LuaUtils::retrieveField<uint64_t>(L, -1, LuaNames::AppConfiguration::vboSize);
	appCfg.vboSize = vboSize;
	const unsigned long iboSize = LuaUtils::retrieveField<uint64_t>(L, -1, LuaNames::AppConfiguration::iboSize);
	appCfg.iboSize = iboSize;
	const unsigned int vaoPoolSize = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::vaoPoolSize);
	appCfg.vaoPoolSize = vaoPoolSize;

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
}

}
