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

		io.NavInputs[ImGuiNavInput_Activate] = state.isButtonPressed(ButtonName::A) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_Cancel] = state.isButtonPressed(ButtonName::B) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_Menu] = state.isButtonPressed(ButtonName::X) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_Input] = state.isButtonPressed(ButtonName::Y) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_FocusPrev] = state.isButtonPressed(ButtonName::LBUMPER) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_FocusNext] = state.isButtonPressed(ButtonName::RBUMPER) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_TweakSlow] = state.isButtonPressed(ButtonName::LBUMPER) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_TweakFast] = state.isButtonPressed(ButtonName::RBUMPER) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_DpadLeft] = state.isButtonPressed(ButtonName::DPAD_LEFT) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_DpadRight] = state.isButtonPressed(ButtonName::DPAD_RIGHT) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_DpadUp] = state.isButtonPressed(ButtonName::DPAD_UP) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_DpadDown] = state.isButtonPressed(ButtonName::DPAD_DOWN) ? 1.0f : 0.0f;
		io.NavInputs[ImGuiNavInput_LStickLeft] = -state.axisValue(AxisName::LX) > 0.15f ? -state.axisValue(AxisName::LX) : 0.0f;
		io.NavInputs[ImGuiNavInput_LStickRight] = state.axisValue(AxisName::LX) > 0.15f ? state.axisValue(AxisName::LX) : 0.0f;
		io.NavInputs[ImGuiNavInput_LStickUp] = -state.axisValue(AxisName::LY) > 0.15f ? -state.axisValue(AxisName::LY) : 0.0f;
		io.NavInputs[ImGuiNavInput_LStickDown] = state.axisValue(AxisName::LY) > 0.15f ? state.axisValue(AxisName::LY) : 0.0f;

		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		return true;
	}

	io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
	return false;
}

}
