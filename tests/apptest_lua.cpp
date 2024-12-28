#include <ncine/config.h>
#include <ncine/version.h>
#include <cstdio>

#include "apptest_lua.h"
#include <ncine/Application.h>
#include <ncine/LuaUtils.h>
#include <ncine/LuaIAppEventHandler.h>
#include <ncine/LuaIInputEventHandler.h>
#include <ncine/FileSystem.h>
#if !SUPPRESS_ON_SCREEN_ERRORS
	#include <ncine/TextNode.h>
#endif
#include "apptest_datapath.h"

#if NCINE_WITH_IMGUI
	#include <ncine/imgui.h>
#endif

namespace {

/// Used to access the event handler object from the Lua API static functions
MyEventHandler *eventHandlerPtr = nullptr;

#ifdef __ANDROID__
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const char *DefaultScriptName = "script.lua";
const float MinErrorStringScale = 0.5f;

enum LuaAppEventFunctions {
	ON_PRE_INIT = 0,
	ON_INIT,
	ON_FRAME_START,
	ON_POST_UPDATE,
	ON_DRAW_VIEWPORT,
	ON_FRAME_END,
	ON_RESIZE_WINDOW,
	ON_CHANGE_SCALING_FACTOR,
	ON_SHUTDOWN,
	ON_SUSPEND,
	ON_RESUME,

	COUNT
};

nctl::String scriptPath(512);
nctl::String scriptDir(512);
nctl::String scriptName(64);
nctl::String scriptErrorMsg(512);
nctl::String funcErrorMsg[LuaAppEventFunctions::COUNT] = {
    nctl::String(512), nctl::String(512), nctl::String(512), nctl::String(512),
    nctl::String(512), nctl::String(512), nctl::String(512), nctl::String(512),
    nctl::String(512), nctl::String(512), nctl::String(512)
};
nctl::String errorString(512);
bool scriptToLoadIsReadable = false;
bool scriptExecuted = false;

struct FileDateCheck
{
	bool enabled = true;
	bool fastReload = false;
	float interval = 0.5f;
	nc::fs::FileDate lastDate;
	nc::TimeStamp lastCheck;
} fileDateCheck;

namespace LuaNames {
	static const char *reloadScript = "reload_script";
	static const char *showGui = "show_gui";
	static const char *isGuiHidden = "is_gui_hidden";
}

bool passKeyEvents = true;
#if NCINE_WITH_IMGUI
bool showImGui = false;
nctl::String windowName(64);

nctl::String scriptPathToLoad(512);

// Keep in sync with `LuaNames::LuaIAppEventHandler` strings
const char *LuaAppEventFunctionNames[LuaAppEventFunctions::COUNT] {
	"on_pre_init",
	"on_init",
	"on_frame_start",
	"on_post_update",
	"on_draw_viewport",
	"on_frame_end",
	"on_resize_window",
	"on_change_scaling_factor",
	"on_shutdown",
	"on_suspend",
	"on_resume",
};

int inputTextCallback(ImGuiInputTextCallbackData *data)
{
	nctl::String *string = reinterpret_cast<nctl::String *>(data->UserData);
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		ASSERT(data->Buf == string->data());
		string->setLength(static_cast<unsigned int>(data->BufTextLen));
		data->Buf = string->data();
	}
	return 0;
}
#endif

/// Returns an integer based on the file date comparison result (similarly to `strcmp`)
int compareDates(const nc::fs::FileDate &dateA, const nc::fs::FileDate &dateB)
{
	const int newerA = 1;
	const int newerB = -1;
	const int equal = 0;

	int comparison = (dateA.year > dateB.year) ? newerA : ((dateA.year < dateB.year) ? newerB : equal);
	comparison = (comparison == equal) ? (dateA.month > dateB.month) ? newerA : ((dateA.month < dateB.month) ? newerB : equal) : comparison;
	comparison = (comparison == equal) ? (dateA.day > dateB.day) ? newerA : ((dateA.day < dateB.day) ? newerB : equal) : comparison;
	comparison = (comparison == equal) ? (dateA.hour > dateB.hour) ? newerA : ((dateA.hour < dateB.hour) ? newerB : equal) : comparison;
	comparison = (comparison == equal) ? (dateA.minute > dateB.minute) ? newerA : ((dateA.minute < dateB.minute) ? newerB : equal) : comparison;
	comparison = (comparison == equal) ? (dateA.second > dateB.second) ? newerA : ((dateA.second < dateB.second) ? newerB : equal) : comparison;

	return comparison;
}

