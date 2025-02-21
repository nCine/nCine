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

ANativeWindow *ImGuiAndroidInput::window_ = nullptr;
bool ImGuiAndroidInput::inputEnabled_ = true;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	static const int MaxClipboardLength = 256;
	static char clipboard[MaxClipboardLength];

	const char *clipboardText(ImGuiContext *context)
	{
		return clipboard;
	}

	void setClipboardText(ImGuiContext *context, const char *text)
	{
		strncpy(clipboard, text, MaxClipboardLength);
	}

	ImGuiKey keyCodeToImGuiKey(int32_t key_code)
	{
		// clang-format off
		switch (key_code)
		{
			case AKEYCODE_TAB:                  return ImGuiKey_Tab;
			case AKEYCODE_DPAD_LEFT:            return ImGuiKey_LeftArrow;
			case AKEYCODE_DPAD_RIGHT:           return ImGuiKey_RightArrow;
			case AKEYCODE_DPAD_UP:              return ImGuiKey_UpArrow;
			case AKEYCODE_DPAD_DOWN:            return ImGuiKey_DownArrow;
			case AKEYCODE_PAGE_UP:              return ImGuiKey_PageUp;
			case AKEYCODE_PAGE_DOWN:            return ImGuiKey_PageDown;
			case AKEYCODE_MOVE_HOME:            return ImGuiKey_Home;
			case AKEYCODE_MOVE_END:             return ImGuiKey_End;
			case AKEYCODE_INSERT:               return ImGuiKey_Insert;
			case AKEYCODE_FORWARD_DEL:          return ImGuiKey_Delete;
			case AKEYCODE_DEL:                  return ImGuiKey_Backspace;
			case AKEYCODE_SPACE:                return ImGuiKey_Space;
			case AKEYCODE_ENTER:                return ImGuiKey_Enter;
			case AKEYCODE_ESCAPE:               return ImGuiKey_Escape;
			case AKEYCODE_APOSTROPHE:           return ImGuiKey_Apostrophe;
			case AKEYCODE_COMMA:                return ImGuiKey_Comma;
			case AKEYCODE_MINUS:                return ImGuiKey_Minus;
			case AKEYCODE_PERIOD:               return ImGuiKey_Period;
			case AKEYCODE_SLASH:                return ImGuiKey_Slash;
			case AKEYCODE_SEMICOLON:            return ImGuiKey_Semicolon;
			case AKEYCODE_EQUALS:               return ImGuiKey_Equal;
			case AKEYCODE_LEFT_BRACKET:         return ImGuiKey_LeftBracket;
			case AKEYCODE_BACKSLASH:            return ImGuiKey_Backslash;
			case AKEYCODE_RIGHT_BRACKET:        return ImGuiKey_RightBracket;
			case AKEYCODE_GRAVE:                return ImGuiKey_GraveAccent;
			case AKEYCODE_CAPS_LOCK:            return ImGuiKey_CapsLock;
			case AKEYCODE_SCROLL_LOCK:          return ImGuiKey_ScrollLock;
			case AKEYCODE_NUM_LOCK:             return ImGuiKey_NumLock;
			case AKEYCODE_SYSRQ:                return ImGuiKey_PrintScreen;
			case AKEYCODE_BREAK:                return ImGuiKey_Pause;
			case AKEYCODE_NUMPAD_0:             return ImGuiKey_Keypad0;
			case AKEYCODE_NUMPAD_1:             return ImGuiKey_Keypad1;
			case AKEYCODE_NUMPAD_2:             return ImGuiKey_Keypad2;
			case AKEYCODE_NUMPAD_3:             return ImGuiKey_Keypad3;
			case AKEYCODE_NUMPAD_4:             return ImGuiKey_Keypad4;
			case AKEYCODE_NUMPAD_5:             return ImGuiKey_Keypad5;
			case AKEYCODE_NUMPAD_6:             return ImGuiKey_Keypad6;
			case AKEYCODE_NUMPAD_7:             return ImGuiKey_Keypad7;
			case AKEYCODE_NUMPAD_8:             return ImGuiKey_Keypad8;
			case AKEYCODE_NUMPAD_9:             return ImGuiKey_Keypad9;
			case AKEYCODE_NUMPAD_DOT:           return ImGuiKey_KeypadDecimal;
			case AKEYCODE_NUMPAD_DIVIDE:        return ImGuiKey_KeypadDivide;
			case AKEYCODE_NUMPAD_MULTIPLY:      return ImGuiKey_KeypadMultiply;
			case AKEYCODE_NUMPAD_SUBTRACT:      return ImGuiKey_KeypadSubtract;
			case AKEYCODE_NUMPAD_ADD:           return ImGuiKey_KeypadAdd;
			case AKEYCODE_NUMPAD_ENTER:         return ImGuiKey_KeypadEnter;
			case AKEYCODE_NUMPAD_EQUALS:        return ImGuiKey_KeypadEqual;
			case AKEYCODE_CTRL_LEFT:            return ImGuiKey_LeftCtrl;
			case AKEYCODE_SHIFT_LEFT:           return ImGuiKey_LeftShift;
			case AKEYCODE_ALT_LEFT:             return ImGuiKey_LeftAlt;
			case AKEYCODE_META_LEFT:            return ImGuiKey_LeftSuper;
			case AKEYCODE_CTRL_RIGHT:           return ImGuiKey_RightCtrl;
			case AKEYCODE_SHIFT_RIGHT:          return ImGuiKey_RightShift;
			case AKEYCODE_ALT_RIGHT:            return ImGuiKey_RightAlt;
			case AKEYCODE_META_RIGHT:           return ImGuiKey_RightSuper;
			case AKEYCODE_MENU:                 return ImGuiKey_Menu;
			case AKEYCODE_0:                    return ImGuiKey_0;
			case AKEYCODE_1:                    return ImGuiKey_1;
			case AKEYCODE_2:                    return ImGuiKey_2;
			case AKEYCODE_3:                    return ImGuiKey_3;
			case AKEYCODE_4:                    return ImGuiKey_4;
			case AKEYCODE_5:                    return ImGuiKey_5;
			case AKEYCODE_6:                    return ImGuiKey_6;
			case AKEYCODE_7:                    return ImGuiKey_7;
			case AKEYCODE_8:                    return ImGuiKey_8;
			case AKEYCODE_9:                    return ImGuiKey_9;
			case AKEYCODE_A:                    return ImGuiKey_A;
			case AKEYCODE_B:                    return ImGuiKey_B;
			case AKEYCODE_C:                    return ImGuiKey_C;
			case AKEYCODE_D:                    return ImGuiKey_D;
			case AKEYCODE_E:                    return ImGuiKey_E;
			case AKEYCODE_F:                    return ImGuiKey_F;
			case AKEYCODE_G:                    return ImGuiKey_G;
			case AKEYCODE_H:                    return ImGuiKey_H;
			case AKEYCODE_I:                    return ImGuiKey_I;
			case AKEYCODE_J:                    return ImGuiKey_J;
			case AKEYCODE_K:                    return ImGuiKey_K;
			case AKEYCODE_L:                    return ImGuiKey_L;
			case AKEYCODE_M:                    return ImGuiKey_M;
			case AKEYCODE_N:                    return ImGuiKey_N;
			case AKEYCODE_O:                    return ImGuiKey_O;
			case AKEYCODE_P:                    return ImGuiKey_P;
			case AKEYCODE_Q:                    return ImGuiKey_Q;
			case AKEYCODE_R:                    return ImGuiKey_R;
			case AKEYCODE_S:                    return ImGuiKey_S;
			case AKEYCODE_T:                    return ImGuiKey_T;
			case AKEYCODE_U:                    return ImGuiKey_U;
			case AKEYCODE_V:                    return ImGuiKey_V;
			case AKEYCODE_W:                    return ImGuiKey_W;
			case AKEYCODE_X:                    return ImGuiKey_X;
			case AKEYCODE_Y:                    return ImGuiKey_Y;
			case AKEYCODE_Z:                    return ImGuiKey_Z;
			case AKEYCODE_F1:                   return ImGuiKey_F1;
			case AKEYCODE_F2:                   return ImGuiKey_F2;
			case AKEYCODE_F3:                   return ImGuiKey_F3;
			case AKEYCODE_F4:                   return ImGuiKey_F4;
			case AKEYCODE_F5:                   return ImGuiKey_F5;
			case AKEYCODE_F6:                   return ImGuiKey_F6;
			case AKEYCODE_F7:                   return ImGuiKey_F7;
			case AKEYCODE_F8:                   return ImGuiKey_F8;
			case AKEYCODE_F9:                   return ImGuiKey_F9;
			case AKEYCODE_F10:                  return ImGuiKey_F10;
			case AKEYCODE_F11:                  return ImGuiKey_F11;
			case AKEYCODE_F12:                  return ImGuiKey_F12;
			case AKEYCODE_BACK:                 return ImGuiKey_AppBack;
			default:                            return ImGuiKey_None;
		}
		// clang-format on
	}

}

