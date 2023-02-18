// Based on imgui/backends/imgui_impl_sdl.cpp

#include "ImGuiSdlInput.h"
#include "ImGuiJoyMappedInput.h"

// SDL
#include <SDL.h>
#include <SDL_syswm.h>
#if defined(__APPLE__)
	#include "TargetConditionals.h"
#endif

#if SDL_VERSION_ATLEAST(2, 0, 4) && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !(defined(__APPLE__) && TARGET_OS_IOS)
	#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE 1
#else
	#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE 0
#endif
#define SDL_HAS_VULKAN SDL_VERSION_ATLEAST(2, 0, 6)

namespace ncine {

namespace {

	void setClipboardText(void *userData, const char *text)
	{
		SDL_SetClipboardText(text);
	}

	// Note: native IME will only display if user calls SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1") _before_ SDL_CreateWindow().
	void setPlatformImeData(ImGuiViewport *viewport, ImGuiPlatformImeData *data)
	{
		if (data->WantVisible)
		{
			SDL_Rect r;
			r.x = static_cast<int>(data->InputPos.x);
			r.y = static_cast<int>(data->InputPos.y);
			r.w = 1;
			r.h = static_cast<int>(data->InputLineHeight);
			SDL_SetTextInputRect(&r);
			SDL_StartTextInput();
		}
		else
			SDL_StopTextInput();
	}

	ImGuiKey sdlKeycodeToImGuiKey(int keycode)
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
			case SDLK_QUOTE: return ImGuiKey_Apostrophe;
			case SDLK_COMMA: return ImGuiKey_Comma;
			case SDLK_MINUS: return ImGuiKey_Minus;
			case SDLK_PERIOD: return ImGuiKey_Period;
			case SDLK_SLASH: return ImGuiKey_Slash;
			case SDLK_SEMICOLON: return ImGuiKey_Semicolon;
			case SDLK_EQUALS: return ImGuiKey_Equal;
			case SDLK_LEFTBRACKET: return ImGuiKey_LeftBracket;
			case SDLK_BACKSLASH: return ImGuiKey_Backslash;
			case SDLK_RIGHTBRACKET: return ImGuiKey_RightBracket;
			case SDLK_BACKQUOTE: return ImGuiKey_GraveAccent;
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

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool ImGuiSdlInput::inputEnabled_ = true;
bool ImGuiSdlInput::mouseCanUseGlobalState_ = false;
unsigned int ImGuiSdlInput::pendingMouseLeaveFrame_ = 0;
SDL_Window *ImGuiSdlInput::window_ = nullptr;
unsigned long int ImGuiSdlInput::time_ = 0;
unsigned int ImGuiSdlInput::mouseWindowID_ = 0;
int ImGuiSdlInput::mouseButtonsDown_ = 0;
SDL_Cursor *ImGuiSdlInput::mouseCursors_[ImGuiMouseCursor_COUNT] = {};
SDL_Cursor *ImGuiSdlInput::lastMouseCursor_ = nullptr;
char *ImGuiSdlInput::clipboardTextData_ = nullptr;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

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

	io.SetClipboardTextFn = setClipboardText;
	io.GetClipboardTextFn = clipboardText;
	io.ClipboardUserData = nullptr;

