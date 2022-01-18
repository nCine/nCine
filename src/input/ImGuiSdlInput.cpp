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
#define SDL_HAS_MOUSE_FOCUS_CLICKTHROUGH SDL_VERSION_ATLEAST(2, 0, 5)
#define SDL_HAS_VULKAN SDL_VERSION_ATLEAST(2, 0, 6)

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool ImGuiSdlInput::inputEnabled_ = true;
bool ImGuiSdlInput::mouseCanUseGlobalState_ = false;
SDL_Window *ImGuiSdlInput::window_ = nullptr;
double ImGuiSdlInput::time_ = 0.0;
bool ImGuiSdlInput::mousePressed_[3] = { false, false, false };
SDL_Cursor *ImGuiSdlInput::mouseCursors_[ImGuiMouseCursor_COUNT] = {};
char *ImGuiSdlInput::clipboardTextData_ = nullptr;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const char *ImGuiSdlInput::clipboardText(void *userData)
{
	if (clipboardTextData_)
		SDL_free(clipboardTextData_);
	clipboardTextData_ = SDL_GetClipboardText();
	return clipboardTextData_;
}

namespace {

	void setClipboardText(void *userData, const char *text)
	{
		SDL_SetClipboardText(text);
	}

}

void ImGuiSdlInput::init(SDL_Window *window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();

	// Check and store if we are on a SDL backend that supports global mouse position
	// ("wayland" and "rpi" don't support it, but we chose to use a white-list instead of a black-list)
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
	const char *sdlBackend = SDL_GetCurrentVideoDriver();
	const char *globalMouseWhitelist[] = { "windows", "cocoa", "x11", "DIVE", "VMAN" };
	mouseCanUseGlobalState_ = false;
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
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendPlatformName = "nCine_SDL";

	window_ = window;

	// Keyboard mapping. Dear ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
#if IMGUI_VERSION_NUM > 18600
	io.KeyMap[ImGuiKey_KeypadEnter] = SDL_SCANCODE_KP_ENTER;
#else
	io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
#endif
	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

	io.SetClipboardTextFn = setClipboardText;
	io.GetClipboardTextFn = clipboardText;
	io.ClipboardUserData = nullptr;

	mouseCursors_[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	mouseCursors_[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	mouseCursors_[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	mouseCursors_[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	mouseCursors_[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	mouseCursors_[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
	mouseCursors_[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
	mouseCursors_[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	mouseCursors_[ImGuiMouseCursor_NotAllowed] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);

#ifdef IMGUI_HAS_DOCK
	// Our mouse update function expect PlatformHandle to be filled for the main viewport
	ImGuiViewport *main_viewport = ImGui::GetMainViewport();
	main_viewport->PlatformHandle = reinterpret_cast<void *>(window);
#endif

#ifdef _WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	#ifdef IMGUI_HAS_DOCK
	main_viewport->PlatformHandleRaw = wmInfo.info.win.window;
	#else
	io.ImeWindowHandle = wmInfo.info.win.window;
	#endif
#else
	(void)window;
#endif

#if SDL_HAS_MOUSE_FOCUS_CLICKTHROUGH
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
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
	memset(mouseCursors_, 0, sizeof(mouseCursors_));

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
	Uint64 currentTime = SDL_GetPerformanceCounter();
	io.DeltaTime = time_ > 0 ? static_cast<float>((static_cast<double>(currentTime - time_) / frequency)) : static_cast<float>(1.0f / 60.0f);
	time_ = currentTime;

	updateMousePosAndButtons();
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
		case SDL_MOUSEWHEEL:
		{
			if (event->wheel.x > 0)
				io.MouseWheelH += 1;
			if (event->wheel.x < 0)
				io.MouseWheelH -= 1;
			if (event->wheel.y > 0)
				io.MouseWheel += 1;
			if (event->wheel.y < 0)
				io.MouseWheel -= 1;
			return true;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			if (event->button.button == SDL_BUTTON_LEFT)
				mousePressed_[0] = true;
			if (event->button.button == SDL_BUTTON_RIGHT)
				mousePressed_[1] = true;
			if (event->button.button == SDL_BUTTON_MIDDLE)
				mousePressed_[2] = true;
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
			int key = event->key.keysym.scancode;
			IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
			io.KeysDown[key] = (event->type == SDL_KEYDOWN);
			io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
			io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
#ifdef _WIN32
			io.KeySuper = false;
#else
			io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
#endif
			return true;
		}
		case SDL_WINDOWEVENT:
		{
			if (event->window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
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

void ImGuiSdlInput::updateMousePosAndButtons()
{
	ImGuiIO &io = ImGui::GetIO();

	const ImVec2 mousePosPrev = io.MousePos;
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

	// Update mouse buttons
	int mouseXLocal, mouseYLocal;
	const Uint32 mouseButtons = SDL_GetMouseState(&mouseXLocal, &mouseYLocal);
	io.MouseDown[0] = mousePressed_[0] || (mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0; // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[1] = mousePressed_[1] || (mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
	io.MouseDown[2] = mousePressed_[2] || (mouseButtons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
	mousePressed_[0] = mousePressed_[1] = mousePressed_[2] = false;

	// Obtain focused and hovered window. We forward mouse input when focused or when hovered (and no other window is capturing)
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
	SDL_Window *focusedWindow = SDL_GetKeyboardFocus();

	SDL_Window *hoveredWindow = SDL_HAS_MOUSE_FOCUS_CLICKTHROUGH ? SDL_GetMouseFocus() : nullptr; // This is better but is only reliably useful with SDL 2.0.5+ and SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH.
	SDL_Window *mouseWindow = nullptr;
	if (hoveredWindow && window_ == hoveredWindow)
		mouseWindow = hoveredWindow;
	else if (focusedWindow && window_ == focusedWindow)
		mouseWindow = focusedWindow;

	// SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger other operations outside
	SDL_CaptureMouse(ImGui::IsAnyMouseDown() ? SDL_TRUE : SDL_FALSE);
#else
	// SDL 2.0.3 and non-windowed systems: single-viewport only
	SDL_Window *mouseWindow = (SDL_GetWindowFlags(window_) & SDL_WINDOW_INPUT_FOCUS) ? window_ : nullptr;
#endif

	if (mouseWindow == nullptr)
		return;

	// Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
	if (io.WantSetMousePos)
		SDL_WarpMouseInWindow(window_, static_cast<int>(mousePosPrev.x), static_cast<int>(mousePosPrev.y));

	// Set Dear ImGui mouse position from OS position + get buttons. (this is the common behavior)
	if (mouseCanUseGlobalState_)
	{
		// Single-viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
		// Unlike local position obtained earlier this will be valid when straying out of bounds.
		int mouseXGlobal, mouseYGlobal;
		SDL_GetGlobalMouseState(&mouseXGlobal, &mouseYGlobal);
		int windowX, windowY;
		SDL_GetWindowPosition(mouseWindow, &windowX, &windowY);
		io.MousePos = ImVec2(static_cast<float>(mouseXGlobal - windowX), static_cast<float>(mouseYGlobal - windowY));
	}
	else
		io.MousePos = ImVec2(static_cast<float>(mouseXLocal), static_cast<float>(mouseYLocal));
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
		SDL_SetCursor(mouseCursors_[imguiCursor] ? mouseCursors_[imguiCursor] : mouseCursors_[ImGuiMouseCursor_Arrow]);
		SDL_ShowCursor(SDL_TRUE);
	}
}

void ImGuiSdlInput::updateGamepads()
{
	ImGuiIO &io = ImGui::GetIO();
	memset(io.NavInputs, 0, sizeof(io.NavInputs));
	if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
		return;

	const bool joyMappedInput = imGuiJoyMappedInput();

	if (joyMappedInput == false)
	{
		// Get gamepad
		SDL_GameController *gameController = SDL_GameControllerOpen(0);
		if (!gameController)
		{
			io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
			return;
		}

		// Update gamepad inputs
#define MAP_BUTTON(NAV_NO, BUTTON_NO) \
	{ \
		io.NavInputs[NAV_NO] = (SDL_GameControllerGetButton(gameController, BUTTON_NO) != 0) ? 1.0f : 0.0f; \
	}

#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) \
	{ \
		float vn = static_cast<float>(SDL_GameControllerGetAxis(gameController, AXIS_NO) - V0) / static_cast<float>(V1 - V0); \
		if (vn > 1.0f) \
			vn = 1.0f; \
		if (vn > 0.0f && io.NavInputs[NAV_NO] < vn) \
			io.NavInputs[NAV_NO] = vn; \
	}

		const int thumbDeadZone = 8000; // SDL_gamecontroller.h suggests using this value.
		MAP_BUTTON(ImGuiNavInput_Activate, SDL_CONTROLLER_BUTTON_A) // Cross / A
		MAP_BUTTON(ImGuiNavInput_Cancel, SDL_CONTROLLER_BUTTON_B) // Circle / B
		MAP_BUTTON(ImGuiNavInput_Menu, SDL_CONTROLLER_BUTTON_X) // Square / X
		MAP_BUTTON(ImGuiNavInput_Input, SDL_CONTROLLER_BUTTON_Y) // Triangle / Y
		MAP_BUTTON(ImGuiNavInput_DpadLeft, SDL_CONTROLLER_BUTTON_DPAD_LEFT) // D-Pad Left
		MAP_BUTTON(ImGuiNavInput_DpadRight, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) // D-Pad Right
		MAP_BUTTON(ImGuiNavInput_DpadUp, SDL_CONTROLLER_BUTTON_DPAD_UP) // D-Pad Up
		MAP_BUTTON(ImGuiNavInput_DpadDown, SDL_CONTROLLER_BUTTON_DPAD_DOWN) // D-Pad Down
		MAP_BUTTON(ImGuiNavInput_FocusPrev, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) // L1 / LB
		MAP_BUTTON(ImGuiNavInput_FocusNext, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) // R1 / RB
		MAP_BUTTON(ImGuiNavInput_TweakSlow, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) // L1 / LB
		MAP_BUTTON(ImGuiNavInput_TweakFast, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) // R1 / RB
		MAP_ANALOG(ImGuiNavInput_LStickLeft, SDL_CONTROLLER_AXIS_LEFTX, -thumbDeadZone, -32768)
		MAP_ANALOG(ImGuiNavInput_LStickRight, SDL_CONTROLLER_AXIS_LEFTX, +thumbDeadZone, +32767)
		MAP_ANALOG(ImGuiNavInput_LStickUp, SDL_CONTROLLER_AXIS_LEFTY, -thumbDeadZone, -32767)
		MAP_ANALOG(ImGuiNavInput_LStickDown, SDL_CONTROLLER_AXIS_LEFTY, +thumbDeadZone, +32767)

		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

#undef MAP_BUTTON
#undef MAP_ANALOG
	}
}

}
