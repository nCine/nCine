// Based on imgui/backends/imgui_impl_glfw.cpp

#include "ImGuiGlfwInput.h"
#include "ImGuiJoyMappedInput.h"

#include <GLFW/glfw3.h>

#ifdef _WIN32
	#undef APIENTRY
	#ifndef GLFW_EXPOSE_NATIVE_WIN32
		#define GLFW_EXPOSE_NATIVE_WIN32
	#endif
	#include <GLFW/glfw3native.h>   // for glfwGetWin32Window()
#endif
#ifdef __APPLE__
	#ifndef GLFW_EXPOSE_NATIVE_COCOA
		#define GLFW_EXPOSE_NATIVE_COCOA
	#endif
	#include <GLFW/glfw3native.h>   // for glfwGetCocoaWindow()
#endif

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
	#include <emscripten/html5.h>
#endif

// We gather version tests as define in order to easily see which features are version-dependent.
#define GLFW_VERSION_COMBINED           (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 + GLFW_VERSION_REVISION)
#ifdef GLFW_RESIZE_NESW_CURSOR          // Let's be nice to people who pulled GLFW between 2019-04-16 (3.4 define) and 2019-11-29 (cursors defines) // FIXME: Remove when GLFW 3.4 is released?
	#define GLFW_HAS_NEW_CURSORS            (GLFW_VERSION_COMBINED >= 3400) // 3.4+ GLFW_RESIZE_ALL_CURSOR, GLFW_RESIZE_NESW_CURSOR, GLFW_RESIZE_NWSE_CURSOR, GLFW_NOT_ALLOWED_CURSOR
#else
	#define GLFW_HAS_NEW_CURSORS            (0)
#endif
#define GLFW_HAS_GAMEPAD_API            (GLFW_VERSION_COMBINED >= 3300) // 3.3+ glfwGetGamepadState() new api
#define GLFW_HAS_GETKEYNAME             (GLFW_VERSION_COMBINED >= 3200) // 3.2+ glfwGetKeyName()
#define GLFW_HAS_GETERROR               (GLFW_VERSION_COMBINED >= 3300) // 3.3+ glfwGetError()

namespace ncine {

namespace {

	// Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
	GLFWwindowfocusfun prevUserCallbackWindowFocus = nullptr;
	GLFWcursorposfun prevUserCallbackCursorPos = nullptr;
	GLFWcursorenterfun prevUserCallbackCursorEnter = nullptr;
	GLFWmousebuttonfun prevUserCallbackMousebutton = nullptr;
	GLFWscrollfun prevUserCallbackScroll = nullptr;
	GLFWkeyfun prevUserCallbackKey = nullptr;
	GLFWcharfun prevUserCallbackChar = nullptr;
	GLFWmonitorfun prevUserCallbackMonitor = nullptr;
#ifdef _WIN32
	WNDPROC glfwWndProc = nullptr;
#endif

	const char *clipboardText(void *userData)
	{
		return glfwGetClipboardString(reinterpret_cast<GLFWwindow *>(userData));
	}

	void setClipboardText(void *userData, const char *text)
	{
		glfwSetClipboardString(reinterpret_cast<GLFWwindow *>(userData), text);
	}