	// Load mouse cursors
	mouseCursors_[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	mouseCursors_[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	mouseCursors_[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	mouseCursors_[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	mouseCursors_[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	mouseCursors_[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
	mouseCursors_[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
	mouseCursors_[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
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
	lastMouseCursor_ = nullptr;

	ImGuiIO &io = ImGui::GetIO();
	io.BackendPlatformName = nullptr;

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
	SDL_GL_GetDrawableSize(window_, &displayW, &displayH);
	io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
	if (w > 0 && h > 0)
		io.DisplayFramebufferScale = ImVec2(static_cast<float>(displayW) / w, static_cast<float>(displayH) / h);

	// Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
	static Uint64 frequency = SDL_GetPerformanceFrequency();
	const Uint64 currentTime = SDL_GetPerformanceCounter();
	io.DeltaTime = time_ > 0 ? static_cast<float>((static_cast<double>(currentTime - time_) / frequency)) : static_cast<float>(1.0f / 60.0f);
	time_ = currentTime;

	if (pendingMouseLeaveFrame_ && pendingMouseLeaveFrame_ >= ImGui::GetFrameCount() && mouseButtonsDown_ == 0)
	{
		mouseWindowID_ = 0;
		pendingMouseLeaveFrame_ = 0;
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
			const ImVec2 mousePos(static_cast<float>(event->motion.x), static_cast<float>(event->motion.y));
			io.AddMousePosEvent(mousePos.x, mousePos.y);
			return true;
		}
		case SDL_MOUSEWHEEL:
		{
#if SDL_VERSION_ATLEAST(2, 0, 18) // If this fails to compile on Emscripten: update to latest Emscripten!
			float wheelX = -event->wheel.preciseX;
			const float wheelY = event->wheel.preciseY;
#else
			float wheelX = static_cast<float>(-event->wheel.x);
			const float wheelY = static_cast<float>(event->wheel.y);
#endif
#ifdef __EMSCRIPTEN__
			wheelX /= 100.0f;
#endif
			io.AddMouseWheelEvent(wheelX, wheelY);
			return true;
		}
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
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
			io.AddMouseButtonEvent(mouseButton, (event->type == SDL_MOUSEBUTTONDOWN));
			mouseButtonsDown_ = (event->type == SDL_MOUSEBUTTONDOWN) ? (mouseButtonsDown_ | (1 << mouseButton)) : (mouseButtonsDown_ & ~(1 << mouseButton));
			return true;
		}
		case SDL_TEXTINPUT:
		{
			io.AddInputCharactersUTF8(event->text.text);
			return true;
		}
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			updateKeyModifiers(static_cast<SDL_Keymod>(event->key.keysym.mod));
			const ImGuiKey key = sdlKeycodeToImGuiKey(event->key.keysym.sym);
			io.AddKeyEvent(key, (event->type == SDL_KEYDOWN));
			// To support legacy indexing (<1.87 user code). Legacy backend uses SDLK_*** as indices to IsKeyXXX() functions.
			io.SetKeyEventNativeData(key, event->key.keysym.sym, event->key.keysym.scancode, event->key.keysym.scancode);
			return true;
		}
		case SDL_WINDOWEVENT:
		{
			// - When capturing mouse, SDL will send a bunch of conflicting LEAVE/ENTER event on every mouse move, but the final ENTER tends to be right.
			// - However we won't get a correct LEAVE event for a captured window.
			// - In some cases, when detaching a window from main viewport SDL may send SDL_WINDOWEVENT_ENTER one frame too late,
			//   causing SDL_WINDOWEVENT_LEAVE on previous frame to interrupt drag operation by clear mouse position. This is why
			//   we delay process the SDL_WINDOWEVENT_LEAVE events by one frame. See issue #5012 for details.
			const Uint8 windowEvent = event->window.event;
			if (windowEvent == SDL_WINDOWEVENT_ENTER)
			{
				mouseWindowID_ = event->window.windowID;
				pendingMouseLeaveFrame_ = 0;
			}
			if (windowEvent == SDL_WINDOWEVENT_LEAVE)
				pendingMouseLeaveFrame_ = ImGui::GetFrameCount() + 1;
			if (windowEvent == SDL_WINDOWEVENT_FOCUS_GAINED)
				io.AddFocusEvent(true);
			else if (event->window.event == SDL_WINDOWEVENT_FOCUS_LOST)
				io.AddFocusEvent(false);
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

const char *ImGuiSdlInput::clipboardText(void *userData)
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
	// SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger other operations outside
	SDL_CaptureMouse((mouseButtonsDown_ != 0) ? SDL_TRUE : SDL_FALSE);
	SDL_Window *focusedWindow = SDL_GetKeyboardFocus();
	const bool isAppFocused = (window_ == focusedWindow);
#else
	const bool isAppFocused = (SDL_GetWindowFlags(window_) & SDL_WINDOW_INPUT_FOCUS) != 0; // SDL 2.0.3 and non-windowed systems: single-viewport only
#endif

	if (isAppFocused)
	{
		// (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
		if (io.WantSetMousePos)
			SDL_WarpMouseInWindow(window_, static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));

		// (Optional) Fallback to provide mouse position when focused (SDL_MOUSEMOTION already provides this when hovered or captured)
		if (mouseCanUseGlobalState_ && mouseButtonsDown_ == 0)
		{
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
		SDL_Cursor *expectedCursor = mouseCursors_[imguiCursor] ?mouseCursors_[imguiCursor] : mouseCursors_[ImGuiMouseCursor_Arrow];
		if (lastMouseCursor_ != expectedCursor)
		{
			SDL_SetCursor(expectedCursor); // SDL function doesn't have an early out (see #6113)
			lastMouseCursor_ = expectedCursor;
		}
	}
}

void ImGuiSdlInput::updateGamepads()
{
	ImGuiIO &io = ImGui::GetIO();
	if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0) // FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs.
		return;

	const bool joyMappedInput = imGuiJoyMappedInput();

	if (joyMappedInput == false)
	{
		// Get gamepad
		io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
		SDL_GameController *gameController = SDL_GameControllerOpen(0);
		if (!gameController)
			return;
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

		// Update gamepad inputs
		// clang-format off
		#define IM_SATURATE(V)                      (V < 0.0f ? 0.0f : V > 1.0f ? 1.0f : V)
		#define MAP_BUTTON(KEY_NO, BUTTON_NO)       { io.AddKeyEvent(KEY_NO, SDL_GameControllerGetButton(gameController, BUTTON_NO) != 0); }
		#define MAP_ANALOG(KEY_NO, AXIS_NO, V0, V1) { float vn = static_cast<float>(SDL_GameControllerGetAxis(gameController, AXIS_NO) - V0) / static_cast<float>(V1 - V0); vn = IM_SATURATE(vn); io.AddKeyAnalogEvent(KEY_NO, vn > 0.1f, vn); }
		const int thumbDeadZone = 8000; // SDL_gamecontroller.h suggests using this value.
		MAP_BUTTON(ImGuiKey_GamepadStart,           SDL_CONTROLLER_BUTTON_START);
		MAP_BUTTON(ImGuiKey_GamepadBack,            SDL_CONTROLLER_BUTTON_BACK);
		MAP_BUTTON(ImGuiKey_GamepadFaceLeft,        SDL_CONTROLLER_BUTTON_X);              // Xbox X, PS Square
		MAP_BUTTON(ImGuiKey_GamepadFaceRight,       SDL_CONTROLLER_BUTTON_B);              // Xbox B, PS Circle
		MAP_BUTTON(ImGuiKey_GamepadFaceUp,          SDL_CONTROLLER_BUTTON_Y);              // Xbox Y, PS Triangle
		MAP_BUTTON(ImGuiKey_GamepadFaceDown,        SDL_CONTROLLER_BUTTON_A);              // Xbox A, PS Cross
		MAP_BUTTON(ImGuiKey_GamepadDpadLeft,        SDL_CONTROLLER_BUTTON_DPAD_LEFT);
		MAP_BUTTON(ImGuiKey_GamepadDpadRight,       SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
		MAP_BUTTON(ImGuiKey_GamepadDpadUp,          SDL_CONTROLLER_BUTTON_DPAD_UP);
		MAP_BUTTON(ImGuiKey_GamepadDpadDown,        SDL_CONTROLLER_BUTTON_DPAD_DOWN);
		MAP_BUTTON(ImGuiKey_GamepadL1,              SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
		MAP_BUTTON(ImGuiKey_GamepadR1,              SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
		MAP_ANALOG(ImGuiKey_GamepadL2,              SDL_CONTROLLER_AXIS_TRIGGERLEFT,  0.0f, 32767);
		MAP_ANALOG(ImGuiKey_GamepadR2,              SDL_CONTROLLER_AXIS_TRIGGERRIGHT, 0.0f, 32767);
		MAP_BUTTON(ImGuiKey_GamepadL3,              SDL_CONTROLLER_BUTTON_LEFTSTICK);
		MAP_BUTTON(ImGuiKey_GamepadR3,              SDL_CONTROLLER_BUTTON_RIGHTSTICK);
		MAP_ANALOG(ImGuiKey_GamepadLStickLeft,      SDL_CONTROLLER_AXIS_LEFTX,  -thumbDeadZone, -32768);
		MAP_ANALOG(ImGuiKey_GamepadLStickRight,     SDL_CONTROLLER_AXIS_LEFTX,  +thumbDeadZone, +32767);
		MAP_ANALOG(ImGuiKey_GamepadLStickUp,        SDL_CONTROLLER_AXIS_LEFTY,  -thumbDeadZone, -32768);
		MAP_ANALOG(ImGuiKey_GamepadLStickDown,      SDL_CONTROLLER_AXIS_LEFTY,  +thumbDeadZone, +32767);
		MAP_ANALOG(ImGuiKey_GamepadRStickLeft,      SDL_CONTROLLER_AXIS_RIGHTX, -thumbDeadZone, -32768);
		MAP_ANALOG(ImGuiKey_GamepadRStickRight,     SDL_CONTROLLER_AXIS_RIGHTX, +thumbDeadZone, +32767);
		MAP_ANALOG(ImGuiKey_GamepadRStickUp,        SDL_CONTROLLER_AXIS_RIGHTY, -thumbDeadZone, -32768);
		MAP_ANALOG(ImGuiKey_GamepadRStickDown,      SDL_CONTROLLER_AXIS_RIGHTY, +thumbDeadZone, +32767);
		#undef MAP_BUTTON
		#undef MAP_ANALOG
		// clang-format on
	}
}

}
