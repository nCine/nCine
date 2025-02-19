#include "imgui.h"
#include "Application.h"
#include "IInputManager.h"

namespace ncine {

bool imGuiJoyMappedInput()
{
	ImGuiIO &io = ImGui::GetIO();

	if (theApplication().inputManager().isJoyMapped(0))
	{
		const JoyMappedState &state = theApplication().inputManager().joyMappedState(0);

		// clang-format off
		#define IM_SATURATE(V)                      (V < 0.0f ? 0.0f : V > 1.0f ? 1.0f : V)
		#define MAP_BUTTON(KEY_NO, BUTTON_NO)       { io.AddKeyEvent(KEY_NO, state.isButtonDown(BUTTON_NO)); }
		#define MAP_ANALOG(KEY_NO, AXIS_NO, V0, V1) { float vn = static_cast<float>(state.axisValue(AXIS_NO) - V0) / static_cast<float>(V1 - V0);\
		                                                    vn = IM_SATURATE(vn); io.AddKeyAnalogEvent(KEY_NO, vn > 0.1f, vn); }
		const int thumbDeadZone = 8000; // SDL_gamecontroller.h suggests using this value.
		MAP_BUTTON(ImGuiKey_GamepadStart,           ButtonName::START);
		MAP_BUTTON(ImGuiKey_GamepadBack,            ButtonName::BACK);
		MAP_BUTTON(ImGuiKey_GamepadFaceDown,        ButtonName::A);              // Xbox A, PS Cross
		MAP_BUTTON(ImGuiKey_GamepadFaceRight,       ButtonName::B);              // Xbox B, PS Circle
		MAP_BUTTON(ImGuiKey_GamepadFaceLeft,        ButtonName::X);              // Xbox X, PS Square
		MAP_BUTTON(ImGuiKey_GamepadFaceUp,          ButtonName::Y);              // Xbox Y, PS Triangle
		MAP_BUTTON(ImGuiKey_GamepadDpadLeft,        ButtonName::DPAD_LEFT);
		MAP_BUTTON(ImGuiKey_GamepadDpadRight,       ButtonName::DPAD_RIGHT);
		MAP_BUTTON(ImGuiKey_GamepadDpadUp,          ButtonName::DPAD_UP);
		MAP_BUTTON(ImGuiKey_GamepadDpadDown,        ButtonName::DPAD_DOWN);
		MAP_BUTTON(ImGuiKey_GamepadL1,              ButtonName::LBUMPER);
		MAP_BUTTON(ImGuiKey_GamepadR1,              ButtonName::RBUMPER);
		MAP_ANALOG(ImGuiKey_GamepadL2,              AxisName::LTRIGGER,  0.0f, 32767);
		MAP_ANALOG(ImGuiKey_GamepadR2,              AxisName::RTRIGGER, 0.0f, 32767);
		MAP_BUTTON(ImGuiKey_GamepadL3,              ButtonName::LSTICK);
		MAP_BUTTON(ImGuiKey_GamepadR3,              ButtonName::RSTICK);
		MAP_ANALOG(ImGuiKey_GamepadLStickLeft,      AxisName::LX, -thumbDeadZone, -32768);
		MAP_ANALOG(ImGuiKey_GamepadLStickRight,     AxisName::LX, +thumbDeadZone, +32767);
		MAP_ANALOG(ImGuiKey_GamepadLStickUp,        AxisName::LY, -thumbDeadZone, -32768);
		MAP_ANALOG(ImGuiKey_GamepadLStickDown,      AxisName::LY, +thumbDeadZone, +32767);
		MAP_ANALOG(ImGuiKey_GamepadRStickLeft,      AxisName::RX, -thumbDeadZone, -32768);
		MAP_ANALOG(ImGuiKey_GamepadRStickRight,     AxisName::RX, +thumbDeadZone, +32767);
		MAP_ANALOG(ImGuiKey_GamepadRStickUp,        AxisName::RY, -thumbDeadZone, -32768);
		MAP_ANALOG(ImGuiKey_GamepadRStickDown,      AxisName::RY, +thumbDeadZone, +32767);
		#undef MAP_BUTTON
		#undef MAP_ANALOG
		// clang-format on

		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		return true;
	}

	io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
	return false;
}

}
