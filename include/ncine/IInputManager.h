#ifndef CLASS_NCINE_IINPUTMANAGER
#define CLASS_NCINE_IINPUTMANAGER

#include "InputEvents.h"

namespace ncine {

class IInputEventHandler;
class JoyMapping;
template <class T>
class Vector2;
using Vector2f = Vector2<float>;

/// The interface class for parsing and dispatching input events
class DLL_PUBLIC IInputManager
{
  public:
	enum class MouseCursorMode
	{
		/// Mouse cursor behaves normally
		NORMAL,
		/// Mouse cursor is hidden but behaves normally
		HIDDEN,
		/// Mouse cursor is hidden and locked to the window
		/*! \note Mouse movement will be relative if supported (with no acceleration and no scaling) */
		DISABLED
	};

	static const int MaxNumJoysticks;
	static const short int MaxAxisValue = 32767;

	// From `XInput.h` in DirectX SDK
	static const float LeftStickDeadZone;
	static const float RightStickDeadZone;
	static const float TriggerDeadZone;

	IInputManager() {}
	virtual ~IInputManager() {}
	/// Gets the current input event handler for the manager
	static inline IInputEventHandler *handler() { return inputEventHandler_; }
	/// Sets the input event handler for the manager
	static void setHandler(IInputEventHandler *inputEventHandler);

	/// Returns current mouse state
	virtual const MouseState &mouseState() const = 0;
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
	/// Returns the number of available hats for the specified joystick
	virtual int joyNumHats(int joyId) const = 0;
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
	/// Adds joystick mapping configurations from a strings array terminated by a `nullptr`
	void addJoyMappingsFromStrings(const char **mappingStrings);
	/// Returns the current number of valid joystick mappings
	unsigned int numJoyMappings() const;

	/// Returns current mouse cursor mode
	inline MouseCursorMode mouseCursorMode() const { return mouseCursorMode_; }
	/// Sets the mouse cursor mode
	virtual void setMouseCursorMode(MouseCursorMode mode);

  protected:
	static IInputEventHandler *inputEventHandler_;
	static MouseCursorMode mouseCursorMode_;

	static JoyMapping joyMapping_;
	static DropEvent dropEvent_;
};

}

#endif