/// Prepends the specified path to the `package.path` string of the Lua package library
/*! \note The same function is also present in `apptest_luareload.cpp` */
void prependPackagePath(lua_State *L, const char *path)
{
	static nctl::String packagePath(512);

	nc::LuaUtils::getGlobal(L, "package");
	nc::LuaUtils::getField(L, -1, "path");
	const char *oldPackagePath = nc::LuaUtils::retrieve<const char *>(L, -1);
	packagePath.format("%s;%s", nc::fs::joinPath(path, "?.lua").data(), oldPackagePath);

	nc::LuaUtils::pop(L); // pop the retrieved old package path string
	nc::LuaUtils::push(L, packagePath.data());
	nc::LuaUtils::setField(L, -2, "path");
	nc::LuaUtils::pop(L); // pop the package table
}

/// Checks if any of the error strings for the callback functions contains text
bool funcErrorMsgEmpty()
{
	bool areEmpty = true;
	for (unsigned int i = 0; i < LuaAppEventFunctions::COUNT; i++)
	{
		if (funcErrorMsg[i].isEmpty() == false)
		{
			areEmpty = false;
			break;
		}
	}
	return areEmpty;
}

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

MyEventHandler::MyEventHandler()
    : luaState_(nc::LuaStateManager::ApiType::FULL,
                nc::LuaStateManager::StatisticsTracking::DISABLED,
                nc::LuaStateManager::StandardLibraries::LOADED)
{
	eventHandlerPtr = this;
	expose(luaState_.state());
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);

	if (config.argc() == 2)
	{
		if (strncmp(config.argv(1), "-v", 2) == 0 || strncmp(config.argv(1), "--version", 9) == 0)
		{
			printf("%s from nCine %s (%s)\n", nc::fs::baseName(config.argv(0)).data(),
			       nc::VersionStrings::Version, nc::VersionStrings::GitBranch);
			config.consoleLogLevel = nc::ILogger::LogLevel::OFF;
			nc::theApplication().quit();
			return;
		}
		else if (strncmp(config.argv(1), "-h", 2) == 0 || strncmp(config.argv(1), "--help", 6) == 0)
		{
			printf("Usage: %s [FILE]\nExecute the specified Lua script, or the default one (\"%s\") if unspecified.\n", config.argv(0), DefaultScriptName);
			config.consoleLogLevel = nc::ILogger::LogLevel::OFF;
			nc::theApplication().quit();
			return;
		}
	}
	else if (config.argc() > 2)
		LOGI_X("Additional console parameters after the first one are ignored.");

	scriptPath = (config.argc() > 1) ? config.argv(1) : DefaultScriptName;

	if (nc::fs::isReadableFile(scriptPath.data()) == false)
		scriptPath = prefixDataPath("scripts", scriptPath.data());

	if (nc::fs::isReadableFile(scriptPath.data()))
	{
		scriptToLoadIsReadable = true;
		scriptDir = nc::fs::dirName(scriptPath.data());
		prependPackagePath(luaState_.state(), scriptDir.data());

		scriptName = nc::fs::baseName(scriptPath.data());
		scriptExecuted = luaState_.runFromFile(scriptPath.data(), scriptName.data(), &scriptErrorMsg);
	}

	if (scriptExecuted)
	{
		fileDateCheck.lastDate = nc::fs::lastModificationTime(scriptPath.data());
		fileDateCheck.lastCheck = nc::TimeStamp::now();
		LOGI_X("Executed Lua script \"%s\" successfully", scriptPath.data());
	}

#if NCINE_WITH_IMGUI
		scriptPathToLoad = scriptPath;
#endif

	nc::LuaIAppEventHandler::onPreInit(luaState_.state(), config, &funcErrorMsg[ON_PRE_INIT]);
}

void MyEventHandler::onInit()
{
#if !SUPPRESS_ON_SCREEN_ERRORS
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());
	text_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get(), 512);
	text_->setAlignment(nc::TextNode::Alignment::LEFT);
#endif

	if (fileDateCheck.enabled)
	{
		// Prevent suspension if hot reload is enabled (allows editing while the application is running)
		nc::theApplication().setAutoSuspension(false);
	}

	nc::LuaIAppEventHandler::onInit(luaState_.state(), &funcErrorMsg[ON_INIT]);
}

