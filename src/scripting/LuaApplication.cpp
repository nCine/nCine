#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaApplication.h"
#include "LuaAppConfiguration.h"
#include "LuaUntrackedUserData.h"
#include "LuaVector2Utils.h"
#include "Application.h"
#include "FileSystem.h"

namespace ncine {

namespace LuaNames {
namespace Application {
	static const char *Application = "application";

	static const char *appConfiguration = "get_app_configuration";

	static const char *renderingSettings = "get_rendering_settings";
	static const char *setRenderingSettings = "set_rendering_settings";

	static const char *guiSettings = "get_gui_settings";
	static const char *setGuiSettings = "set_gui_settings";

	static const char *debugOverlaySettings = "get_debugoverlay_settings";
	static const char *setDebugOverlaySettings = "set_debugoverlay_settings";

	static const char *rootNode = "get_rootnode";
	static const char *screenViewport = "get_screen_viewport";
	static const char *numFrames = "get_num_frames";
	static const char *frameTime = "get_frame_time";

	static const char *width = "get_width";
	static const char *height = "get_height";
	static const char *resolution = "get_resolution";

	static const char *isSuspended = "is_suspended";
	static const char *setSuspended = "set_suspended";

	static const char *autoSuspension = "get_auto_suspension";
	static const char *setAutoSuspension = "set_auto_suspension";

	static const char *quit = "quit";

	namespace RenderingSettings {
		static const char *batchingEnabled = "batching";
		static const char *batchingWithIndices = "batching_with_indices";
		static const char *cullingEnabled = "culling";
		static const char *minBatchSize = "min_batch_size";
		static const char *maxBatchSize = "max_batch_size";
	}

	namespace GuiSettings {
		static const char *imguiLayer = "imgui_layer";
		static const char *nuklearLayer = "nuklear_layer";
		static const char *imguiViewport = "imgui_viewport";
		static const char *nuklearViewport = "nuklear_viewport";
	}

	namespace DebugOverlaySettings {
		static const char *showProfilerGraphs = "profiler_graphs";
		static const char *showInfoText = "info_text";
		static const char *showInterface = "interface";
	}
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaApplication::expose(lua_State *L)
{
	lua_createtable(L, 0, 15);

	LuaUtils::addFunction(L, LuaNames::Application::appConfiguration, appConfiguration);

	LuaUtils::addFunction(L, LuaNames::Application::renderingSettings, renderingSettings);
	LuaUtils::addFunction(L, LuaNames::Application::setRenderingSettings, setRenderingSettings);

	LuaUtils::addFunction(L, LuaNames::Application::guiSettings, guiSettings);
	LuaUtils::addFunction(L, LuaNames::Application::setGuiSettings, setGuiSettings);

	LuaUtils::addFunction(L, LuaNames::Application::debugOverlaySettings, debugOverlaySettings);
	LuaUtils::addFunction(L, LuaNames::Application::setDebugOverlaySettings, setDebugOverlaySettings);

	LuaUtils::addFunction(L, LuaNames::Application::rootNode, rootNode);
	LuaUtils::addFunction(L, LuaNames::Application::screenViewport, screenViewport);
	LuaUtils::addFunction(L, LuaNames::Application::numFrames, numFrames);
	LuaUtils::addFunction(L, LuaNames::Application::frameTime, frameTime);

	LuaUtils::addFunction(L, LuaNames::Application::width, width);
	LuaUtils::addFunction(L, LuaNames::Application::height, height);
	LuaUtils::addFunction(L, LuaNames::Application::resolution, resolution);

	LuaUtils::addFunction(L, LuaNames::Application::isSuspended, isSuspended);
	LuaUtils::addFunction(L, LuaNames::Application::setSuspended, setSuspended);

	LuaUtils::addFunction(L, LuaNames::Application::autoSuspension, autoSuspension);
	LuaUtils::addFunction(L, LuaNames::Application::setAutoSuspension, setAutoSuspension);

	LuaUtils::addFunction(L, LuaNames::Application::quit, quit);

	lua_setfield(L, -2, LuaNames::Application::Application);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaApplication::appConfiguration(lua_State *L)
{
	const AppConfiguration &appCfg = theApplication().appConfiguration();

	LuaAppConfiguration::push(L, appCfg);

	return 1;
}

int LuaApplication::renderingSettings(lua_State *L)
{
	const Application::RenderingSettings &settings = theApplication().renderingSettings();

	lua_createtable(L, 0, 5);
	LuaUtils::pushField(L, LuaNames::Application::RenderingSettings::batchingEnabled, settings.batchingEnabled);
	LuaUtils::pushField(L, LuaNames::Application::RenderingSettings::batchingWithIndices, settings.batchingWithIndices);
	LuaUtils::pushField(L, LuaNames::Application::RenderingSettings::cullingEnabled, settings.cullingEnabled);
	LuaUtils::pushField(L, LuaNames::Application::RenderingSettings::minBatchSize, settings.minBatchSize);
	LuaUtils::pushField(L, LuaNames::Application::RenderingSettings::maxBatchSize, settings.maxBatchSize);

	return 1;
}

int LuaApplication::setRenderingSettings(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	Application::RenderingSettings &settings = theApplication().renderingSettings();

	settings.batchingEnabled = LuaUtils::retrieveField<bool>(L, -1, LuaNames::Application::RenderingSettings::batchingEnabled);
	settings.batchingWithIndices = LuaUtils::retrieveField<bool>(L, -1, LuaNames::Application::RenderingSettings::batchingWithIndices);
	settings.cullingEnabled = LuaUtils::retrieveField<bool>(L, -1, LuaNames::Application::RenderingSettings::cullingEnabled);
	settings.minBatchSize = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::Application::RenderingSettings::minBatchSize);
	settings.maxBatchSize = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::Application::RenderingSettings::maxBatchSize);

