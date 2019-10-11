// Based on imgui/examples/imgui_impl_glfw.cpp

#include "ImGuiGlfwInput.h"
#include "ImGuiJoyMappedInput.h"

#include <GLFW/glfw3.h>
#ifdef _WIN32
	#undef APIENTRY
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h> // for glfwGetWin32Window
#endif

// clang-format off
#define GLFW_HAS_WINDOW_TOPMOST        (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ GLFW_FLOATING
#define GLFW_HAS_WINDOW_HOVERED        (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ GLFW_HOVERED
#define GLFW_HAS_WINDOW_ALPHA          (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwSetWindowOpacity
#define GLFW_HAS_PER_MONITOR_DPI       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwGetMonitorContentScale
#define GLFW_HAS_VULKAN                (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwCreateWindowSurface
// clang-format on

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool ImGuiGlfwInput::inputEnabled_ = true;
GLFWwindow *ImGuiGlfwInput::window_ = nullptr;
double ImGuiGlfwInput::time_ = 0.0;
bool ImGuiGlfwInput::mouseJustPressed_[5] = { false, false, false, false, false };
GLFWcursor *ImGuiGlfwInput::mouseCursors_[ImGuiMouseCursor_COUNT] = { nullptr };

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	const char *clipboardText(void *userData)
	{
		return glfwGetClipboardString(reinterpret_cast<GLFWwindow *>(userData));
	}

	void setClipboardText(void *userData, const char *text)
	{
		glfwSetClipboardString(reinterpret_cast<GLFWwindow *>(userData), text);
	}

}

