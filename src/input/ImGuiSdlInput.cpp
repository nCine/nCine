// Based on imgui/backends/imgui_impl_sdl.cpp

#include "ImGuiSdlInput.h"
#include "ImGuiJoyMappedInput.h"

// SDL
#include <SDL.h>
#include <SDL_syswm.h>
#ifdef __APPLE__
	#include "TargetConditionals.h"
#endif
#ifdef __EMSCRIPTEN__
	#include <emscripten/em_js.h>
#endif

#if SDL_VERSION_ATLEAST(2, 0, 4) && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !(defined(__APPLE__) && TARGET_OS_IOS)
	#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE 1
#else
	#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE 0
#endif
#define SDL_HAS_VULKAN SDL_VERSION_ATLEAST(2, 0, 6)
#define SDL_HAS_OPEN_URL SDL_VERSION_ATLEAST(2,0,14)
#if SDL_HAS_VULKAN
extern "C" { extern DECLSPEC void SDLCALL SDL_Vulkan_GetDrawableSize(SDL_Window *window, int *w, int *h); }
#endif

namespace ncine {

namespace {

	void setClipboardText(ImGuiContext *context, const char *text)
	{
		SDL_SetClipboardText(text);
	}

	// Note: native IME will only display if user calls SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1") _before_ SDL_CreateWindow().
	void setPlatformImeData(ImGuiContext *context, ImGuiViewport *viewport, ImGuiPlatformImeData *data)
	{
		if (data->WantVisible)
		{
			SDL_Rect r;
			r.x = static_cast<int>(data->InputPos.x);
			r.y = static_cast<int>(data->InputPos.y);
			r.w = 1;
			r.h = static_cast<int>(data->InputLineHeight);
			SDL_SetTextInputRect(&r);
		}
	}

	ImGuiViewport *viewportForWindowId(SDL_Window *window, Uint32 windowId)
	{
		const Uint32 thisWindowId = SDL_GetWindowID(window);
		return (thisWindowId == windowId) ? ImGui::GetMainViewport() : nullptr;
	}