	return 0;
}

int LuaApplication::guiSettings(lua_State *L)
{
	const Application::GuiSettings &settings = theApplication().guiSettings();

	lua_createtable(L, 0, 4);
	LuaUtils::pushField(L, LuaNames::Application::GuiSettings::imguiLayer, settings.imguiLayer);
	LuaUtils::pushField(L, LuaNames::Application::GuiSettings::nuklearLayer, settings.nuklearLayer);
	LuaUtils::pushField(L, LuaNames::Application::GuiSettings::imguiViewport, settings.imguiViewport);
	LuaUtils::pushField(L, LuaNames::Application::GuiSettings::nuklearViewport, settings.nuklearViewport);

	return 1;
}

int LuaApplication::setGuiSettings(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	Application::GuiSettings &settings = theApplication().guiSettings();

	settings.imguiLayer = LuaUtils::retrieveField<unsigned int>(L, -1, LuaNames::Application::GuiSettings::imguiLayer);
	settings.nuklearLayer = LuaUtils::retrieveField<unsigned int>(L, -1, LuaNames::Application::GuiSettings::nuklearLayer);
	settings.imguiViewport = reinterpret_cast<Viewport *>(LuaUtils::retrieveFieldLightUserData(L, -1, LuaNames::Application::GuiSettings::imguiViewport));
	settings.nuklearViewport = reinterpret_cast<Viewport *>(LuaUtils::retrieveFieldLightUserData(L, -1, LuaNames::Application::GuiSettings::imguiViewport));

	return 0;
}

int LuaApplication::debugOverlaySettings(lua_State *L)
{
	const IDebugOverlay::DisplaySettings &settings = theApplication().debugOverlaySettings();

	lua_createtable(L, 0, 3);
	LuaUtils::pushField(L, LuaNames::Application::DebugOverlaySettings::showProfilerGraphs, settings.showProfilerGraphs);
	LuaUtils::pushField(L, LuaNames::Application::DebugOverlaySettings::showInfoText, settings.showInfoText);
	LuaUtils::pushField(L, LuaNames::Application::DebugOverlaySettings::showInterface, settings.showInterface);

	return 1;
}

int LuaApplication::setDebugOverlaySettings(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	IDebugOverlay::DisplaySettings &settings = theApplication().debugOverlaySettings();

	settings.showProfilerGraphs = LuaUtils::retrieveField<bool>(L, -1, LuaNames::Application::DebugOverlaySettings::showProfilerGraphs);
	settings.showInfoText = LuaUtils::retrieveField<bool>(L, -1, LuaNames::Application::DebugOverlaySettings::showInfoText);
	settings.showInterface = LuaUtils::retrieveField<bool>(L, -1, LuaNames::Application::DebugOverlaySettings::showInterface);

	return 0;
}

int LuaApplication::rootNode(lua_State *L)
{
	LuaUntrackedUserData<SceneNode>::push(L, &theApplication().rootNode());
	return 1;
}

int LuaApplication::screenViewport(lua_State *L)
{
	LuaUntrackedUserData<Viewport>::push(L, &theApplication().screenViewport());
	return 1;
}

int LuaApplication::numFrames(lua_State *L)
{
	// The cast disambiguate the `push` function on Emscripten
	LuaUtils::push(L, static_cast<uint64_t>(theApplication().numFrames()));
	return 1;
}

int LuaApplication::frameTime(lua_State *L)
{
	LuaUtils::push(L, theApplication().frameTime());
	return 1;
}

int LuaApplication::width(lua_State *L)
{
	LuaUtils::push(L, theApplication().width());
	return 1;
}

int LuaApplication::height(lua_State *L)
{
	LuaUtils::push(L, theApplication().height());
	return 1;
}

int LuaApplication::resolution(lua_State *L)
{
	LuaVector2fUtils::push(L, theApplication().resolution());
	return 1;
}

int LuaApplication::isSuspended(lua_State *L)
{
	LuaUtils::push(L, theApplication().isSuspended());
	return 1;
}

int LuaApplication::setSuspended(lua_State *L)
{
	const bool suspended = LuaUtils::retrieve<bool>(L, -1);
	theApplication().setSuspended(suspended);
	return 0;
}

int LuaApplication::autoSuspension(lua_State *L)
{
	LuaUtils::push(L, theApplication().autoSuspension());
	return 1;
}

int LuaApplication::setAutoSuspension(lua_State *L)
{
	const bool autoSuspension = LuaUtils::retrieve<bool>(L, -1);
	theApplication().setAutoSuspension(autoSuspension);
	return 0;
}

int LuaApplication::quit(lua_State *L)
{
	theApplication().quit();
	return 0;
}

}
