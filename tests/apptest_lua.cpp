#include "apptest_lua.h"
#include <ncine/Application.h>
#include <ncine/LuaIAppEventHandler.h>
#include <ncine/LuaIInputEventHandler.h>
#include <ncine/FileSystem.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const char *DefaultScriptName = "script.lua";

const char *scriptName = nullptr;
nctl::String dataPathScript;

bool scriptLoaded = false;
bool dataPathScriptLoaded = false;

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

	scriptName = (config.argc() > 1) ? config.argv(1) : DefaultScriptName;
	dataPathScript = prefixDataPath("scripts", scriptName);

	if (nc::fs::isReadableFile(scriptName))
		scriptLoaded = luaState_.runFromFile(scriptName);
	else if (nc::fs::isReadableFile(dataPathScript.data()))
		dataPathScriptLoaded = luaState_.runFromFile(dataPathScript.data(), scriptName);

	nc::LuaIAppEventHandler::onPreInit(luaState_.state(), config);
}

void MyEventHandler::onInit()
{
	if (scriptLoaded == false && dataPathScriptLoaded == false)
	{
		nctl::String errorString;
		errorString.format("Couldn't load \"%s\"", scriptName);
		if (dataPathScript.compare(scriptName))
			errorString.formatAppend(" or \"%s\"", dataPathScript.data());

		nc::SceneNode &rootNode = nc::theApplication().rootNode();

		font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
		                                   (prefixDataPath("fonts", FontTextureFile)).data());
		text_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get());
		text_->setString(errorString);
		text_->setAlignment(nc::TextNode::Alignment::LEFT);
		text_->setPosition(text_->width() * 0.5f, text_->height() * 0.5f);
	}

	nc::LuaIAppEventHandler::onInit(luaState_.state());
}

void MyEventHandler::onFrameStart()
{
	nc::LuaIAppEventHandler::onFrameStart(luaState_.state());
}

void MyEventHandler::onPostUpdate()
{
	nc::LuaIAppEventHandler::onPostUpdate(luaState_.state());
}

void MyEventHandler::onDrawViewport(nc::Viewport &viewport)
{
	nc::LuaIAppEventHandler::onDrawViewport(luaState_.state(), viewport);
}

void MyEventHandler::onFrameEnd()
{
	nc::LuaIAppEventHandler::onFrameEnd(luaState_.state());
}

void MyEventHandler::onResizeWindow(int width, int height)
{
	nc::LuaIAppEventHandler::onResizeWindow(luaState_.state(), width, height);
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

	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}

void MyEventHandler::onTextInput(const nc::TextInputEvent &event)
{
	nc::LuaIInputEventHandler::onTextInput(luaState_.state(), event);
}

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

#ifdef __ANDROID__
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

bool MyEventHandler::onQuitRequest()
{
	return nc::LuaIInputEventHandler::onQuitRequest(luaState_.state());
}