	ImGuiKey glfwKeyToImGuiKey(int key)
	{
		switch (key)
		{
			case GLFW_KEY_TAB: return ImGuiKey_Tab;
			case GLFW_KEY_LEFT: return ImGuiKey_LeftArrow;
			case GLFW_KEY_RIGHT: return ImGuiKey_RightArrow;
			case GLFW_KEY_UP: return ImGuiKey_UpArrow;
			case GLFW_KEY_DOWN: return ImGuiKey_DownArrow;
			case GLFW_KEY_PAGE_UP: return ImGuiKey_PageUp;
			case GLFW_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
			case GLFW_KEY_HOME: return ImGuiKey_Home;
			case GLFW_KEY_END: return ImGuiKey_End;
			case GLFW_KEY_INSERT: return ImGuiKey_Insert;
			case GLFW_KEY_DELETE: return ImGuiKey_Delete;
			case GLFW_KEY_BACKSPACE: return ImGuiKey_Backspace;
			case GLFW_KEY_SPACE: return ImGuiKey_Space;
			case GLFW_KEY_ENTER: return ImGuiKey_Enter;
			case GLFW_KEY_ESCAPE: return ImGuiKey_Escape;
			case GLFW_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
			case GLFW_KEY_COMMA: return ImGuiKey_Comma;
			case GLFW_KEY_MINUS: return ImGuiKey_Minus;
			case GLFW_KEY_PERIOD: return ImGuiKey_Period;
			case GLFW_KEY_SLASH: return ImGuiKey_Slash;
			case GLFW_KEY_SEMICOLON: return ImGuiKey_Semicolon;
			case GLFW_KEY_EQUAL: return ImGuiKey_Equal;
			case GLFW_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
			case GLFW_KEY_BACKSLASH: return ImGuiKey_Backslash;
			case GLFW_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
			case GLFW_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
			case GLFW_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
			case GLFW_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
			case GLFW_KEY_NUM_LOCK: return ImGuiKey_NumLock;
			case GLFW_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
			case GLFW_KEY_PAUSE: return ImGuiKey_Pause;
			case GLFW_KEY_KP_0: return ImGuiKey_Keypad0;
			case GLFW_KEY_KP_1: return ImGuiKey_Keypad1;
			case GLFW_KEY_KP_2: return ImGuiKey_Keypad2;
			case GLFW_KEY_KP_3: return ImGuiKey_Keypad3;
			case GLFW_KEY_KP_4: return ImGuiKey_Keypad4;
			case GLFW_KEY_KP_5: return ImGuiKey_Keypad5;
			case GLFW_KEY_KP_6: return ImGuiKey_Keypad6;
			case GLFW_KEY_KP_7: return ImGuiKey_Keypad7;
			case GLFW_KEY_KP_8: return ImGuiKey_Keypad8;
			case GLFW_KEY_KP_9: return ImGuiKey_Keypad9;
			case GLFW_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
			case GLFW_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
			case GLFW_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
			case GLFW_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
			case GLFW_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
			case GLFW_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
			case GLFW_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
			case GLFW_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
			case GLFW_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
			case GLFW_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
			case GLFW_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
			case GLFW_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
			case GLFW_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
			case GLFW_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
			case GLFW_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
			case GLFW_KEY_MENU: return ImGuiKey_Menu;
			case GLFW_KEY_0: return ImGuiKey_0;
			case GLFW_KEY_1: return ImGuiKey_1;
			case GLFW_KEY_2: return ImGuiKey_2;
			case GLFW_KEY_3: return ImGuiKey_3;
			case GLFW_KEY_4: return ImGuiKey_4;
			case GLFW_KEY_5: return ImGuiKey_5;
			case GLFW_KEY_6: return ImGuiKey_6;
			case GLFW_KEY_7: return ImGuiKey_7;
			case GLFW_KEY_8: return ImGuiKey_8;
			case GLFW_KEY_9: return ImGuiKey_9;
			case GLFW_KEY_A: return ImGuiKey_A;
			case GLFW_KEY_B: return ImGuiKey_B;
			case GLFW_KEY_C: return ImGuiKey_C;
			case GLFW_KEY_D: return ImGuiKey_D;
			case GLFW_KEY_E: return ImGuiKey_E;
			case GLFW_KEY_F: return ImGuiKey_F;
			case GLFW_KEY_G: return ImGuiKey_G;
			case GLFW_KEY_H: return ImGuiKey_H;
			case GLFW_KEY_I: return ImGuiKey_I;
			case GLFW_KEY_J: return ImGuiKey_J;
			case GLFW_KEY_K: return ImGuiKey_K;
			case GLFW_KEY_L: return ImGuiKey_L;
			case GLFW_KEY_M: return ImGuiKey_M;
			case GLFW_KEY_N: return ImGuiKey_N;
			case GLFW_KEY_O: return ImGuiKey_O;
			case GLFW_KEY_P: return ImGuiKey_P;
			case GLFW_KEY_Q: return ImGuiKey_Q;
			case GLFW_KEY_R: return ImGuiKey_R;
			case GLFW_KEY_S: return ImGuiKey_S;
			case GLFW_KEY_T: return ImGuiKey_T;
			case GLFW_KEY_U: return ImGuiKey_U;
			case GLFW_KEY_V: return ImGuiKey_V;
			case GLFW_KEY_W: return ImGuiKey_W;
			case GLFW_KEY_X: return ImGuiKey_X;
			case GLFW_KEY_Y: return ImGuiKey_Y;
			case GLFW_KEY_Z: return ImGuiKey_Z;
			case GLFW_KEY_F1: return ImGuiKey_F1;
			case GLFW_KEY_F2: return ImGuiKey_F2;
			case GLFW_KEY_F3: return ImGuiKey_F3;
			case GLFW_KEY_F4: return ImGuiKey_F4;
			case GLFW_KEY_F5: return ImGuiKey_F5;
			case GLFW_KEY_F6: return ImGuiKey_F6;
			case GLFW_KEY_F7: return ImGuiKey_F7;
			case GLFW_KEY_F8: return ImGuiKey_F8;
			case GLFW_KEY_F9: return ImGuiKey_F9;
			case GLFW_KEY_F10: return ImGuiKey_F10;
			case GLFW_KEY_F11: return ImGuiKey_F11;
			case GLFW_KEY_F12: return ImGuiKey_F12;
			case GLFW_KEY_F13: return ImGuiKey_F13;
			case GLFW_KEY_F14: return ImGuiKey_F14;
			case GLFW_KEY_F15: return ImGuiKey_F15;
			case GLFW_KEY_F16: return ImGuiKey_F16;
			case GLFW_KEY_F17: return ImGuiKey_F17;
			case GLFW_KEY_F18: return ImGuiKey_F18;
			case GLFW_KEY_F19: return ImGuiKey_F19;
			case GLFW_KEY_F20: return ImGuiKey_F20;
			case GLFW_KEY_F21: return ImGuiKey_F21;
			case GLFW_KEY_F22: return ImGuiKey_F22;
			case GLFW_KEY_F23: return ImGuiKey_F23;
			case GLFW_KEY_F24: return ImGuiKey_F24;
			default: return ImGuiKey_None;
		}
	}

