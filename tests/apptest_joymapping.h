#ifndef APPTEST_JOYMAPPING
#define APPTEST_JOYMAPPING

#include "Application.h"
#include "IInputEventHandler.h"
#include "Vector2.h"

namespace nc = ncine;

namespace {

enum MappedAxis
{
	AXIS_LX = 0,
	AXIS_LY,
	AXIS_LTRIGGER,
	AXIS_RX,
	AXIS_RY,
	AXIS_RTRIGGER
};

enum MappedButton
{
	BUTTON_A = 0,
	BUTTON_B,
	BUTTON_X,
	BUTTON_Y,
	BUTTON_LBUMPER,
	BUTTON_RBUMPER
};

// Only tested with the Xbox 360 Controller
#ifdef _WIN32
	const unsigned int AxesMapping[] = { 0, 1, 4, 2, 3, 5 };
	const int InvertAxis[] = { 1, -1, 1, -1, 1, 1 };
	const unsigned int ButtonsMapping[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
#else
	const unsigned int AxesMapping[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	const int InvertAxis[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
	const unsigned int ButtonsMapping[] = { 0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 8 };
#endif

// From `XInput.h` in DirectX SDK
const float LeftStickDeadZone = 7849 / 32767.0f;
const float RightStickDeadZone = 8689 / 32767.0f;
const float TriggerDeadZone = 30 / 32767.0f;

inline float mappedAxisNormValue(int joyId, MappedAxis axisName)
{
	int axisId = static_cast<int>(axisName);
	int mappedAxisId = AxesMapping[axisId];
	int mappedAxisInversion = InvertAxis[mappedAxisId];

	return nc::theApplication().inputManager().joyAxisNormValue(joyId, mappedAxisId) * mappedAxisInversion;
}

inline bool mappedButtonPressed(int joyId, MappedButton buttonName)
{
	int buttonId = static_cast<int>(buttonName);
	unsigned int mappedButtonId = ButtonsMapping[buttonId];

	return nc::theApplication().inputManager().isJoyButtonPressed(joyId, mappedButtonId);
}

inline bool isAxis(const nc::JoyAxisEvent &event, MappedAxis axisName)
{
	int axisId = static_cast<int>(axisName);
	int mappedAxisId = AxesMapping[axisId];

	return (event.axisId == mappedAxisId);
}

inline float normValue(const nc::JoyAxisEvent &event, MappedAxis axisName)
{
	int axisId = static_cast<int>(axisName);
	int mappedAxisId = AxesMapping[axisId];
	int mappedAxisInversion = InvertAxis[mappedAxisId];

	return event.normValue * mappedAxisInversion;
}

inline bool isButton(const nc::JoyButtonEvent &event, MappedButton buttonName)
{
	int buttonId = static_cast<int>(buttonName);
	int mappedButtonId = ButtonsMapping[buttonId];

	return (event.buttonId == mappedButtonId);
}

inline void deadZoneNormalize(nc::Vector2f &joyVector, float deadZoneValue)
{
	deadZoneValue = nc::clamp(deadZoneValue, 0.0f, 1.0f);

	if (joyVector.length() <= deadZoneValue)
	{
		joyVector = nc::Vector2f::Zero;
	}
	else
	{
		float normalizedLength = (joyVector.length() - deadZoneValue) / (1.0f - deadZoneValue);
		normalizedLength = nc::clamp(normalizedLength, 0.0f, 1.0f);
		joyVector = joyVector.normalize() * normalizedLength;
	}
}

}

#endif