	ImGuiKey sdlKeycodeToImGuiKey(SDL_Keycode keycode, SDL_Scancode scancode)
	{
		switch (keycode)
		{
			case SDLK_TAB: return ImGuiKey_Tab;
			case SDLK_LEFT: return ImGuiKey_LeftArrow;
			case SDLK_RIGHT: return ImGuiKey_RightArrow;
			case SDLK_UP: return ImGuiKey_UpArrow;
			case SDLK_DOWN: return ImGuiKey_DownArrow;
			case SDLK_PAGEUP: return ImGuiKey_PageUp;
			case SDLK_PAGEDOWN: return ImGuiKey_PageDown;
			case SDLK_HOME: return ImGuiKey_Home;
			case SDLK_END: return ImGuiKey_End;
			case SDLK_INSERT: return ImGuiKey_Insert;
			case SDLK_DELETE: return ImGuiKey_Delete;
			case SDLK_BACKSPACE: return ImGuiKey_Backspace;
			case SDLK_SPACE: return ImGuiKey_Space;
			case SDLK_RETURN: return ImGuiKey_Enter;
			case SDLK_ESCAPE: return ImGuiKey_Escape;
			//case SDLK_QUOTE: return ImGuiKey_Apostrophe;
			case SDLK_COMMA: return ImGuiKey_Comma;
			//case SDLK_MINUS: return ImGuiKey_Minus;
			case SDLK_PERIOD: return ImGuiKey_Period;
			//case SDLK_SLASH: return ImGuiKey_Slash;
			case SDLK_SEMICOLON: return ImGuiKey_Semicolon;
			//case SDLK_EQUALS: return ImGuiKey_Equal;
			//case SDLK_LEFTBRACKET: return ImGuiKey_LeftBracket;
			//case SDLK_BACKSLASH: return ImGuiKey_Backslash;
			//case SDLK_RIGHTBRACKET: return ImGuiKey_RightBracket;
			//case SDLK_BACKQUOTE: return ImGuiKey_GraveAccent;
			case SDLK_CAPSLOCK: return ImGuiKey_CapsLock;
			case SDLK_SCROLLLOCK: return ImGuiKey_ScrollLock;
			case SDLK_NUMLOCKCLEAR: return ImGuiKey_NumLock;
			case SDLK_PRINTSCREEN: return ImGuiKey_PrintScreen;
			case SDLK_PAUSE: return ImGuiKey_Pause;
			case SDLK_KP_0: return ImGuiKey_Keypad0;
			case SDLK_KP_1: return ImGuiKey_Keypad1;
			case SDLK_KP_2: return ImGuiKey_Keypad2;
			case SDLK_KP_3: return ImGuiKey_Keypad3;
			case SDLK_KP_4: return ImGuiKey_Keypad4;
			case SDLK_KP_5: return ImGuiKey_Keypad5;
			case SDLK_KP_6: return ImGuiKey_Keypad6;
			case SDLK_KP_7: return ImGuiKey_Keypad7;
			case SDLK_KP_8: return ImGuiKey_Keypad8;
			case SDLK_KP_9: return ImGuiKey_Keypad9;
			case SDLK_KP_PERIOD: return ImGuiKey_KeypadDecimal;
			case SDLK_KP_DIVIDE: return ImGuiKey_KeypadDivide;
			case SDLK_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
			case SDLK_KP_MINUS: return ImGuiKey_KeypadSubtract;
			case SDLK_KP_PLUS: return ImGuiKey_KeypadAdd;
			case SDLK_KP_ENTER: return ImGuiKey_KeypadEnter;
			case SDLK_KP_EQUALS: return ImGuiKey_KeypadEqual;
			case SDLK_LCTRL: return ImGuiKey_LeftCtrl;
			case SDLK_LSHIFT: return ImGuiKey_LeftShift;
			case SDLK_LALT: return ImGuiKey_LeftAlt;
			case SDLK_LGUI: return ImGuiKey_LeftSuper;
			case SDLK_RCTRL: return ImGuiKey_RightCtrl;
			case SDLK_RSHIFT: return ImGuiKey_RightShift;
			case SDLK_RALT: return ImGuiKey_RightAlt;
			case SDLK_RGUI: return ImGuiKey_RightSuper;
			case SDLK_APPLICATION: return ImGuiKey_Menu;
			case SDLK_0: return ImGuiKey_0;
			case SDLK_1: return ImGuiKey_1;
			case SDLK_2: return ImGuiKey_2;
			case SDLK_3: return ImGuiKey_3;
			case SDLK_4: return ImGuiKey_4;
			case SDLK_5: return ImGuiKey_5;
			case SDLK_6: return ImGuiKey_6;
			case SDLK_7: return ImGuiKey_7;
			case SDLK_8: return ImGuiKey_8;
			case SDLK_9: return ImGuiKey_9;
			case SDLK_a: return ImGuiKey_A;
			case SDLK_b: return ImGuiKey_B;
			case SDLK_c: return ImGuiKey_C;
			case SDLK_d: return ImGuiKey_D;
			case SDLK_e: return ImGuiKey_E;
			case SDLK_f: return ImGuiKey_F;
			case SDLK_g: return ImGuiKey_G;
			case SDLK_h: return ImGuiKey_H;
			case SDLK_i: return ImGuiKey_I;
			case SDLK_j: return ImGuiKey_J;
			case SDLK_k: return ImGuiKey_K;
			case SDLK_l: return ImGuiKey_L;
			case SDLK_m: return ImGuiKey_M;
			case SDLK_n: return ImGuiKey_N;
			case SDLK_o: return ImGuiKey_O;
			case SDLK_p: return ImGuiKey_P;
			case SDLK_q: return ImGuiKey_Q;
			case SDLK_r: return ImGuiKey_R;
			case SDLK_s: return ImGuiKey_S;
			case SDLK_t: return ImGuiKey_T;
			case SDLK_u: return ImGuiKey_U;
			case SDLK_v: return ImGuiKey_V;
			case SDLK_w: return ImGuiKey_W;
			case SDLK_x: return ImGuiKey_X;
			case SDLK_y: return ImGuiKey_Y;
			case SDLK_z: return ImGuiKey_Z;
			case SDLK_F1: return ImGuiKey_F1;
			case SDLK_F2: return ImGuiKey_F2;
			case SDLK_F3: return ImGuiKey_F3;
			case SDLK_F4: return ImGuiKey_F4;
			case SDLK_F5: return ImGuiKey_F5;
			case SDLK_F6: return ImGuiKey_F6;
			case SDLK_F7: return ImGuiKey_F7;
			case SDLK_F8: return ImGuiKey_F8;
			case SDLK_F9: return ImGuiKey_F9;
			case SDLK_F10: return ImGuiKey_F10;
			case SDLK_F11: return ImGuiKey_F11;
			case SDLK_F12: return ImGuiKey_F12;
			case SDLK_F13: return ImGuiKey_F13;
			case SDLK_F14: return ImGuiKey_F14;
			case SDLK_F15: return ImGuiKey_F15;
			case SDLK_F16: return ImGuiKey_F16;
			case SDLK_F17: return ImGuiKey_F17;
			case SDLK_F18: return ImGuiKey_F18;
			case SDLK_F19: return ImGuiKey_F19;
			case SDLK_F20: return ImGuiKey_F20;
			case SDLK_F21: return ImGuiKey_F21;
			case SDLK_F22: return ImGuiKey_F22;
			case SDLK_F23: return ImGuiKey_F23;
			case SDLK_F24: return ImGuiKey_F24;
			case SDLK_AC_BACK: return ImGuiKey_AppBack;
			case SDLK_AC_FORWARD: return ImGuiKey_AppForward;
			default: break;
		}

		// Fallback to scancode
		switch (scancode)
		{
			case SDL_SCANCODE_GRAVE: return ImGuiKey_GraveAccent;
			case SDL_SCANCODE_MINUS: return ImGuiKey_Minus;
			case SDL_SCANCODE_EQUALS: return ImGuiKey_Equal;
			case SDL_SCANCODE_LEFTBRACKET: return ImGuiKey_LeftBracket;
			case SDL_SCANCODE_RIGHTBRACKET: return ImGuiKey_RightBracket;
			case SDL_SCANCODE_NONUSBACKSLASH: return ImGuiKey_Oem102;
			case SDL_SCANCODE_BACKSLASH: return ImGuiKey_Backslash;
			case SDL_SCANCODE_SEMICOLON: return ImGuiKey_Semicolon;
			case SDL_SCANCODE_APOSTROPHE: return ImGuiKey_Apostrophe;
			case SDL_SCANCODE_COMMA: return ImGuiKey_Comma;
			case SDL_SCANCODE_PERIOD: return ImGuiKey_Period;
			case SDL_SCANCODE_SLASH: return ImGuiKey_Slash;
			default: break;
		}
		return ImGuiKey_None;
	}