	// X11 does not include current pressed/released modifier key in 'mods' flags submitted by GLFW
	// See https://github.com/ocornut/imgui/issues/6034 and https://github.com/glfw/glfw/issues/1630
	void updateKeyModifiers(GLFWwindow *window)
	{
		ImGuiIO &io = ImGui::GetIO();
		io.AddKeyEvent(ImGuiMod_Ctrl, (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Shift, (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Alt, (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Super, (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS));
	}

	int translateUntranslatedKey(int key, int scancode)
	{
#if GLFW_HAS_GETKEYNAME && !defined(__EMSCRIPTEN__)
		// GLFW 3.1+ attempts to "untranslate" keys, which goes the opposite of what every other framework does, making using lettered shortcuts difficult.
		// (It had reasons to do so: namely GLFW is/was more likely to be used for WASD-type game controls rather than lettered shortcuts, but IHMO the 3.1 change could have been done differently)
		// See https://github.com/glfw/glfw/issues/1502 for details.
		// Adding a workaround to undo this (so our keys are translated->untranslated->translated, likely a lossy process).
		// This won't cover edge cases but this is at least going to cover common cases.
		if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_EQUAL)
			return key;
		GLFWerrorfun prevErrorCallback = glfwSetErrorCallback(nullptr);
		const char *keyName = glfwGetKeyName(key, scancode);
		glfwSetErrorCallback(prevErrorCallback);
	#if GLFW_HAS_GETERROR && !defined(__EMSCRIPTEN__) // Eat errors (see #5908)
		(void)glfwGetError(nullptr);
	#endif
		if (keyName && keyName[0] != 0 && keyName[1] == 0)
		{
			const char charNames[] = "`-=[]\\,;\'./";
			const int charKeys[] = { GLFW_KEY_GRAVE_ACCENT, GLFW_KEY_MINUS, GLFW_KEY_EQUAL, GLFW_KEY_LEFT_BRACKET, GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_BACKSLASH, GLFW_KEY_COMMA, GLFW_KEY_SEMICOLON, GLFW_KEY_APOSTROPHE, GLFW_KEY_PERIOD, GLFW_KEY_SLASH, 0 };
			IM_ASSERT(IM_ARRAYSIZE(charNames) == IM_ARRAYSIZE(charKeys));
			if (keyName[0] >= '0' && keyName[0] <= '9')
				key = GLFW_KEY_0 + (keyName[0] - '0');
			else if (keyName[0] >= 'A' && keyName[0] <= 'Z')
				key = GLFW_KEY_A + (keyName[0] - 'A');
			else if (keyName[0] >= 'a' && keyName[0] <= 'z')
				key = GLFW_KEY_A + (keyName[0] - 'a');
			else if (const char *p = strchr(charNames, keyName[0]))
				key = charKeys[p - charNames];
		}
#else
		IM_UNUSED(scancode);
#endif
		return key;
	}

	float saturate(float v)
	{
		return (v < 0.0f) ? 0.0f : (v > 1.0f) ? 1.0f : v;
	}

#ifdef __EMSCRIPTEN__
	EM_BOOL emscriptenWheelCallback(int eventType, const EmscriptenWheelEvent *wheelEvent, void *userData)
	{
		// Mimic emscriptenHandleWheel() in SDL.
		// Corresponding equivalent in GLFW JS emulation layer has incorrect quantizing preventing small values. See #6096
		float multiplier = 0.0f;

		if (wheelEvent->deltaMode == DOM_DELTA_PIXEL)       { multiplier = 1.0f / 100.0f; } // 100 pixels make up a step.
		else if (wheelEvent->deltaMode == DOM_DELTA_LINE)   { multiplier = 1.0f / 3.0f; }   // 3 lines make up a step.
		else if (wheelEvent->deltaMode == DOM_DELTA_PAGE)   { multiplier = 80.0f; }         // A page makes up 80 steps.
		float wheelX = wheelEvent->deltaX * -multiplier;
		float wheelY = wheelEvent->deltaY * -multiplier;

		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseWheelEvent(wheelX, wheelY);

		return EM_TRUE;
	}
#endif

#ifdef _WIN32
	// GLFW doesn't allow to distinguish Mouse vs TouchScreen vs Pen.
	// Add support for Win32 (based on imgui_impl_win32), because we rely on _TouchScreen info to trickle inputs differently.
	ImGuiMouseSource getMouseSourceFromMessageExtraInfo()
	{
		LPARAM extra_info = ::GetMessageExtraInfo();
		if ((extra_info & 0xFFFFFF80) == 0xFF515700)
			return ImGuiMouseSource_Pen;
		if ((extra_info & 0xFFFFFF80) == 0xFF515780)
			return ImGuiMouseSource_TouchScreen;
		return ImGuiMouseSource_Mouse;
	}

	LRESULT CALLBACK ImGui_ImplGlfw_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_MOUSEMOVE: case WM_NCMOUSEMOVE:
			case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK: case WM_LBUTTONUP:
			case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK: case WM_RBUTTONUP:
			case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK: case WM_MBUTTONUP:
			case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK: case WM_XBUTTONUP:
				ImGui::GetIO().AddMouseSourceEvent(getMouseSourceFromMessageExtraInfo());
				break;
		}
		return ::CallWindowProc(glfwWndProc, hWnd, msg, wParam, lParam);
	}
#endif

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool ImGuiGlfwInput::inputEnabled_ = true;
GLFWwindow *ImGuiGlfwInput::window_ = nullptr;
GLFWwindow *ImGuiGlfwInput::mouseWindow_ = nullptr;
double ImGuiGlfwInput::time_ = 0.0;
GLFWcursor *ImGuiGlfwInput::mouseCursors_[ImGuiMouseCursor_COUNT] = {};
ImVec2 ImGuiGlfwInput::lastValidMousePos_ = { FLT_MAX, FLT_MAX };
bool ImGuiGlfwInput::installedCallbacks_ = false;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ImGuiGlfwInput::init(GLFWwindow *window, bool withCallbacks)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();

