#include <android/keycodes.h>
#include <android/input.h>

#include "imgui.h"
#include "ImGuiAndroidInput.h"
#include "ImGuiJoyMappedInput.h"
#include "Application.h"
#include "AndroidJniHelper.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool ImGuiAndroidInput::inputEnabled_ = true;
int ImGuiAndroidInput::simulatedSoftKeyReleased_ = 0;
int ImGuiAndroidInput::simulatedMouseButtonState_ = 0;
int ImGuiAndroidInput::mouseButtonState_ = 0;
unsigned int ImGuiAndroidInput::pointerCount_ = 0;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	static const int MaxClipboardLength = 256;
	static char clipboard[MaxClipboardLength];

	const char *clipboardText(void *userData)
	{
		return clipboard;
	}

	void setClipboardText(void *userData, const char *text)
	{
		strncpy(clipboard, text, MaxClipboardLength);
	}

}

void ImGuiAndroidInput::init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.BackendPlatformName = "nCine_Android";
	io.DisplaySize = ImVec2(theApplication().width(), theApplication().height());
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = AKEYCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = AKEYCODE_DPAD_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = AKEYCODE_DPAD_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = AKEYCODE_DPAD_UP;
	io.KeyMap[ImGuiKey_DownArrow] = AKEYCODE_DPAD_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = AKEYCODE_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = AKEYCODE_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = AKEYCODE_MOVE_HOME;
	io.KeyMap[ImGuiKey_End] = AKEYCODE_MOVE_END;
	io.KeyMap[ImGuiKey_Insert] = AKEYCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = AKEYCODE_FORWARD_DEL;
	io.KeyMap[ImGuiKey_Backspace] = AKEYCODE_DEL;
	io.KeyMap[ImGuiKey_Space] = AKEYCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = AKEYCODE_ENTER;
	io.KeyMap[ImGuiKey_Escape] = AKEYCODE_ESCAPE;
#if IMGUI_VERSION_NUM > 18600
	io.KeyMap[ImGuiKey_KeypadEnter] = AKEYCODE_NUMPAD_ENTER;
#else
	io.KeyMap[ImGuiKey_KeyPadEnter] = AKEYCODE_NUMPAD_ENTER;
#endif
	io.KeyMap[ImGuiKey_A] = AKEYCODE_A;
	io.KeyMap[ImGuiKey_C] = AKEYCODE_C;
	io.KeyMap[ImGuiKey_V] = AKEYCODE_V;
	io.KeyMap[ImGuiKey_X] = AKEYCODE_X;
	io.KeyMap[ImGuiKey_Y] = AKEYCODE_Y;
	io.KeyMap[ImGuiKey_Z] = AKEYCODE_Z;

	io.SetClipboardTextFn = setClipboardText;
	io.GetClipboardTextFn = clipboardText;
	io.ClipboardUserData = nullptr;
}

void ImGuiAndroidInput::shutdown()
{
	ImGui::DestroyContext();
}

void ImGuiAndroidInput::newFrame()
{
	ImGuiIO &io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! Missing call to ImGuiDrawing::buildFonts() function?");
	io.DeltaTime = theApplication().interval();

	updateMouseButtons();

	// Update game controllers (if enabled and available)
	memset(io.NavInputs, 0, sizeof(io.NavInputs));
	if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)
		imGuiJoyMappedInput();
}

void ImGuiAndroidInput::postNewFrame()
{
	if (simulatedSoftKeyReleased_ != 0)
	{
		ImGuiIO &io = ImGui::GetIO();
		io.KeysDown[simulatedSoftKeyReleased_] = false;
		simulatedSoftKeyReleased_ = 0;
	}
}

