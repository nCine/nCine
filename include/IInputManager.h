#ifndef CLASS_NCIINPUTMANAGER
#define CLASS_NCIINPUTMANAGER

#if defined(__ANDROID__)
	#include "AndroidKeys.h"
#elif defined(WITH_SDL)
	#include "SdlKeys.h"
#elif defined(WITH_GLFW)
	#include "GlfwKeys.h"
#endif

namespace ncine {

class IInputEventHandler;

#ifdef __ANDROID__
class TouchEvent
{
  public:
	TouchEvent() : count(0), id(0), id2(1), x(0.0f), y(0.0f), x2(0.0f), y2(0.0f) { }

	int count;
	int id, id2;
	float x, y, x2, y2;
};

class AccelerometerEvent
{
  public:
	AccelerometerEvent(): x(0.0f), y(0.0f), z(0.0f) { }

	float x, y, z;
};
#else
/// Information about mouse state
class MouseState
{
  public:
	/// Pointer position on the X axis
	int x;
	/// Pointer position on the Y axis
	int y;

	virtual bool isLeftButtonDown() const = 0;
	virtual bool isMiddleButtonDown() const = 0;
	virtual bool isRightButtonDown() const = 0;
	virtual bool isWheelUpButtonDown() const = 0;
	virtual bool isWheelDownButtonDown() const = 0;
};

/// Information about a mouse event
class MouseEvent
{
  public:
	/// Pointer position on the X axis
	int x;
	/// Pointer position on the Y axis
	int y;

	virtual bool isLeftButton() const = 0;
	virtual bool isMiddleButton() const = 0;
	virtual bool isRightButton() const = 0;
	virtual bool isWheelUpButton() const = 0;
	virtual bool isWheelDownButton() const = 0;
};

/// Information about keyboard state
class KeyboardState
{
  public:
	KeyboardState() { }

	virtual bool isKeyDown(KeySym key) const = 0;
};
#endif

/// Information about a keyboard event
class KeyboardEvent
{
  public:
	/// Key scan code
	int scancode;
	/// Key symbol code
	KeySym sym;
	/// Key modifiers mask
	int mod;
	/// Unicode character
	unsigned short int unicode;

	KeyboardEvent() : scancode(0), sym(KEY_UNKNOWN), mod(0), unicode(0) { }
};

/// Information about a joystick axis event
class JoyAxisEvent
{
  public:
	int joyId;
	int axisId;
	short int value;
	float normValue;
};

/// Information about a joystick button event
class JoyButtonEvent
{
  public:
	int joyId;
	int buttonId;
};

/// The interface class for parsing and dispatching input events
class IInputManager
{
  public:
	virtual ~IInputManager() { }

	inline static void setHandler(IInputEventHandler *inputEventHandler)
	{
		inputEventHandler_ = inputEventHandler;
	}

#ifndef __ANDROID__
	virtual const MouseState& mouseState() = 0;
	virtual const KeyboardState& keyboardState() const = 0;
#endif

	static short int MaxAxisValue;
	/// Returns true if the specified joystick is connected
	virtual bool isJoyPresent(int joyId) const = 0;
	/// Returns the name of the specified joystick
	virtual const char* joyName(int joyId) const = 0;
	/// Returns the number of available buttons for the specified joystick
	virtual int joyNumButtons(int joyId) const = 0;
	/// Returns the number of available axes for the specified joystick
	virtual int joyNumAxes(int joyId) const = 0;
	/// Returns the state of a joystick button
	virtual bool isJoyButtonPressed(int joyId, int buttonId) const = 0;
	/// Returns a value between -32768 and 32767 for a joystick axis
	virtual short int joyAxisValue(int joyId, int axisId) const = 0;
	/// Returns a normalized value between -1 and 1 for a joystick axis
	virtual float joyAxisNormValue(int joyId, int axisId) const = 0;

  protected:
	IInputManager() { }
	static IInputEventHandler *inputEventHandler_;
};

}

#endif
