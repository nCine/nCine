#include "IInputManager.h"
#include "JoyMapping.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

IInputEventHandler *IInputManager::inputEventHandler_ = nullptr;
IInputManager::MouseCursorMode IInputManager::mouseCursorMode_ = IInputManager::MOUSE_CURSOR_NORMAL;
JoyMapping IInputManager::joyMapping_;

// From `XInput.h` in DirectX SDK
const float IInputManager::LeftStickDeadZone = 7849 / 32767.0f;
const float IInputManager::RightStickDeadZone = 8689 / 32767.0f;
const float IInputManager::TriggerDeadZone = 30 / 32767.0f;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void IInputManager::setHandler(IInputEventHandler *inputEventHandler)
{
	inputEventHandler_ = inputEventHandler;
	joyMapping_.setHandler(inputEventHandler);
}

/*! \note Joystick will stay mapped in the`onJoyConnected()` and `onJoyDisconnected()` callbacks */
bool IInputManager::isJoyMapped(int joyId) const
{
	return joyMapping_.isJoyMapped(joyId);
}

const JoyMappedState &IInputManager::joyMappedState(int joyId) const
{
	return joyMapping_.joyMappedState(joyId);
}

void IInputManager::deadZoneNormalize(Vector2f &joyVector, float deadZoneValue) const
{
	return joyMapping_.deadZoneNormalize(joyVector, deadZoneValue);
}

void IInputManager::addJoyMappingsFromFile(const char *filename)
{
	joyMapping_.addMappingsFromFile(filename);
}

void IInputManager::addJoyMappingsFromStrings(const char **mappingStrings)
{
	joyMapping_.addMappingsFromStrings(mappingStrings);
}

}