	void updateKeyModifiers(SDL_Keymod sdlKeyMods)
	{
		ImGuiIO &io = ImGui::GetIO();
		io.AddKeyEvent(ImGuiMod_Ctrl, (sdlKeyMods & KMOD_CTRL) != 0);
		io.AddKeyEvent(ImGuiMod_Shift, (sdlKeyMods & KMOD_SHIFT) != 0);
		io.AddKeyEvent(ImGuiMod_Alt, (sdlKeyMods & KMOD_ALT) != 0);
		io.AddKeyEvent(ImGuiMod_Super, (sdlKeyMods & KMOD_GUI) != 0);
	}

	void updateGamepadButton(ImVector<SDL_GameController *> &gamepads, ImGuiIO &io, ImGuiKey key, SDL_GameControllerButton buttonNo)
	{
		bool mergedValue = false;
		for (SDL_GameController *gamepad : gamepads)
			mergedValue |= SDL_GameControllerGetButton(gamepad, buttonNo) != 0;
		io.AddKeyEvent(key, mergedValue);
	}

	float saturate(float v) { return v < 0.0f ? 0.0f : v > 1.0f ? 1.0f : v; }
	void updateGamepadAnalog(ImVector<SDL_GameController*> &gamepads, ImGuiIO& io, ImGuiKey key, SDL_GameControllerAxis axisNo, float v0, float v1)
	{
		float mergedValue = 0.0f;
		for (SDL_GameController *gamepad : gamepads)
		{
			const float vn = saturate(static_cast<float>(SDL_GameControllerGetAxis(gamepad, axisNo) - v0) / static_cast<float>(v1 - v0));
			if (mergedValue < vn)
				mergedValue = vn;
		}
		io.AddKeyAnalogEvent(key, mergedValue > 0.1f, mergedValue);
	}

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool ImGuiSdlInput::inputEnabled_ = true;

SDL_Window *ImGuiSdlInput::window_ = nullptr;
unsigned long int ImGuiSdlInput::time_ = 0;
char *ImGuiSdlInput::clipboardTextData_ = nullptr;

unsigned int ImGuiSdlInput::mouseWindowID_ = 0;
int ImGuiSdlInput::mouseButtonsDown_ = 0;
SDL_Cursor *ImGuiSdlInput::mouseCursors_[ImGuiMouseCursor_COUNT] = {};
SDL_Cursor *ImGuiSdlInput::mouseLastCursor_ = nullptr;
unsigned int ImGuiSdlInput::mouseLastLeaveFrame_ = 0;
bool ImGuiSdlInput::mouseCanUseGlobalState_ = false;

ImVector<SDL_GameController *> ImGuiSdlInput::gamepads_;
ImGuiSdlInput::GamepadMode ImGuiSdlInput::gamepadMode_ = ImGuiSdlInput::GamepadMode::AUTO_FIRST;
bool ImGuiSdlInput::wantUpdateGamepadsList_ = false;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifdef __EMSCRIPTEN__
EM_JS(void, emscriptenOpenURL, (char const *url), { url = url ? UTF8ToString(url) : null; if (url) window.open(url, '_blank'); });
#endif

void ImGuiSdlInput::init(SDL_Window *window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();

	// Check and store if we are on a SDL backend that supports global mouse position
	// ("wayland" and "rpi" don't support it, but we chose to use a white-list instead of a black-list)
	mouseCanUseGlobalState_ = false;
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
	const char *sdlBackend = SDL_GetCurrentVideoDriver();
	const char *globalMouseWhitelist[] = { "windows", "cocoa", "x11", "DIVE", "VMAN" };
	for (int n = 0; n < IM_ARRAYSIZE(globalMouseWhitelist); n++)
	{
		if (strncmp(sdlBackend, globalMouseWhitelist[n], strlen(globalMouseWhitelist[n])) == 0)
		{
			mouseCanUseGlobalState_ = true;
			break;
		}
	}
#endif

	// Setup backend capabilities flags
	io.BackendPlatformName = "nCine_SDL2";
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)

