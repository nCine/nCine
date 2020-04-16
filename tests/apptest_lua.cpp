#include "apptest_lua.h"
#include <ncine/Application.h>
#include <ncine/LuaIAppEventHandler.h>
#include <ncine/LuaIInputEventHandler.h>
#include "apptest_datapath.h"

namespace {

const char *ScriptFile = "script.lua";

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

MyEventHandler::MyEventHandler()
    : luaState_(nc::LuaStateManager::ApiType::FULL,
                nc::LuaStateManager::StatisticsTracking::ENABLED,
                nc::LuaStateManager::StandardLibraries::LOADED)
{
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);

	luaState_.exposeScriptApi();
	luaState_.run(prefixDataPath("scripts", ScriptFile).data());
	nc::LuaIAppEventHandler::onPreInit(luaState_.state(), config);
}

void MyEventHandler::onInit()
{
	nc::LuaIAppEventHandler::onInit(luaState_.state());
}

void MyEventHandler::onFrameStart()
{
	nc::LuaIAppEventHandler::onFrameStart(luaState_.state());
}

void MyEventHandler::onShutdown()
{
	nc::LuaIAppEventHandler::onShutdown(luaState_.state());
}

void MyEventHandler::onSuspend()
{
	nc::LuaIAppEventHandler::onSuspend(luaState_.state());
}

void MyEventHandler::onResume()
{
	nc::LuaIAppEventHandler::onResume(luaState_.state());
}

void MyEventHandler::onKeyPressed(const nc::KeyboardEvent &event)
{
	nc::LuaIInputEventHandler::onKeyPressed(luaState_.state(), event);
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	nc::LuaIInputEventHandler::onKeyReleased(luaState_.state(), event);

	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
	{
		const bool isSuspended = nc::theApplication().isSuspended();
		nc::theApplication().setSuspended(!isSuspended);
	}
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	nc::LuaIInputEventHandler::onTouchDown(luaState_.state(), event);
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	nc::LuaIInputEventHandler::onTouchUp(luaState_.state(), event);
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	nc::LuaIInputEventHandler::onTouchMove(luaState_.state(), event);
}

void MyEventHandler::onPointerDown(const nc::TouchEvent &event)
{
	nc::LuaIInputEventHandler::onPointerDown(luaState_.state(), event);
}

void MyEventHandler::onPointerUp(const nc::TouchEvent &event)
{
	nc::LuaIInputEventHandler::onPointerUp(luaState_.state(), event);
}

void MyEventHandler::onAcceleration(const nc::AccelerometerEvent &event)
{
	nc::LuaIInputEventHandler::onAcceleration(luaState_.state(), event);
}
#endif

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	nc::LuaIInputEventHandler::onMouseButtonPressed(luaState_.state(), event);
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	nc::LuaIInputEventHandler::onMouseButtonReleased(luaState_.state(), event);
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	nc::LuaIInputEventHandler::onMouseMoved(luaState_.state(), state);
}

void MyEventHandler::onScrollInput(const nc::ScrollEvent &event)
{
	nc::LuaIInputEventHandler::onScrollInput(luaState_.state(), event);
}

void MyEventHandler::onJoyButtonPressed(const nc::JoyButtonEvent &event)
{
	nc::LuaIInputEventHandler::onJoyButtonPressed(luaState_.state(), event);
}

void MyEventHandler::onJoyButtonReleased(const nc::JoyButtonEvent &event)
{
	nc::LuaIInputEventHandler::onJoyButtonReleased(luaState_.state(), event);
}

void MyEventHandler::onJoyHatMoved(const nc::JoyHatEvent &event)
{
	nc::LuaIInputEventHandler::onJoyHatMoved(luaState_.state(), event);
}

void MyEventHandler::onJoyAxisMoved(const nc::JoyAxisEvent &event)
{
	nc::LuaIInputEventHandler::onJoyAxisMoved(luaState_.state(), event);
}

void MyEventHandler::onJoyMappedButtonPressed(const nc::JoyMappedButtonEvent &event)
{
	nc::LuaIInputEventHandler::onJoyMappedButtonPressed(luaState_.state(), event);
}

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	nc::LuaIInputEventHandler::onJoyMappedButtonReleased(luaState_.state(), event);
}

void MyEventHandler::onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event)
{
	nc::LuaIInputEventHandler::onJoyMappedAxisMoved(luaState_.state(), event);
}

void MyEventHandler::onJoyConnected(const nc::JoyConnectionEvent &event)
{
	nc::LuaIInputEventHandler::onJoyConnected(luaState_.state(), event);
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	nc::LuaIInputEventHandler::onJoyDisconnected(luaState_.state(), event);
}