void ImGuiAndroidInput::init(ANativeWindow *window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	window_ = window;

	ImGuiIO &io = ImGui::GetIO();
	io.BackendPlatformName = "nCine_Android";

	ImGuiPlatformIO& platformIo = ImGui::GetPlatformIO();
	platformIo.Platform_SetClipboardTextFn = setClipboardText;
	platformIo.Platform_GetClipboardTextFn = clipboardText;
}

void ImGuiAndroidInput::shutdown()
{
	ImGuiIO &io = ImGui::GetIO();
	io.BackendPlatformName = nullptr;

	ImGui::DestroyContext();
}

void ImGuiAndroidInput::newFrame()
{
	ImGuiIO &io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! Missing call to ImGuiDrawing::buildFonts() function?");
	io.DeltaTime = theApplication().frameTime();

	// Setup display size (every frame to accommodate for window resizing)
	const int32_t windowWidth = ANativeWindow_getWidth(window_);
	const int32_t windowHeight = ANativeWindow_getHeight(window_);
	if (windowWidth > 0 && windowHeight > 0)
	{
		const int displayWidth = windowWidth;
		const int displayHeight = windowHeight;

		io.DisplaySize = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight));
		if (windowWidth > 0 && windowHeight > 0)
			io.DisplayFramebufferScale = ImVec2(static_cast<float>(displayWidth) / windowWidth, static_cast<float>(displayHeight) / windowHeight);
	}

	// Update game controllers (if enabled and available)
	if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)
		imGuiJoyMappedInput();
}

