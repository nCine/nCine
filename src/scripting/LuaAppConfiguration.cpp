#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaAppConfiguration.h"
#include "LuaClassWrapper.h"
#include "LuaUtils.h"
#include "AppConfiguration.h"

namespace ncine {

namespace LuaNames {
namespace AppConfiguration
{
	static const char *dataPath = "data_path";
	static const char *logFile = "log_file";
	static const char *consoleLogLevel = "console_log_level";
	static const char *fileLogLevel = "file_log_level";
	static const char *frameTimerLogInterval = "log_interval";

	static const char *xResolution = "x_res";
	static const char *yResolution = "y_res";
	static const char *inFullscreen = "fullscreen";

	static const char *windowTitle = "window_title";
	static const char *windowIconFilename = "window_icon";

	static const char *fontTexFilename = "font_texture";
	static const char *fontFntFilename = "font_fntfile";

	static const char *vboSize = "vbo_size";
	static const char *iboSize ="ibo_size";
	static const char *vaoPoolSize = "vao_pool_size";

	static const char *withProfilerGraphs = "profiler_graphs";
	static const char *withInfoText = "info_text";
	static const char *withAudio = "audio";
	static const char *withThreads = "threads";
	static const char *withScenegraph = "scenegraph";
	static const char *withVSync = "vsync";
	static const char *withGlDebug = "gldebug";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAppConfiguration::push(lua_State *L, const AppConfiguration &appCfg)
{
	lua_createtable(L, 22, 0);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::dataPath, appCfg.dataPath().data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::logFile, appCfg.logFile().data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::consoleLogLevel, static_cast<int64_t>(appCfg.consoleLogLevel()));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::fileLogLevel, static_cast<int64_t>(appCfg.fileLogLevel()));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::frameTimerLogInterval, appCfg.frameTimerLogInterval());

	LuaUtils::pushField(L, LuaNames::AppConfiguration::xResolution, appCfg.xResolution());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::yResolution, appCfg.yResolution());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::inFullscreen, appCfg.inFullscreen());

	LuaUtils::pushField(L, LuaNames::AppConfiguration::windowTitle, appCfg.windowTitle().data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::windowIconFilename, appCfg.windowIconFilename().data());

	LuaUtils::pushField(L, LuaNames::AppConfiguration::fontTexFilename, appCfg.fontTexFilename().data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::fontFntFilename, appCfg.fontFntFilename().data());

	LuaUtils::pushField(L, LuaNames::AppConfiguration::vboSize, static_cast<int64_t>(appCfg.vboSize()));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::iboSize, static_cast<int64_t>(appCfg.iboSize()));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::vaoPoolSize, appCfg.vaoPoolSize());

	LuaUtils::pushField(L, LuaNames::AppConfiguration::withProfilerGraphs, appCfg.withProfilerGraphs());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withInfoText, appCfg.withInfoText());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withAudio, appCfg.withAudio());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withThreads, appCfg.withThreads());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withScenegraph, appCfg.withScenegraph());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withVSync, appCfg.withVSync());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::withGlDebug, appCfg.withGlDebug());
}

void LuaAppConfiguration::retrieveAndSet(lua_State *L, AppConfiguration &appCfg)
{
	if (lua_istable(L, -1) == false)
		luaL_argerror(L, -1, "Expecting a table");

	const char *dataPath = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::dataPath);
	appCfg.setDataPath(dataPath);
	const char *logFile = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::logFile);
	appCfg.setLogFile(logFile);
	const ILogger::LogLevel consoleLogLevel = static_cast<ILogger::LogLevel>(LuaUtils::retrieveField<int64_t>(L, -1, LuaNames::AppConfiguration::consoleLogLevel));
	appCfg.setConsoleLogLevel(consoleLogLevel);
	const ILogger::LogLevel fileLogLevel = static_cast<ILogger::LogLevel>(LuaUtils::retrieveField<int64_t>(L, -1, LuaNames::AppConfiguration::fileLogLevel));
	appCfg.setFileLogLevel(fileLogLevel);
	const float logInterval = LuaUtils::retrieveField<float>(L, -1, LuaNames::AppConfiguration::frameTimerLogInterval);
	appCfg.setFrameTimerLogInterval(logInterval);

	const unsigned int xRes = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::xResolution);
	const unsigned int yRes = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::yResolution);
	appCfg.setResolution(xRes, yRes);
	const bool inFullscreen = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::inFullscreen);
	appCfg.setFullScreen(inFullscreen);

	const char *windowTitle = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::windowTitle);
	appCfg.setWindowTitle(windowTitle);
	const char *windowIconFilename = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::windowIconFilename);
	appCfg.setWindowIconFilename(windowIconFilename);

	const char *fontTexFilename = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::fontTexFilename);
	appCfg.setFontTexFilename(fontTexFilename);
	const char *fontFntFilename = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::fontFntFilename);
	appCfg.setFontFntFilename(fontFntFilename);

	const unsigned long vboSize = LuaUtils::retrieveField<uint64_t>(L, -1, LuaNames::AppConfiguration::vboSize);
	appCfg.setVboSize(vboSize);
	const unsigned long iboSize = LuaUtils::retrieveField<uint64_t>(L, -1, LuaNames::AppConfiguration::iboSize);
	appCfg.setIboSize(iboSize);
	const unsigned int vaoPoolSize = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::vaoPoolSize);
	appCfg.setVaoPoolSize(vaoPoolSize);

	const bool withProfilerGraphs = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withProfilerGraphs);
	appCfg.enableProfilerGraphs(withProfilerGraphs);
	const bool withInfoText = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withInfoText);
	appCfg.enableInfoText(withInfoText);
	const bool withAudio = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withAudio);
	appCfg.enableAudio(withAudio);
	const bool withThreads = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withThreads);
	appCfg.enableThreads(withThreads);
	const bool withScenegraph = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withScenegraph);
	appCfg.enableScenegraph(withScenegraph);
	const bool withVSync = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withVSync);
	appCfg.enableVSync(withVSync);
	const bool withGlDebug = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::withGlDebug);
	appCfg.enableGlDebug(withGlDebug);
}

}
