#ifndef CLASS_NCINE_IINPUTMANAGER
#define CLASS_NCINE_IINPUTMANAGER

#include "InputEvents.h"

namespace ncine {

class IInputEventHandler;
class JoyMapping;
template <class T> class Vector2;
typedef class Vector2<float> Vector2f;

/// The interface class for parsing and dispatching input events
class DLL_PUBLIC IInputManager
{
  public:
	enum MouseCursorMode
	{
		/// Mouse cursor behaves normally
		MOUSE_CURSOR_NORMAL,
		/// Mouse cursor is hidden but behaves normally
		MOUSE_CURSOR_HIDDEN,
		/// Mouse cursor is hidden and locked to the window
		MOUSE_CURSOR_DISABLED
	};

	static const int MaxNumJoysticks;
	static const short int MaxAxisValue = 32767;

	// From `XInput.h` in DirectX SDK
	static const float LeftStickDeadZone;
	static const float RightStickDeadZone;
	static const float TriggerDeadZone;

	IInputManager() { }
	virtual ~IInputManager() { }
	/// Sets the input event handler for the manager
	static void setHandler(IInputEventHandler *inputEventHandler);

	/// Returns current mouse state
	virtual const MouseState &mouseState() = 0;
	/// Returns current keyboard state
	virtual const KeyboardState &keyboardState() const = 0;

	/// Returns true if the specified joystick is connected
	virtual bool isJoyPresent(int joyId) const = 0;
	/// Returns the name of the specified joystick
	virtual const char *joyName(int joyId) const = 0;
	/// Returns the GUID of the specified joystick
	virtual const char *joyGuid(int joyId) const = 0;
	/// Returns the number of available buttons for the specified joystick
	virtual int joyNumButtons(int joyId) const = 0;
	/// Returns the number of available axes for the specified joystick
	virtual int joyNumAxes(int joyId) const = 0;
	/// Returns the state of the joystick
	virtual const JoystickState &joystickState(int joyId) const = 0;

	/// Returns `true` if the joystick has a valid mapping configuration
	bool isJoyMapped(int joyId) const;
	/// Returns the state of the mapped joystick
	const JoyMappedState &joyMappedState(int joyId) const;
	/// Modify the joystick axis vector to account for a dead zone
	void deadZoneNormalize(Vector2f &joyVector, float deadZoneValue) const;

	/// Adds joystick mapping configurations from a text file
	void addJoyMappingsFromFile(const char *filename);
	/// Adds joystick mapping configurations from a strings array terminated by a `NULL` pointer
	void addJoyMappingsFromStrings(const char **mappingStrings);

	/// Returns current mouse cursor mode
	inline MouseCursorMode mouseCursorMode() const { return mouseCursorMode_; }
	/// Sets the mouse cursor mode
	virtual void setMouseCursorMode(MouseCursorMode mode) = 0;

  protected:
	static IInputEventHandler *inputEventHandler_;
	static MouseCursorMode mouseCursorMode_;

	static JoyMapping joyMapping_;
};

}

#endif