	window_ = window;

	ImGuiPlatformIO& platformIo = ImGui::GetPlatformIO();
	platformIo.Platform_SetClipboardTextFn = setClipboardText;
	platformIo.Platform_GetClipboardTextFn = clipboardText;
	platformIo.Platform_ClipboardUserData = nullptr;
	platformIo.Platform_SetImeDataFn = setPlatformImeData;
#ifdef __EMSCRIPTEN__
	platformIo.Platform_OpenInShellFn = [](ImGuiContext*, const char *url) { emscriptenOpenURL(url); return true; };
#elif SDL_HAS_OPEN_URL
	platformIo.Platform_OpenInShellFn = [](ImGuiContext*, const char *url) { return SDL_OpenURL(url) == 0; };
#endif

	// Gamepad handling
	gamepadMode_ = GamepadMode::AUTO_FIRST;
	wantUpdateGamepadsList_ = true;

	// Load mouse cursors
	mouseCursors_[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	mouseCursors_[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	mouseCursors_[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	mouseCursors_[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	mouseCursors_[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	mouseCursors_[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
	mouseCursors_[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
	mouseCursors_[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	mouseCursors_[ImGuiMouseCursor_Wait] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
	mouseCursors_[ImGuiMouseCursor_Progress] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
	mouseCursors_[ImGuiMouseCursor_NotAllowed] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);

	// Set platform dependent data in viewport
	// Our mouse update function expect PlatformHandle to be filled for the main viewport
	ImGuiViewport *mainViewport = ImGui::GetMainViewport();
	mainViewport->PlatformHandleRaw = nullptr;
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(window, &info))
	{
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
		mainViewport->PlatformHandleRaw = (void *)info.info.win.window;
#elif defined(__APPLE__) && defined(SDL_VIDEO_DRIVER_COCOA)
		mainViewport->PlatformHandleRaw = (void *)info.info.cocoa.window;
#endif
	}

	// From 2.0.5: Set SDL hint to receive mouse click events on window focus, otherwise SDL doesn't emit the event.
	// Without this, when clicking to gain focus, our widgets wouldn't activate even though they showed as hovered.
	// (This is unfortunately a global SDL setting, so enabling it might have a side-effect on your application.
	// It is unlikely to make a difference, but if your app absolutely needs to ignore the initial on-focus click:
	// you can ignore SDL_MOUSEBUTTONDOWN events coming right after a SDL_WINDOWEVENT_FOCUS_GAINED)
#ifdef SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif

	// From 2.0.18: Enable native IME.
	// IMPORTANT: This is used at the time of SDL_CreateWindow() so this will only affects secondary windows, if any.
	// For the main window to be affected, your application needs to call this manually before calling SDL_CreateWindow().
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	// From 2.0.22: Disable auto-capture, this is preventing drag and drop across multiple windows (see #5710)
#ifdef SDL_HINT_MOUSE_AUTO_CAPTURE
	SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "0");
#endif
}

void ImGuiSdlInput::shutdown()
{
	window_ = nullptr;

	// Destroy last known clipboard data
	if (clipboardTextData_)
		SDL_free(clipboardTextData_);
	clipboardTextData_ = nullptr;

	// Destroy SDL mouse cursors
	for (ImGuiMouseCursor i = 0; i < ImGuiMouseCursor_COUNT; i++)
		SDL_FreeCursor(mouseCursors_[i]);
	closeGamepads();

	ImGuiIO &io = ImGui::GetIO();
	io.BackendPlatformName = nullptr;
	io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad);

	ImGui::DestroyContext();
}

void ImGuiSdlInput::newFrame()
{
	ImGuiIO &io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! Missing call to ImGuiDrawing::buildFonts() function?");

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int displayW, displayH;
	SDL_GetWindowSize(window_, &w, &h);
	if (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED)
		w = h = 0;
#if SDL_HAS_VULKAN
	else if (SDL_GetWindowFlags(window_) & SDL_WINDOW_VULKAN)
		SDL_Vulkan_GetDrawableSize(window_, &displayW, &displayH);
	else
#endif
	SDL_GL_GetDrawableSize(window_, &displayW, &displayH);
	io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
	if (w > 0 && h > 0)
		io.DisplayFramebufferScale = ImVec2(static_cast<float>(displayW) / w, static_cast<float>(displayH) / h);

	// Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
	// (Accept SDL_GetPerformanceCounter() not returning a monotonically increasing value. Happens in VMs and Emscripten, see #6189, #6114, #3644)
	static Uint64 frequency = SDL_GetPerformanceFrequency();
	Uint64 currentTime = SDL_GetPerformanceCounter();
	if (currentTime <= time_)
		currentTime = time_ + 1;
	io.DeltaTime = time_ > 0 ? static_cast<float>((static_cast<double>(currentTime - time_) / frequency)) : static_cast<float>(1.0f / 60.0f);
	time_ = currentTime;

	if (mouseLastLeaveFrame_ && mouseLastLeaveFrame_ >= ImGui::GetFrameCount() && mouseButtonsDown_ == 0)
	{
		mouseWindowID_ = 0;
		mouseLastLeaveFrame_ = 0;
		io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
	}

	updateMouseData();
	updateMouseCursor();

	// Update game controllers (if enabled and available)
	updateGamepads();
}

// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
// If you have multiple SDL events and some of them are not meant to be used by dear imgui, you may need to filter events based on their windowID field.
bool ImGuiSdlInput::processEvent(const SDL_Event *event)
{
	if (inputEnabled_ == false)
		return false;

	ImGuiIO &io = ImGui::GetIO();
	switch (event->type)
	{
		case SDL_MOUSEMOTION:
		{
			if (viewportForWindowId(window_, event->motion.windowID) == nullptr)
				return false;
			const ImVec2 mousePos(static_cast<float>(event->motion.x), static_cast<float>(event->motion.y));
			io.AddMouseSourceEvent(event->motion.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
			io.AddMousePosEvent(mousePos.x, mousePos.y);
			return true;
		}
		case SDL_MOUSEWHEEL:
		{
			if (viewportForWindowId(window_, event->motion.windowID) == nullptr)
				return false;
#if SDL_VERSION_ATLEAST(2, 0, 18) // If this fails to compile on Emscripten: update to latest Emscripten!
			float wheelX = -event->wheel.preciseX;
			const float wheelY = event->wheel.preciseY;
#else
			float wheelX = static_cast<float>(-event->wheel.x);
			const float wheelY = static_cast<float>(event->wheel.y);
#endif
#if defined(__EMSCRIPTEN__) && !SDL_VERSION_ATLEAST(2,31,0)
			wheelX /= 100.0f;
#endif
			io.AddMouseSourceEvent(event->wheel.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
			io.AddMouseWheelEvent(wheelX, wheelY);
			return true;
		}
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			if (viewportForWindowId(window_, event->motion.windowID) == nullptr)
				return false;
			int mouseButton = -1;
			if (event->button.button == SDL_BUTTON_LEFT)
				mouseButton = 0;
			else if (event->button.button == SDL_BUTTON_RIGHT)
				mouseButton = 1;
			else if (event->button.button == SDL_BUTTON_MIDDLE)
				mouseButton = 2;
			else if (event->button.button == SDL_BUTTON_X1)
				mouseButton = 3;
			else if (event->button.button == SDL_BUTTON_X2)
				mouseButton = 4;
			if (mouseButton == -1)
				break;
			io.AddMouseSourceEvent(event->wheel.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
			io.AddMouseButtonEvent(mouseButton, (event->type == SDL_MOUSEBUTTONDOWN));
			mouseButtonsDown_ = (event->type == SDL_MOUSEBUTTONDOWN) ? (mouseButtonsDown_ | (1 << mouseButton)) : (mouseButtonsDown_ & ~(1 << mouseButton));
			return true;
		}
		case SDL_TEXTINPUT:
		{
			if (viewportForWindowId(window_, event->motion.windowID) == nullptr)
				return false;
			io.AddInputCharactersUTF8(event->text.text);
			return true;
		}
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			if (viewportForWindowId(window_, event->motion.windowID) == nullptr)
				return false;
			updateKeyModifiers(static_cast<SDL_Keymod>(event->key.keysym.mod));
			const ImGuiKey key = sdlKeycodeToImGuiKey(event->key.keysym.sym, event->key.keysym.scancode);
			io.AddKeyEvent(key, (event->type == SDL_KEYDOWN));
			// To support legacy indexing (<1.87 user code). Legacy backend uses SDLK_*** as indices to IsKeyXXX() functions.
			io.SetKeyEventNativeData(key, event->key.keysym.sym, event->key.keysym.scancode, event->key.keysym.scancode);
			return true;
		}
		case SDL_WINDOWEVENT:
		{
			if (viewportForWindowId(window_, event->motion.windowID) == nullptr)
				return false;
			// - When capturing mouse, SDL will send a bunch of conflicting LEAVE/ENTER event on every mouse move, but the final ENTER tends to be right.
			// - However we won't get a correct LEAVE event for a captured window.
			// - In some cases, when detaching a window from main viewport SDL may send SDL_WINDOWEVENT_ENTER one frame too late,
			//   causing SDL_WINDOWEVENT_LEAVE on previous frame to interrupt drag operation by clear mouse position. This is why
			//   we delay process the SDL_WINDOWEVENT_LEAVE events by one frame. See issue #5012 for details.
			const Uint8 windowEvent = event->window.event;
			if (windowEvent == SDL_WINDOWEVENT_ENTER)
			{
				mouseWindowID_ = event->window.windowID;
				mouseLastLeaveFrame_ = 0;
			}
			if (windowEvent == SDL_WINDOWEVENT_LEAVE)
				mouseLastLeaveFrame_ = ImGui::GetFrameCount() + 1;
			if (windowEvent == SDL_WINDOWEVENT_FOCUS_GAINED)
				io.AddFocusEvent(true);
			else if (event->window.event == SDL_WINDOWEVENT_FOCUS_LOST)
				io.AddFocusEvent(false);
			return true;
		}
		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
		{
			wantUpdateGamepadsList_ = true;
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

const char *ImGuiSdlInput::clipboardText(ImGuiContext *context)
{
	if (clipboardTextData_)
		SDL_free(clipboardTextData_);
	clipboardTextData_ = SDL_GetClipboardText();
	return clipboardTextData_;
}

void ImGuiSdlInput::updateMouseData()
{
	ImGuiIO &io = ImGui::GetIO();

	// We forward mouse input when hovered or captured (via SDL_MOUSEMOTION) or when focused (below)
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
	// - SDL_CaptureMouse() let the OS know e.g. that our drags can extend outside of parent boundaries (we want updated position) and shouldn't trigger other operations outside.
	// - Debuggers under Linux tends to leave captured mouse on break, which may be very inconvenient, so to migitate the issue we wait until mouse has moved to begin capture.
	bool wantCapture = false;
	for (int buttonN = 0; buttonN < ImGuiMouseButton_COUNT && !wantCapture; buttonN++)
		if (ImGui::IsMouseDragging(buttonN, 1.0f))
			wantCapture = true;
	SDL_CaptureMouse(wantCapture ? SDL_TRUE : SDL_FALSE);

	SDL_Window *focusedWindow = SDL_GetKeyboardFocus();
	const bool isAppFocused = (window_ == focusedWindow);
#else
	const bool isAppFocused = (SDL_GetWindowFlags(window_) & SDL_WINDOW_INPUT_FOCUS) != 0; // SDL 2.0.3 and non-windowed systems: single-viewport only
#endif

	if (isAppFocused)
	{
		// (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when io.ConfigNavMoveSetMousePos is enabled by user)
		if (io.WantSetMousePos)
			SDL_WarpMouseInWindow(window_, static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));

		// (Optional) Fallback to provide mouse position when focused (SDL_MOUSEMOTION already provides this when hovered or captured)
		const bool isRelativeMouseMode = SDL_GetRelativeMouseMode() != 0;
		if (mouseCanUseGlobalState_ && mouseButtonsDown_ == 0 && !isRelativeMouseMode)
		{
			// Single-viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
			int windowX, windowY, mouseXGlobal, mouseYGlobal;
			SDL_GetGlobalMouseState(&mouseXGlobal, &mouseYGlobal);
			SDL_GetWindowPosition(window_, &windowX, &windowY);
			io.AddMousePosEvent(static_cast<float>(mouseXGlobal - windowX), static_cast<float>(mouseYGlobal - windowY));
		}
	}
}

void ImGuiSdlInput::updateMouseCursor()
{
	ImGuiIO &io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return;

	ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
	if (io.MouseDrawCursor || imguiCursor == ImGuiMouseCursor_None)
	{
		// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
		SDL_ShowCursor(SDL_FALSE);
	}
	else
	{
		// Show OS mouse cursor
		SDL_Cursor *expectedCursor = mouseCursors_[imguiCursor] ? mouseCursors_[imguiCursor] : mouseCursors_[ImGuiMouseCursor_Arrow];
		if (mouseLastCursor_ != expectedCursor)
		{
			SDL_SetCursor(expectedCursor); // SDL function doesn't have an early out (see #6113)
			mouseLastCursor_ = expectedCursor;
		}
	}
}

void ImGuiSdlInput::closeGamepads()
{
	if (gamepadMode_ != GamepadMode::MANUAL)
	{
		for (SDL_GameController *gamepad : gamepads_)
			SDL_GameControllerClose(gamepad);
		gamepads_.resize(0);
	}
}

void ImGuiSdlInput::setGamepadMode(GamepadMode mode, SDL_GameController **manualGamepadsArray, unsigned int manualGamepadsCount)
{
	closeGamepads();
	if (mode == GamepadMode::MANUAL)
	{
		IM_ASSERT(manualGamepadsArray != nullptr || manualGamepadsCount <= 0);
		for (unsigned int n = 0; n < manualGamepadsCount; n++)
			gamepads_.push_back(manualGamepadsArray[n]);
	}
	else
	{
		IM_ASSERT(manualGamepadsArray == nullptr && manualGamepadsCount <= 0);
		wantUpdateGamepadsList_ = true;
	}
	gamepadMode_ = mode;
}

void ImGuiSdlInput::updateGamepads()
{
	ImGuiIO &io = ImGui::GetIO();

	// Update list of controller(s) to use
	if (wantUpdateGamepadsList_ && gamepadMode_ != GamepadMode::MANUAL)
	{
		closeGamepads();
		const int joystickCount = SDL_NumJoysticks();
		for (int n = 0; n < joystickCount; n++)
		{
			if (SDL_IsGameController(n))
			{
				if (SDL_GameController *gamepad = SDL_GameControllerOpen(n))
				{
					gamepads_.push_back(gamepad);
					if (gamepadMode_ == GamepadMode::AUTO_FIRST)
						break;
				}
			}
		}
		wantUpdateGamepadsList_ = false;
	}

	// FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs.
	if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
		return;
	io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
	if (gamepads_.Size == 0)
		return;
	io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

	// No need to call `imGuiJoyMappedInput()` as the backend is using `SDL_GameController`

	// Update gamepad inputs
	// clang-format off
	const int thumbDeadZone = 8000; // SDL_gamecontroller.h suggests using this value.
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadStart,       SDL_CONTROLLER_BUTTON_START);
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadBack,        SDL_CONTROLLER_BUTTON_BACK);
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadFaceLeft,    SDL_CONTROLLER_BUTTON_X);              // Xbox X, PS Square
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadFaceRight,   SDL_CONTROLLER_BUTTON_B);              // Xbox B, PS Circle
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadFaceUp,      SDL_CONTROLLER_BUTTON_Y);              // Xbox Y, PS Triangle
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadFaceDown,    SDL_CONTROLLER_BUTTON_A);              // Xbox A, PS Cross
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadDpadLeft,    SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadDpadRight,   SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadDpadUp,      SDL_CONTROLLER_BUTTON_DPAD_UP);
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadDpadDown,    SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadL1,          SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadR1,          SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	updateGamepadAnalog(gamepads_, io, ImGuiKey_GamepadL2,          SDL_CONTROLLER_AXIS_TRIGGERLEFT,  0.0f, 32767);
	updateGamepadAnalog(gamepads_, io, ImGuiKey_GamepadR2,          SDL_CONTROLLER_AXIS_TRIGGERRIGHT, 0.0f, 32767);
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadL3,          SDL_CONTROLLER_BUTTON_LEFTSTICK);
	updateGamepadButton(gamepads_, io, ImGuiKey_GamepadR3,          SDL_CONTROLLER_BUTTON_RIGHTSTICK);
	updateGamepadAnalog(gamepads_, io, ImGuiKey_GamepadLStickLeft,  SDL_CONTROLLER_AXIS_LEFTX,  -thumbDeadZone, -32768);
	updateGamepadAnalog(gamepads_, io, ImGuiKey_GamepadLStickRight, SDL_CONTROLLER_AXIS_LEFTX,  +thumbDeadZone, +32767);
	updateGamepadAnalog(gamepads_, io, ImGuiKey_GamepadLStickUp,    SDL_CONTROLLER_AXIS_LEFTY,  -thumbDeadZone, -32768);
	updateGamepadAnalog(gamepads_, io, ImGuiKey_GamepadLStickDown,  SDL_CONTROLLER_AXIS_LEFTY,  +thumbDeadZone, +32767);
	updateGamepadAnalog(gamepads_, io, ImGuiKey_GamepadRStickLeft,  SDL_CONTROLLER_AXIS_RIGHTX, -thumbDeadZone, -32768);
	updateGamepadAnalog(gamepads_, io, ImGuiKey_GamepadRStickRight, SDL_CONTROLLER_AXIS_RIGHTX, +thumbDeadZone, +32767);
	updateGamepadAnalog(gamepads_, io, ImGuiKey_GamepadRStickUp,    SDL_CONTROLLER_AXIS_RIGHTY, -thumbDeadZone, -32768);
	updateGamepadAnalog(gamepads_, io, ImGuiKey_GamepadRStickDown,  SDL_CONTROLLER_AXIS_RIGHTY, +thumbDeadZone, +32767);
	// clang-format on
}

}
