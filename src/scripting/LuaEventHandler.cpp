#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaEventHandler.h"

#include "LuaNames.h"
#include "LuaStateManager.h"
#include "LuaIAppEventHandler.h"
#include "LuaIInputEventHandler.h"

#include "PCApplication.h"

namespace ncine {

namespace LuaNames {
namespace LuaEventHandler {
	static const char *start = "start";
}}

LuaStateManager *LuaEventHandler::luaState_ = nullptr;

nctl::UniquePtr<IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<LuaEventHandler>();
}

int start(lua_State *L)
{
	int argc = 0;
	const int MaxArguments = 32;
	static const char *argv[MaxArguments];
	for (unsigned int i = 0; i < MaxArguments; i++)
		argv[i] = nullptr;

	lua_getglobal(L, "arg");
	if (lua_istable(L, -1))
	{
		argc = lua_rawlen(L, -1) + 1;
		if (argc > MaxArguments)
			argc = MaxArguments;

		for (int i = 0; i < argc; i++)
		{
			const int type = lua_rawgeti(L, -1, i); // argc array start from index 0 with the name of the script
			if (type == LUA_TSTRING)
				argv[i] = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
	}

	ncine::PCApplication::start(createAppEventHandler, argc, const_cast<char **>(argv));
	return 0;
}

extern "C"
{

#if defined(_WIN32) && !defined(__MINGW32__)

	#if NCINE_DEBUG
		#define NCINE_LUAOPEN_NAME luaopen_ncine_d
	#else
		#define NCINE_LUAOPEN_NAME luaopen_ncine
	#endif

#else

	#if NCINE_DEBUG
		#define NCINE_LUAOPEN_NAME luaopen_libncine_d
	#else
		#define NCINE_LUAOPEN_NAME luaopen_libncine
	#endif

#endif

	DLL_PUBLIC int NCINE_LUAOPEN_NAME(lua_State *L)
	{
		LuaEventHandler::luaState_ = new LuaStateManager(L, LuaStateManager::ApiType::FULL,
		                                                 LuaStateManager::StatisticsTracking::ENABLED,
		                                                 LuaStateManager::StandardLibraries::LOADED);
		lua_getglobal(L, LuaNames::ncine);

		lua_pushcfunction(L, start);
		lua_setfield(L, -2, LuaNames::LuaEventHandler::start);

		return 1;
	}
}

void LuaEventHandler::onPreInit(AppConfiguration &config)
{
#ifdef NCINE_TESTS_DATA_DIR
	config.dataPath() = NCINE_TESTS_DATA_DIR;
#endif

	LuaIAppEventHandler::onPreInit(luaState_->state(), config);
}

void LuaEventHandler::onInit()
{
	LuaIAppEventHandler::onInit(luaState_->state());
}

void LuaEventHandler::onFrameStart()
{
	LuaIAppEventHandler::onFrameStart(luaState_->state());
}

void LuaEventHandler::onPostUpdate()
{
	LuaIAppEventHandler::onPostUpdate(luaState_->state());
}

void LuaEventHandler::onDrawViewport(Viewport &viewport)
{
	LuaIAppEventHandler::onDrawViewport(luaState_->state(), viewport);
}

void LuaEventHandler::onFrameEnd()
{
	LuaIAppEventHandler::onFrameEnd(luaState_->state());
}

void LuaEventHandler::onShutdown()
{
	LuaIAppEventHandler::onShutdown(luaState_->state());
	delete luaState_;
}

void LuaEventHandler::onSuspend()
{
	LuaIAppEventHandler::onSuspend(luaState_->state());
}

void LuaEventHandler::onResume()
{
	LuaIAppEventHandler::onResume(luaState_->state());
}

void LuaEventHandler::onKeyPressed(const KeyboardEvent &event)
{
	LuaIInputEventHandler::onKeyPressed(luaState_->state(), event);
}

void LuaEventHandler::onKeyReleased(const KeyboardEvent &event)
{
	LuaIInputEventHandler::onKeyReleased(luaState_->state(), event);
}

void LuaEventHandler::onMouseButtonPressed(const MouseEvent &event)
{
	LuaIInputEventHandler::onMouseButtonPressed(luaState_->state(), event);
}

void LuaEventHandler::onMouseButtonReleased(const MouseEvent &event)
{
	LuaIInputEventHandler::onMouseButtonReleased(luaState_->state(), event);
}

void LuaEventHandler::onMouseMoved(const MouseState &state)
{
	LuaIInputEventHandler::onMouseMoved(luaState_->state(), state);
}

void LuaEventHandler::onScrollInput(const ScrollEvent &event)
{
	LuaIInputEventHandler::onScrollInput(luaState_->state(), event);
}

void LuaEventHandler::onJoyButtonPressed(const JoyButtonEvent &event)
{
	LuaIInputEventHandler::onJoyButtonPressed(luaState_->state(), event);
}

void LuaEventHandler::onJoyButtonReleased(const JoyButtonEvent &event)
{
	LuaIInputEventHandler::onJoyButtonReleased(luaState_->state(), event);
}

void LuaEventHandler::onJoyHatMoved(const JoyHatEvent &event)
{
	LuaIInputEventHandler::onJoyHatMoved(luaState_->state(), event);
}

void LuaEventHandler::onJoyAxisMoved(const JoyAxisEvent &event)
{
	LuaIInputEventHandler::onJoyAxisMoved(luaState_->state(), event);
}

void LuaEventHandler::onJoyMappedButtonPressed(const JoyMappedButtonEvent &event)
{
	LuaIInputEventHandler::onJoyMappedButtonPressed(luaState_->state(), event);
}

void LuaEventHandler::onJoyMappedButtonReleased(const JoyMappedButtonEvent &event)
{
	LuaIInputEventHandler::onJoyMappedButtonReleased(luaState_->state(), event);
}

void LuaEventHandler::onJoyMappedAxisMoved(const JoyMappedAxisEvent &event)
{
	LuaIInputEventHandler::onJoyMappedAxisMoved(luaState_->state(), event);
}

void LuaEventHandler::onJoyConnected(const JoyConnectionEvent &event)
{
	LuaIInputEventHandler::onJoyConnected(luaState_->state(), event);
}

void LuaEventHandler::onJoyDisconnected(const JoyConnectionEvent &event)
{
	LuaIInputEventHandler::onJoyDisconnected(luaState_->state(), event);
}

}