	// Setup backend capabilities flags
	io.BackendPlatformName = "nCine_GLFW";
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)

	window_ = window;
	time_ = 0.0;

	io.SetClipboardTextFn = setClipboardText;
	io.GetClipboardTextFn = clipboardText;
	io.ClipboardUserData = window_;

	// Create mouse cursors
	// (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
	// GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
	// Missing cursors will return NULL and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
	const GLFWerrorfun prevErrorCallback = glfwSetErrorCallback(nullptr);
	mouseCursors_[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	mouseCursors_[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	mouseCursors_[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	mouseCursors_[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	mouseCursors_[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
	mouseCursors_[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
	mouseCursors_[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
	mouseCursors_[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
	mouseCursors_[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
	mouseCursors_[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	mouseCursors_[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	mouseCursors_[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	mouseCursors_[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
	glfwSetErrorCallback(prevErrorCallback);
#if GLFW_HAS_GETERROR && !defined(__EMSCRIPTEN__) // Eat errors (see #5908)
	(void)glfwGetError(nullptr);
#endif

	// Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
	if (withCallbacks)
		installCallbacks(window);

	// Register Emscripten Wheel callback to workaround issue in Emscripten GLFW Emulation (#6096)
	// We intentionally do not check 'if (install_callbacks)' here, as some users may set it to false and call GLFW callback themselves.
	// FIXME: May break chaining in case user registered their own Emscripten callback?
#ifdef __EMSCRIPTEN__
	emscripten_set_wheel_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, false, emscriptenWheelCallback);
#endif

	// Set platform dependent data in viewport
	ImGuiViewport *mainViewport = ImGui::GetMainViewport();
#ifdef _WIN32
	mainViewport->PlatformHandleRaw = glfwGetWin32Window(window_);
#elif defined(__APPLE__)
	mainViewport->PlatformHandleRaw = (void*)glfwGetCocoaWindow(window_);
#else
	(void)mainViewport;
#endif

	// Windows: register a WndProc hook so we can intercept some messages.
#ifdef _WIN32
	glfwWndProc = (WNDPROC)::GetWindowLongPtr((HWND)mainViewport->PlatformHandleRaw, GWLP_WNDPROC);
	IM_ASSERT(glfwWndProc != nullptr);
	::SetWindowLongPtr((HWND)mainViewport->PlatformHandleRaw, GWLP_WNDPROC, (LONG_PTR)ImGui_ImplGlfw_WndProc);
#endif
}

void ImGuiGlfwInput::shutdown()
{
	ImGuiIO &io = ImGui::GetIO();

	if (installedCallbacks_)
		restoreCallbacks(window_);

#ifdef __EMSCRIPTEN__
	emscripten_set_wheel_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, false, nullptr);
#endif

	for (ImGuiMouseCursor i = 0; i < ImGuiMouseCursor_COUNT; i++)
		glfwDestroyCursor(mouseCursors_[i]);

		// Windows: register a WndProc hook so we can intercept some messages.
#ifdef _WIN32
	ImGuiViewport *mainViewport = ImGui::GetMainViewport();
	::SetWindowLongPtr((HWND)mainViewport->PlatformHandleRaw, GWLP_WNDPROC, (LONG_PTR)glfwWndProc);
	glfwWndProc = nullptr;
#endif

	io.BackendPlatformName = nullptr;
	io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad);

	ImGui::DestroyContext();
}

void ImGuiGlfwInput::newFrame()
{
	ImGuiIO &io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! Missing call to ImGuiDrawing::buildFonts() function?");

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int displayW, displayH;
	glfwGetWindowSize(window_, &w, &h);
	glfwGetFramebufferSize(window_, &displayW, &displayH);
	io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
	if (w > 0 && h > 0)
		io.DisplayFramebufferScale = ImVec2(static_cast<float>(displayW) / w, static_cast<float>(displayH) / h);

	// Setup time step
	// (Accept glfwGetTime() not returning a monotonically increasing value. Seems to happens on disconnecting peripherals and probably on VMs and Emscripten, see #6491, #6189, #6114, #3644)
	double currentTime = glfwGetTime();
	if (currentTime <= time_)
		currentTime = time_ + 0.00001f;
	io.DeltaTime = time_ > 0.0 ? static_cast<float>(currentTime - time_) : static_cast<float>(1.0f / 60.0f);
	time_ = currentTime;

	updateMouseData();
	updateMouseCursor();

	// Update game controllers (if enabled and available)
	updateGamepads();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ImGuiGlfwInput::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (prevUserCallbackMousebutton != nullptr && window == window_)
		prevUserCallbackMousebutton(window, button, action, mods);

	if (inputEnabled_ == false)
		return;

	updateKeyModifiers(window);
	ImGuiIO &io = ImGui::GetIO();
	if (button >= 0 && button < ImGuiMouseButton_COUNT)
		io.AddMouseButtonEvent(button, action == GLFW_PRESS);
}

void ImGuiGlfwInput::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	if (prevUserCallbackScroll != nullptr && window == window_)
		prevUserCallbackScroll(window, xoffset, yoffset);

	if (inputEnabled_ == false)
		return;

#ifdef __EMSCRIPTEN__
	// Ignore GLFW events: will be processed in emscriptenWheelCallback().
	return;
#endif

	ImGuiIO &io = ImGui::GetIO();
	io.AddMouseWheelEvent(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

void ImGuiGlfwInput::keyCallback(GLFWwindow *window, int keycode, int scancode, int action, int mods)
{
	if (prevUserCallbackKey != nullptr && window == window_)
		prevUserCallbackKey(window, keycode, scancode, action, mods);

	if (inputEnabled_ == false)
		return;

	updateKeyModifiers(window);

	keycode = translateUntranslatedKey(keycode, scancode);

	ImGuiIO &io = ImGui::GetIO();
	ImGuiKey imguiKey = glfwKeyToImGuiKey(keycode);
	io.AddKeyEvent(imguiKey, (action == GLFW_PRESS));
	io.SetKeyEventNativeData(imguiKey, keycode, scancode); // To support legacy indexing (<1.87 user code)
}

void ImGuiGlfwInput::windowFocusCallback(GLFWwindow *window, int focused)
{
	if (prevUserCallbackWindowFocus != nullptr && window == window_)
		prevUserCallbackWindowFocus(window, focused);

	ImGuiIO &io = ImGui::GetIO();
	io.AddFocusEvent(focused != 0);
}

void ImGuiGlfwInput::cursorPosCallback(GLFWwindow *window, double x, double y)
{
	if (prevUserCallbackCursorPos != nullptr && window == window_)
		prevUserCallbackCursorPos(window, x, y);

	if (inputEnabled_ == false)
		return;

	ImGuiIO &io = ImGui::GetIO();
	io.AddMousePosEvent(static_cast<float>(x), static_cast<float>(y));
	lastValidMousePos_ = ImVec2(static_cast<float>(x), static_cast<float>(y));
}

void ImGuiGlfwInput::cursorEnterCallback(GLFWwindow *window, int entered)
{
	if (prevUserCallbackCursorEnter != nullptr && window == window_)
		prevUserCallbackCursorEnter(window, entered);

	if (entered)
		mouseWindow_ = window;
	if (!entered && mouseWindow_ == window)
		mouseWindow_ = nullptr;
}

void ImGuiGlfwInput::charCallback(GLFWwindow *window, unsigned int c)
{
	if (prevUserCallbackChar != nullptr && window == window_)
		prevUserCallbackChar(window, c);

	if (inputEnabled_ == false)
		return;

	ImGuiIO &io = ImGui::GetIO();
	io.AddInputCharacter(c);
}

void ImGuiGlfwInput::monitorCallback(GLFWmonitor *monitor, int event)
{
	if (prevUserCallbackMonitor != nullptr)
		prevUserCallbackMonitor(monitor, event);

	// Unused in 'master' branch but 'docking' branch will use this, so we declare it ahead of it so if you have to install callbacks you can install this one too.
}

void ImGuiGlfwInput::installCallbacks(GLFWwindow *window)
{
	IM_ASSERT(installedCallbacks_ == false && "Callbacks already installed!");
	IM_ASSERT(window_ == window);

	prevUserCallbackWindowFocus = glfwSetWindowFocusCallback(window, windowFocusCallback);
	prevUserCallbackCursorEnter = glfwSetCursorEnterCallback(window, cursorEnterCallback);
	prevUserCallbackCursorPos = glfwSetCursorPosCallback(window, cursorPosCallback);
	prevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, mouseButtonCallback);
	prevUserCallbackScroll = glfwSetScrollCallback(window, scrollCallback);
	prevUserCallbackKey = glfwSetKeyCallback(window, keyCallback);
	prevUserCallbackChar = glfwSetCharCallback(window, charCallback);
	prevUserCallbackMonitor = glfwSetMonitorCallback(monitorCallback);
	installedCallbacks_ = true;
}

void ImGuiGlfwInput::restoreCallbacks(GLFWwindow *window)
{
	IM_ASSERT(installedCallbacks_ == true && "Callbacks not installed!");
	IM_ASSERT(window_ == window);

	glfwSetWindowFocusCallback(window, prevUserCallbackWindowFocus);
	glfwSetCursorEnterCallback(window, prevUserCallbackCursorEnter);
	glfwSetCursorPosCallback(window, prevUserCallbackCursorPos);
	glfwSetMouseButtonCallback(window, prevUserCallbackMousebutton);
	glfwSetScrollCallback(window, prevUserCallbackScroll);
	glfwSetKeyCallback(window, prevUserCallbackKey);
	glfwSetCharCallback(window, prevUserCallbackChar);
	glfwSetMonitorCallback(prevUserCallbackMonitor);

	installedCallbacks_ = false;
	prevUserCallbackWindowFocus = nullptr;
	prevUserCallbackCursorEnter = nullptr;
	prevUserCallbackCursorPos = nullptr;
	prevUserCallbackMousebutton = nullptr;
	prevUserCallbackScroll = nullptr;
	prevUserCallbackKey = nullptr;
	prevUserCallbackChar = nullptr;
	prevUserCallbackMonitor = nullptr;
}

void ImGuiGlfwInput::updateMouseData()
{
	ImGuiIO &io = ImGui::GetIO();

	// (those braces are here to reduce diff with multi-viewports support in 'docking' branch)
	{
#ifdef __EMSCRIPTEN__
		const bool isWindowFocused = true;
#else
		const bool isWindowFocused = glfwGetWindowAttrib(window_, GLFW_FOCUSED) != 0;
#endif
		if (isWindowFocused)
		{
			// (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
			if (io.WantSetMousePos)
				glfwSetCursorPos(window_, static_cast<double>(io.MousePos.x), static_cast<double>(io.MousePos.y));

			// (Optional) Fallback to provide mouse position when focused (ImGui_ImplGlfw_CursorPosCallback already provides this when hovered or captured)
			if (window_ == nullptr)
			{
				double mouseX, mouseY;
				glfwGetCursorPos(window_, &mouseX, &mouseY);
				lastValidMousePos_ = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
				io.AddMousePosEvent(static_cast<float>(mouseX), static_cast<float>(mouseY));
			}
		}
	}
}

void ImGuiGlfwInput::updateMouseCursor()
{
	ImGuiIO &io = ImGui::GetIO();
	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		return;

	ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
	// (those braces are here to reduce diff with multi-viewports support in 'docking' branch)
	{
		if (imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else
		{
			// Show OS mouse cursor
			// FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
			glfwSetCursor(window_, mouseCursors_[imguiCursor] ? mouseCursors_[imguiCursor] : mouseCursors_[ImGuiMouseCursor_Arrow]);
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void ImGuiGlfwInput::updateGamepads()
{
	ImGuiIO &io = ImGui::GetIO();
	if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0) // FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs.
		return;

	const bool joyMappedInput = imGuiJoyMappedInput();

	if (joyMappedInput == false)
	{
		io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
#if GLFW_HAS_GAMEPAD_API && !defined(__EMSCRIPTEN__)
		GLFWgamepadstate gamepad;
		if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad))
			return;
		// clang-format off
		#define MAP_BUTTON(KEY_NO, BUTTON_NO, _UNUSED)          do { io.AddKeyEvent(KEY_NO, gamepad.buttons[BUTTON_NO] != 0); } while (0)
		#define MAP_ANALOG(KEY_NO, AXIS_NO, _UNUSED, V0, V1)    do { float v = gamepad.axes[AXIS_NO]; v = (v - V0) / (V1 - V0);\
		                                                             io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, saturate(v)); } while (0)
		// clang-format on
#else
		int axesCount = 0, buttonsCount = 0;
		const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonsCount);
		if (axesCount == 0 || buttonsCount == 0)
			return;
		// clang-format off
		#define MAP_BUTTON(KEY_NO, _UNUSED, BUTTON_NO)          do { io.AddKeyEvent(KEY_NO, (buttonsCount > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS)); } while (0)
		#define MAP_ANALOG(KEY_NO, _UNUSED, AXIS_NO, V0, V1)    do { float v = (axesCount > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0);\
		                                                             io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, saturate(v)); } while (0)
		// clang-format on
#endif
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		// clang-format off
		MAP_BUTTON(ImGuiKey_GamepadStart,       GLFW_GAMEPAD_BUTTON_START,          7);
		MAP_BUTTON(ImGuiKey_GamepadBack,        GLFW_GAMEPAD_BUTTON_BACK,           6);
		MAP_BUTTON(ImGuiKey_GamepadFaceLeft,    GLFW_GAMEPAD_BUTTON_X,              2);     // Xbox X, PS Square
		MAP_BUTTON(ImGuiKey_GamepadFaceRight,   GLFW_GAMEPAD_BUTTON_B,              1);     // Xbox B, PS Circle
		MAP_BUTTON(ImGuiKey_GamepadFaceUp,      GLFW_GAMEPAD_BUTTON_Y,              3);     // Xbox Y, PS Triangle
		MAP_BUTTON(ImGuiKey_GamepadFaceDown,    GLFW_GAMEPAD_BUTTON_A,              0);     // Xbox A, PS Cross
		MAP_BUTTON(ImGuiKey_GamepadDpadLeft,    GLFW_GAMEPAD_BUTTON_DPAD_LEFT,      13);
		MAP_BUTTON(ImGuiKey_GamepadDpadRight,   GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,     11);
		MAP_BUTTON(ImGuiKey_GamepadDpadUp,      GLFW_GAMEPAD_BUTTON_DPAD_UP,        10);
		MAP_BUTTON(ImGuiKey_GamepadDpadDown,    GLFW_GAMEPAD_BUTTON_DPAD_DOWN,      12);
		MAP_BUTTON(ImGuiKey_GamepadL1,          GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,    4);
		MAP_BUTTON(ImGuiKey_GamepadR1,          GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,   5);
		MAP_ANALOG(ImGuiKey_GamepadL2,          GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,     4,      -0.75f,  +1.0f);
		MAP_ANALOG(ImGuiKey_GamepadR2,          GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,    5,      -0.75f,  +1.0f);
		MAP_BUTTON(ImGuiKey_GamepadL3,          GLFW_GAMEPAD_BUTTON_LEFT_THUMB,     8);
		MAP_BUTTON(ImGuiKey_GamepadR3,          GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,    9);
		MAP_ANALOG(ImGuiKey_GamepadLStickLeft,  GLFW_GAMEPAD_AXIS_LEFT_X,           0,      -0.25f,  -1.0f);
		MAP_ANALOG(ImGuiKey_GamepadLStickRight, GLFW_GAMEPAD_AXIS_LEFT_X,           0,      +0.25f,  +1.0f);
		MAP_ANALOG(ImGuiKey_GamepadLStickUp,    GLFW_GAMEPAD_AXIS_LEFT_Y,           1,      -0.25f,  -1.0f);
		MAP_ANALOG(ImGuiKey_GamepadLStickDown,  GLFW_GAMEPAD_AXIS_LEFT_Y,           1,      +0.25f,  +1.0f);
		MAP_ANALOG(ImGuiKey_GamepadRStickLeft,  GLFW_GAMEPAD_AXIS_RIGHT_X,          2,      -0.25f,  -1.0f);
		MAP_ANALOG(ImGuiKey_GamepadRStickRight, GLFW_GAMEPAD_AXIS_RIGHT_X,          2,      +0.25f,  +1.0f);
		MAP_ANALOG(ImGuiKey_GamepadRStickUp,    GLFW_GAMEPAD_AXIS_RIGHT_Y,          3,      -0.25f,  -1.0f);
		MAP_ANALOG(ImGuiKey_GamepadRStickDown,  GLFW_GAMEPAD_AXIS_RIGHT_Y,          3,      +0.25f,  +1.0f);
		#undef MAP_BUTTON
		#undef MAP_ANALOG
		// clang-format on

	}
}

}