void MyEventHandler::onFrameStart()
{
#if NCINE_WITH_IMGUI
	if (showImGui)
	{
		const int argc = nc::theApplication().appConfiguration().argc();
		windowName = (argc > 0) ? nc::fs::baseName(nc::theApplication().appConfiguration().argv(0)) : "apptest_lua";

		ImGui::SetNextWindowSize(ImVec2(350.0f, 215.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(25.0f, 25.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin(windowName.data(), &showImGui))
		{
			passKeyEvents = true;
			const ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackResize;
			if (ImGui::InputText("Execute", scriptPathToLoad.data(), scriptPathToLoad.capacity(), inputTextFlags, inputTextCallback, &scriptPathToLoad))
			{
				scriptPath = scriptPathToLoad;
				scriptToLoadIsReadable = nc::fs::isReadableFile(scriptPathToLoad.data());
				scriptErrorMsg.clear();
				if (scriptToLoadIsReadable)
					runScript(LoadMode::NORMAL);
			}
			if (ImGui::IsItemActive())
				passKeyEvents = false;

			ImGui::Text("Can load \"%s\":", scriptPath.data());
			ImGui::SameLine();
			if (scriptToLoadIsReadable)
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "yes");
			else
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "no");
			ImGui::TextUnformatted("Executed:");
			ImGui::SameLine();
			if (scriptExecuted)
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "yes");
			else
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "no");

			if ((scriptErrorMsg.isEmpty() == false || funcErrorMsgEmpty() == false) && ImGui::TreeNode("Error messages"))
			{
				if (scriptErrorMsg.isEmpty() == false)
					ImGui::InputTextMultiline("Script", scriptErrorMsg.data(), scriptErrorMsg.length() + 1, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);
				ImGui::Separator();
				for (unsigned int i = 0; i < LuaAppEventFunctions::COUNT; i++)
				{
					if (funcErrorMsg[i].isEmpty() == false)
					{
						const unsigned int strLen = nctl::strnlen(funcErrorMsg[i].data(), 512);
						ImGui::InputTextMultiline(LuaAppEventFunctionNames[i], funcErrorMsg[i].data(), funcErrorMsg[i].length() + 1, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);
						ImGui::Separator();
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::Button("Reload"))
				runScript(LoadMode::NORMAL);
			ImGui::SameLine();
			if (ImGui::Button("Fast reload"))
				runScript(LoadMode::FAST);

			const ImGuiTreeNodeFlags treeNodeFlags = (fileDateCheck.enabled) ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
			if (ImGui::TreeNodeEx("File date check", treeNodeFlags))
			{
				ImGui::Checkbox("Enabled", &fileDateCheck.enabled);
				ImGui::BeginDisabled(fileDateCheck.enabled == false);
				ImGui::SameLine();
				ImGui::Checkbox("Fast reload", &fileDateCheck.fastReload);
				ImGui::SameLine();
				bool autoSuspension = nc::theApplication().autoSuspension();
				ImGui::Checkbox("Auto-suspension", &autoSuspension);
				nc::theApplication().setAutoSuspension(autoSuspension);
				ImGui::SliderFloat("Interval", &fileDateCheck.interval, 0.1f, 5.0f, "%.1f s");
				const nc::fs::FileDate &date = fileDateCheck.lastDate;
				ImGui::Text("Last modification date: %d-%02d-%02d %02d:%02d:%02d", date.year, date.month, date.day, date.hour, date.minute, date.second);
				const float nextCheckFraction = fileDateCheck.enabled ? (fileDateCheck.lastCheck.secondsSince() / fileDateCheck.interval) : 0.0f;
				ImGui::ProgressBar(nextCheckFraction, ImVec2(0.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text("%.2f s", fileDateCheck.enabled ? (fileDateCheck.interval - fileDateCheck.lastCheck.secondsSince()) : 0.0f);
				ImGui::EndDisabled();
				ImGui::TreePop();
			}
			ImGui::End();
		}
	}
#endif

	nc::LuaIAppEventHandler::onFrameStart(luaState_.state(), &funcErrorMsg[ON_FRAME_START]);

#if !SUPPRESS_ON_SCREEN_ERRORS
	errorString.clear();
	if (scriptToLoadIsReadable && scriptExecuted && scriptErrorMsg.isEmpty() && funcErrorMsgEmpty())
	{
		text_->setString("");
		text_->setEnabled(false);
	}
	else
	{
		if (scriptToLoadIsReadable == false || scriptExecuted == false)
		{
			const char *actionStr = (scriptToLoadIsReadable) == false ? "load" : "execute";
			errorString.format("Couldn't %s Lua script: \"%s\"\n", actionStr, scriptPath.data());

			for (unsigned int i = 0; i < LuaAppEventFunctions::COUNT; i++)
				funcErrorMsg[i].clear();
		}

		if (scriptErrorMsg.isEmpty() == false)
			errorString.formatAppend("%s\n", scriptErrorMsg.data());
		if (funcErrorMsgEmpty() == false)
		{
			for (unsigned int i = 0; i < LuaAppEventFunctions::COUNT; i++)
			{
				if (funcErrorMsg[i].isEmpty() == false)
					errorString.formatAppend("%s\n", funcErrorMsg[i].data());
			}
		}
		text_->setString(errorString);
		text_->setEnabled(true);

		float scale = 1.0f;
		while (text_->width() > nc::theApplication().width() && scale >= MinErrorStringScale)
		{
			scale -= 0.05f;
			text_->setScale(scale);
		}
		text_->setPosition(text_->width() * 0.5f, text_->height() * 0.5f);
	}
#endif

	if (fileDateCheck.enabled && fileDateCheck.lastCheck.secondsSince() >= fileDateCheck.interval)
	{
		fileDateCheck.lastCheck = nc::TimeStamp::now();
		if (nc::fs::isWritableFile(scriptPath.data()))
		{
			const nc::fs::FileDate fileDate = nc::fs::lastModificationTime(scriptPath.data());
			if (compareDates(fileDate, fileDateCheck.lastDate) > 0)
			{
				runScript(fileDateCheck.fastReload ? LoadMode::FAST : LoadMode::NORMAL);
				fileDateCheck.lastDate = fileDate;
			}
		}
	}
}

void MyEventHandler::onPostUpdate()
{
	nc::LuaIAppEventHandler::onPostUpdate(luaState_.state(), &funcErrorMsg[ON_POST_UPDATE]);
}

void MyEventHandler::onDrawViewport(nc::Viewport &viewport)
{
	nc::LuaIAppEventHandler::onDrawViewport(luaState_.state(), viewport, &funcErrorMsg[ON_DRAW_VIEWPORT]);
}

void MyEventHandler::onFrameEnd()
{
	nc::LuaIAppEventHandler::onFrameEnd(luaState_.state(), &funcErrorMsg[ON_FRAME_END]);
}

void MyEventHandler::onResizeWindow(int width, int height)
{
	nc::LuaIAppEventHandler::onResizeWindow(luaState_.state(), width, height, &funcErrorMsg[ON_RESIZE_WINDOW]);
}

void MyEventHandler::onChangeScalingFactor(float factor)
{
	nc::LuaIAppEventHandler::onChangeScalingFactor(luaState_.state(), factor, &funcErrorMsg[ON_CHANGE_SCALING_FACTOR]);
}

void MyEventHandler::onShutdown()
{
	nc::LuaIAppEventHandler::onShutdown(luaState_.state(), &funcErrorMsg[ON_SHUTDOWN]);
}

void MyEventHandler::onSuspend()
{
	nc::LuaIAppEventHandler::onSuspend(luaState_.state(), &funcErrorMsg[ON_SUSPEND]);
}

void MyEventHandler::onResume()
{
	nc::LuaIAppEventHandler::onResume(luaState_.state(), &funcErrorMsg[ON_RESUME]);
}

void MyEventHandler::onKeyPressed(const nc::KeyboardEvent &event)
{
	if (passKeyEvents)
		nc::LuaIInputEventHandler::onKeyPressed(luaState_.state(), event);
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (passKeyEvents)
		nc::LuaIInputEventHandler::onKeyReleased(luaState_.state(), event);
}

void MyEventHandler::onTextInput(const nc::TextInputEvent &event)
{
	if (passKeyEvents)
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

void MyEventHandler::expose(lua_State *L)
{
	nc::LuaUtils::addGlobalFunction(L, LuaNames::reloadScript, reloadScript);
	nc::LuaUtils::addGlobalFunction(L, LuaNames::showGui, showGui);
	nc::LuaUtils::addGlobalFunction(L, LuaNames::isGuiHidden, isGuiHidden);
}

int MyEventHandler::reloadScript(lua_State *L)
{
	// Fast relaod only: cannot destroy and recreate the state from a Lua function
	eventHandlerPtr->runScript(LoadMode::FAST);
	return 0;
}

int MyEventHandler::showGui(lua_State *L)
{
#if NCINE_WITH_IMGUI
	const bool showGui = nc::LuaUtils::retrieve<bool>(L, -1);
	showImGui = showGui;
#endif
	return 0;
}

int MyEventHandler::isGuiHidden(lua_State *L)
{
#if NCINE_WITH_IMGUI
	nc::LuaUtils::push(L, !showImGui);
#else
	nc::LuaUtils::push(L, true);
#endif

	return 1;
}

bool MyEventHandler::runScript(LoadMode mode)
{
	nc::LuaIAppEventHandler::onShutdown(luaState_.state());

	if (mode == LoadMode::NORMAL && scriptExecuted == true)
	{
		luaState_.reopen();
		expose(luaState_.state());

		scriptDir = nc::fs::dirName(scriptPath.data());
		prependPackagePath(luaState_.state(), scriptDir.data());
	}

	scriptName = nc::fs::baseName(scriptPath.data());
	scriptExecuted = luaState_.runFromFile(scriptPath.data(), scriptName.data(), &scriptErrorMsg);

	if (scriptExecuted)
	{
		fileDateCheck.lastDate = nc::fs::lastModificationTime(scriptPath.data());
		fileDateCheck.lastCheck = nc::TimeStamp::now();
		LOGI_X("Executed Lua script \"%s\" correctly", scriptPath.data());
		nc::LuaIAppEventHandler::onInit(luaState_.state(), &funcErrorMsg[ON_INIT]);
	}

	return scriptExecuted;
}