void ImGuiGlfwInput::init(GLFWwindow *window)
{
	window_ = window;
	time_ = 0.0;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup back-end capabilities flags
	ImGuiIO &io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendPlatformName = "nCine_GLFW";

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	io.SetClipboardTextFn = setClipboardText;
	io.GetClipboardTextFn = clipboardText;
	io.ClipboardUserData = window_;
#if defined(_WIN32)
	io.ImeWindowHandle = reinterpret_cast<void *>(glfwGetWin32Window(window_));
#endif

	mouseCursors_[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	mouseCursors_[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	mouseCursors_[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR); // FIXME: GLFW doesn't have this.
	mouseCursors_[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	mouseCursors_[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	mouseCursors_[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR); // FIXME: GLFW doesn't have this.
	mouseCursors_[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR); // FIXME: GLFW doesn't have this.
	mouseCursors_[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
}

void ImGuiGlfwInput::shutdown()
{
	for (ImGuiMouseCursor i = 0; i < ImGuiMouseCursor_COUNT; i++)
	{
		glfwDestroyCursor(mouseCursors_[i]);
		mouseCursors_[i] = nullptr;
	}

	ImGui::DestroyContext();
}

void ImGuiGlfwInput::newFrame()
{
	ImGuiIO &io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int displayW, displayH;
	glfwGetWindowSize(window_, &w, &h);
	glfwGetFramebufferSize(window_, &displayW, &displayH);
	io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
	if (w > 0 && h > 0)
		io.DisplayFramebufferScale = ImVec2(static_cast<float>(displayW) / w, static_cast<float>(displayH) / h);

	// Setup time step
	double currentTime = glfwGetTime();
	io.DeltaTime = time_ > 0.0 ? static_cast<float>(currentTime - time_) : static_cast<float>(1.0f / 60.0f);
	time_ = currentTime;

	updateMousePosAndButtons();
	updateMouseCursor();

	// Update game controllers (if enabled and available)
	updateGamepads();
}

void ImGuiGlfwInput::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (inputEnabled_ == false)
		return;

	if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(mouseJustPressed_))
		mouseJustPressed_[button] = true;
}

void ImGuiGlfwInput::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	if (inputEnabled_ == false)
		return;

	ImGuiIO &io = ImGui::GetIO();
	io.MouseWheelH += static_cast<float>(xoffset);
	io.MouseWheel += static_cast<float>(yoffset);
}

void ImGuiGlfwInput::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (inputEnabled_ == false)
		return;

	ImGuiIO &io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	(void)mods; // Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void ImGuiGlfwInput::charCallback(GLFWwindow *, unsigned int c)
{
	if (inputEnabled_ == false)
		return;

	ImGuiIO &io = ImGui::GetIO();
	io.AddInputCharacter(c);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ImGuiGlfwInput::updateMousePosAndButtons()
{
	// Update buttons
	ImGuiIO &io = ImGui::GetIO();
	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
	{
		// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		io.MouseDown[i] = mouseJustPressed_[i] || glfwGetMouseButton(window_, i) != 0;
		mouseJustPressed_[i] = false;
	}

	// Update mouse position
	const ImVec2 mousePosBackup = io.MousePos;
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	if (glfwGetWindowAttrib(window_, GLFW_FOCUSED))
	{
		if (io.WantSetMousePos)
			glfwSetCursorPos(window_, static_cast<double>(mousePosBackup.x), static_cast<double>(mousePosBackup.y));
		else
		{
			double mouse_x, mouse_y;
			glfwGetCursorPos(window_, &mouse_x, &mouse_y);
			io.MousePos = ImVec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
		}
	}
}

void ImGuiGlfwInput::updateMouseCursor()
{
	ImGuiIO &io = ImGui::GetIO();
	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		return;

	ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
	if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
	{
		// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	else
	{
		// Show OS mouse cursor
		// FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
		glfwSetCursor(window_, mouseCursors_[imgui_cursor] ? mouseCursors_[imgui_cursor] : mouseCursors_[ImGuiMouseCursor_Arrow]);
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void ImGuiGlfwInput::updateGamepads()
{
	ImGuiIO &io = ImGui::GetIO();
	memset(io.NavInputs, 0, sizeof(io.NavInputs));
	if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
		return;

	const bool joyMappedInput = imGuiJoyMappedInput();

	if (joyMappedInput == false)
	{
		// Update gamepad inputs
#define MAP_BUTTON(NAV_NO, BUTTON_NO) \
	{ \
		if (buttonsCount > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) \
			io.NavInputs[NAV_NO] = 1.0f; \
	}

#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) \
	{ \
		float v = (axesCount > AXIS_NO) ? axes[AXIS_NO] : V0; \
		v = (v - V0) / (V1 - V0); \
		if (v > 1.0f) \
			v = 1.0f; \
		if (io.NavInputs[NAV_NO] < v) \
			io.NavInputs[NAV_NO] = v; \
	}

		int axesCount = 0, buttonsCount = 0;
		const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonsCount);
		MAP_BUTTON(ImGuiNavInput_Activate, 0) // Cross / A
		MAP_BUTTON(ImGuiNavInput_Cancel, 1) // Circle / B
		MAP_BUTTON(ImGuiNavInput_Menu, 2) // Square / X
		MAP_BUTTON(ImGuiNavInput_Input, 3) // Triangle / Y
		MAP_BUTTON(ImGuiNavInput_DpadLeft, 13) // D-Pad Left
		MAP_BUTTON(ImGuiNavInput_DpadRight, 11) // D-Pad Right
		MAP_BUTTON(ImGuiNavInput_DpadUp, 10) // D-Pad Up
		MAP_BUTTON(ImGuiNavInput_DpadDown, 12) // D-Pad Down
		MAP_BUTTON(ImGuiNavInput_FocusPrev, 4) // L1 / LB
		MAP_BUTTON(ImGuiNavInput_FocusNext, 5) // R1 / RB
		MAP_BUTTON(ImGuiNavInput_TweakSlow, 4) // L1 / LB
		MAP_BUTTON(ImGuiNavInput_TweakFast, 5) // R1 / RB
		MAP_ANALOG(ImGuiNavInput_LStickLeft, 0, -0.3f, -0.9f)
		MAP_ANALOG(ImGuiNavInput_LStickRight, 0, +0.3f, +0.9f)
		MAP_ANALOG(ImGuiNavInput_LStickUp, 1, +0.3f, +0.9f)
		MAP_ANALOG(ImGuiNavInput_LStickDown, 1, -0.3f, -0.9f)

#undef MAP_BUTTON
#undef MAP_ANALOG

		if (axesCount > 0 && buttonsCount > 0)
			io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		else
			io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
	}
}

}