bool ImGuiAndroidInput::processEvent(const AInputEvent *event)
{
	if (inputEnabled_ == false)
		return false;

	ImGuiIO &io = ImGui::GetIO();
	if (pointerCount_ == 0 && io.ConfigFlags & ImGuiConfigFlags_IsTouchScreen)
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

	if (((AInputEvent_getSource(event) & AINPUT_SOURCE_KEYBOARD) == AINPUT_SOURCE_KEYBOARD ||
	     (AKeyEvent_getFlags(event) & AKEY_EVENT_FLAG_SOFT_KEYBOARD) == AKEY_EVENT_FLAG_SOFT_KEYBOARD) &&
	    AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		const int keyCode = AKeyEvent_getKeyCode(event);
		IM_ASSERT(keyCode >= 0 && keyCode < IM_ARRAYSIZE(io.KeysDown));
		io.KeysDown[keyCode] = (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN);
		io.KeyShift = ((AKeyEvent_getMetaState(event) & AMETA_SHIFT_ON) != 0);
		io.KeyCtrl = ((AKeyEvent_getMetaState(event) & AMETA_CTRL_ON) != 0);
		io.KeyAlt = ((AKeyEvent_getMetaState(event) & AMETA_ALT_ON) != 0);
		io.KeySuper = ((AKeyEvent_getMetaState(event) & AMETA_META_ON) != 0);

		// With the software keyboard the press and release events might happen on the same frame
		if ((AKeyEvent_getFlags(event) & AKEY_EVENT_FLAG_SOFT_KEYBOARD) == AKEY_EVENT_FLAG_SOFT_KEYBOARD)
		{
			static unsigned long int frameNum = 0;
			if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN)
				frameNum = theApplication().numFrames();
			else if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_UP && theApplication().numFrames() == frameNum)
			{
				io.KeysDown[keyCode] = true;
				simulatedSoftKeyReleased_ = keyCode;
			}
		}

		if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN &&
		    (AKeyEvent_getMetaState(event) & AMETA_CTRL_ON) == 0)
		{
			AndroidJniClass_KeyEvent keyEvent(AInputEvent_getType(event), keyCode);
			if (keyEvent.isPrintingKey())
			{
				const int unicodeKey = keyEvent.getUnicodeChar(AKeyEvent_getMetaState(event));
				io.AddInputCharacter(unicodeKey);
			}
		}

		return true;
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN &&
	         AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
		const int action = AMotionEvent_getAction(event);
		pointerCount_ = AMotionEvent_getPointerCount(event);

		if (action == AMOTION_EVENT_ACTION_UP)
			pointerCount_ = 0;

		io.MousePos = ImVec2(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));

		return true;
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE &&
	         AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		io.ConfigFlags &= ~ImGuiConfigFlags_IsTouchScreen;
		const int action = AMotionEvent_getAction(event);

		// Mask out back and forward buttons in the detected state
		// as those are simulated as right and middle buttons
		int maskOutButtons = 0;
		if (simulatedMouseButtonState_ & AMOTION_EVENT_BUTTON_SECONDARY)
			maskOutButtons |= AMOTION_EVENT_BUTTON_BACK;
		if (simulatedMouseButtonState_ & AMOTION_EVENT_BUTTON_TERTIARY)
			maskOutButtons |= AMOTION_EVENT_BUTTON_FORWARD;

		switch (action)
		{
			case AMOTION_EVENT_ACTION_DOWN:
				mouseButtonState_ = AMotionEvent_getButtonState(event);
				mouseButtonState_ &= ~maskOutButtons;
				mouseButtonState_ |= simulatedMouseButtonState_;
				break;
			case AMOTION_EVENT_ACTION_UP:
				mouseButtonState_ = AMotionEvent_getButtonState(event);
				mouseButtonState_ &= ~maskOutButtons;
				mouseButtonState_ |= simulatedMouseButtonState_;
				break;
		}

		io.MousePos = ImVec2(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));
		io.MouseWheelH = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HSCROLL, 0);
		io.MouseWheel = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_VSCROLL, 0);

		return true;
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE &&
	         AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		io.ConfigFlags &= ~ImGuiConfigFlags_IsTouchScreen;
		const int keyCode = AKeyEvent_getKeyCode(event);
		if (keyCode == AKEYCODE_BACK || keyCode == AKEYCODE_FORWARD)
		{
			const int simulatedButton = (keyCode == AKEYCODE_BACK) ? AMOTION_EVENT_BUTTON_SECONDARY : AMOTION_EVENT_BUTTON_TERTIARY;
			static int oldAction = AKEY_EVENT_ACTION_UP;
			const int action = AKeyEvent_getAction(event);

			// checking previous action to avoid key repeat events
			if (action == AKEY_EVENT_ACTION_DOWN && oldAction == AKEY_EVENT_ACTION_UP)
			{
				oldAction = action;
				simulatedMouseButtonState_ |= simulatedButton;
				mouseButtonState_ |= simulatedButton;
			}
			else if (action == AKEY_EVENT_ACTION_UP && oldAction == AKEY_EVENT_ACTION_DOWN)
			{
				oldAction = action;
				simulatedMouseButtonState_ &= ~simulatedButton;
				mouseButtonState_ &= ~simulatedButton;
			}

			io.MousePos = ImVec2(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));
		}

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ImGuiAndroidInput::updateMouseButtons()
{
	ImGuiIO &io = ImGui::GetIO();
	if (pointerCount_ > 0)
	{
		io.MouseDown[0] = (pointerCount_ == 1);
		io.MouseDown[1] = (pointerCount_ == 2);
	}
	else
	{
		io.MouseDown[0] = (mouseButtonState_ & AMOTION_EVENT_BUTTON_PRIMARY);
		io.MouseDown[1] = (mouseButtonState_ & AMOTION_EVENT_BUTTON_SECONDARY);
		io.MouseDown[2] = (mouseButtonState_ & AMOTION_EVENT_BUTTON_TERTIARY);
		io.MouseDown[3] = (mouseButtonState_ & AMOTION_EVENT_BUTTON_BACK);
		io.MouseDown[4] = (mouseButtonState_ & AMOTION_EVENT_BUTTON_FORWARD);
	}
}

}