bool ImGuiAndroidInput::processEvent(const AInputEvent *event)
{
	if (inputEnabled_ == false)
		return false;

	ImGuiIO &io = ImGui::GetIO();
	const int32_t eventType = AInputEvent_getType(event);
	const int32_t eventSource = AInputEvent_getSource(event);

	io.ConfigFlags &= ~ImGuiConfigFlags_IsTouchScreen;
	if ((eventSource & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN)
		io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;

	switch (eventType)
	{
		case AINPUT_EVENT_TYPE_KEY:
		{
			const int32_t eventKeyCode = AKeyEvent_getKeyCode(event);
			const int32_t eventScanCode = AKeyEvent_getScanCode(event);
			const int32_t eventAction = AKeyEvent_getAction(event);
			const int32_t eventMetaState = AKeyEvent_getMetaState(event);

			io.AddKeyEvent(ImGuiMod_Ctrl, (eventMetaState & AMETA_CTRL_ON) != 0);
			io.AddKeyEvent(ImGuiMod_Shift, (eventMetaState & AMETA_SHIFT_ON) != 0);
			io.AddKeyEvent(ImGuiMod_Alt, (eventMetaState & AMETA_ALT_ON) != 0);
			io.AddKeyEvent(ImGuiMod_Super, (eventMetaState & AMETA_META_ON) != 0);

			switch (eventAction)
			{
				// FIXME: AKEY_EVENT_ACTION_DOWN and AKEY_EVENT_ACTION_UP occur at once as soon as a touch pointer
				// goes up from a key. We use a simple key event queue/ and process one event per key per frame in
				// ImGui_ImplAndroid_NewFrame()...or consider using IO queue, if suitable: https://github.com/ocornut/imgui/issues/2787
				case AKEY_EVENT_ACTION_DOWN:
				case AKEY_EVENT_ACTION_UP:
				{
					const ImGuiKey key = keyCodeToImGuiKey(eventKeyCode);
					if (key != ImGuiKey_None)
					{
						io.AddKeyEvent(key, eventAction == AKEY_EVENT_ACTION_DOWN);
						io.SetKeyEventNativeData(key, eventKeyCode, eventScanCode);
					}

					break;
				}
				default:
					break;
			}

			if (eventAction == AKEY_EVENT_ACTION_DOWN && (eventMetaState & AMETA_CTRL_ON) == 0)
			{
				AndroidJniClass_KeyEvent keyEvent(eventType, eventKeyCode);
				if (keyEvent.isPrintingKey())
				{
					const int unicodeKey = keyEvent.getUnicodeChar(eventMetaState);
					io.AddInputCharacter(unicodeKey);
				}
			}

			if ((eventSource & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE)
			{
				LOGE_X("KEYCODE: %u", eventKeyCode);
				if (eventKeyCode == AKEYCODE_BACK)
				{
					io.AddMouseButtonEvent(1, eventAction == AKEY_EVENT_ACTION_DOWN);
					LOGE("BACK BUTTON");
				}
				else if (eventKeyCode == AKEYCODE_FORWARD)
				{
					io.AddMouseButtonEvent(2, eventAction == AKEY_EVENT_ACTION_DOWN);
					LOGE("FORWARD BUTTON");
				}
			}

			return true;
		}
		case AINPUT_EVENT_TYPE_MOTION:
		{
			int32_t eventAction = AMotionEvent_getAction(event);
			const int32_t eventPointerIndex = (eventAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			eventAction &= AMOTION_EVENT_ACTION_MASK;

			switch (AMotionEvent_getToolType(event, eventPointerIndex))
			{
				case AMOTION_EVENT_TOOL_TYPE_MOUSE:
					io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
					break;
				case AMOTION_EVENT_TOOL_TYPE_STYLUS:
				case AMOTION_EVENT_TOOL_TYPE_ERASER:
					io.AddMouseSourceEvent(ImGuiMouseSource_Pen);
					break;
				case AMOTION_EVENT_TOOL_TYPE_FINGER:
				default:
					io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
					break;
			}

			switch (eventAction)
			{
				case AMOTION_EVENT_ACTION_DOWN:
				case AMOTION_EVENT_ACTION_UP:
				{
					// Physical mouse buttons (and probably other physical devices) also invoke the actions AMOTION_EVENT_ACTION_DOWN/_UP,
					// but we have to process them separately to identify the actual button pressed. This is done below via
					// AMOTION_EVENT_ACTION_BUTTON_PRESS/_RELEASE. Here, we only process "FINGER" input (and "UNKNOWN", as a fallback).
					const int toolType = AMotionEvent_getToolType(event, eventPointerIndex);
					if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER || toolType == AMOTION_EVENT_TOOL_TYPE_UNKNOWN)
					{
						io.AddMousePosEvent(AMotionEvent_getX(event, eventPointerIndex), AMotionEvent_getY(event, eventPointerIndex));
						io.AddMouseButtonEvent(0, eventAction == AMOTION_EVENT_ACTION_DOWN);
					}
					break;
				}
				case AMOTION_EVENT_ACTION_BUTTON_PRESS:
				case AMOTION_EVENT_ACTION_BUTTON_RELEASE:
				{
					const int32_t buttonState = AMotionEvent_getButtonState(event);
					io.AddMouseButtonEvent(0, (buttonState & AMOTION_EVENT_BUTTON_PRIMARY) != 0);
					io.AddMouseButtonEvent(1, (buttonState & AMOTION_EVENT_BUTTON_SECONDARY) != 0);
					io.AddMouseButtonEvent(2, (buttonState & AMOTION_EVENT_BUTTON_TERTIARY) != 0);
					break;
				}
				case AMOTION_EVENT_ACTION_HOVER_MOVE: // Hovering: Tool moves while NOT pressed (such as a physical mouse)
				case AMOTION_EVENT_ACTION_MOVE: // Touch pointer moves while DOWN
					io.AddMousePosEvent(AMotionEvent_getX(event, eventPointerIndex), AMotionEvent_getY(event, eventPointerIndex));
					break;
				case AMOTION_EVENT_ACTION_SCROLL:
					io.AddMouseWheelEvent(AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HSCROLL, eventPointerIndex), AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_VSCROLL, eventPointerIndex));
					break;
				default:
					break;
			}
			return true;
		}
		default:
			break;
	}
	return false;
}

}
