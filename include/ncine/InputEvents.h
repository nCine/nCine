#ifndef NCINE_INPUTEVENTS
#define NCINE_INPUTEVENTS

#include "common_defines.h"
#include "Keys.h"

namespace ncine {

/// Information about a screen touch event
class DLL_PUBLIC TouchEvent
{
  public:
	static const unsigned int MaxPointers = 10;
	struct Pointer
	{
		int id;
		float x, y;
		float pressure;
	};

	TouchEvent()
	    : count(0), actionIndex(-1) {}

	unsigned int count;
	int actionIndex;
	Pointer pointers[MaxPointers];

	inline int findPointerIndex(int pointerId) const
	{
		int pointerIndex = -1;
		for (unsigned int i = 0; i < count && i < MaxPointers; i++)
		{
			if (pointers[i].id == pointerId)
			{
				pointerIndex = i;
				break;
			}
		}
		return pointerIndex;
	}
};

#ifdef __ANDROID__
/// Information about an accelerometer event
class DLL_PUBLIC AccelerometerEvent
{
  public:
	AccelerometerEvent()
	    : x(0.0f), y(0.0f), z(0.0f) {}

	float x, y, z;
};
#endif

/// Information about mouse state
class DLL_PUBLIC MouseState
{
  public:
	/// Pointer position on the X axis
	int x;
	/// Pointer position on the Y axis
	int y;

	virtual bool isLeftButtonDown() const = 0;
	virtual bool isMiddleButtonDown() const = 0;
	virtual bool isRightButtonDown() const = 0;
	virtual bool isFourthButtonDown() const = 0;
	virtual bool isFifthButtonDown() const = 0;
};

/// Information about a mouse event
class DLL_PUBLIC MouseEvent
{
  public:
	/// Pointer position on the X axis
	int x;
	/// Pointer position on the Y axis
	int y;

	virtual bool isLeftButton() const = 0;
	virtual bool isMiddleButton() const = 0;
	virtual bool isRightButton() const = 0;
	virtual bool isFourthButton() const = 0;
	virtual bool isFifthButton() const = 0;
};

/// Information about a scroll event (mouse wheel, touchpad gesture, etc.)
class DLL_PUBLIC ScrollEvent
{
  public:
	/// Scroll offset on the X axis
	float x;
	/// Scroll offset on the Y axis
	float y;
};

/// Information about keyboard state
class DLL_PUBLIC KeyboardState
{
  public:
	/// Returns 'true' if the specified key is down
	virtual bool isKeyDown(KeySym key) const = 0;
};

/// Information about a keyboard event
class DLL_PUBLIC KeyboardEvent
{
  public:
	/// Key scan code
	int scancode;
	/// Key symbol code
	KeySym sym;
	/// Key modifiers mask
	int mod;

	KeyboardEvent()
	    : scancode(0), sym(KeySym::UNKNOWN), mod(0) {}
};

/// Information about a text input event
class DLL_PUBLIC TextInputEvent
{
  public:
	/// Unicode code point encoded in UTF-8
	char text[5];

	TextInputEvent()
	{
		text[0] = '\0';
	}
};

/// Information about a drag'n'drop event
class DLL_PUBLIC DropEvent
{
  public:
	static const int MaxNumPaths = 16;
	static const int MaxPathLength = 256;
	unsigned int numPaths;
	char paths[MaxNumPaths][MaxPathLength];

	DropEvent()
	    : numPaths(0)
	{
		for (unsigned int i = 0; i < MaxNumPaths; i++)
			paths[i][0] = '\0';
	}
};

/// Information about the joystick state
class DLL_PUBLIC JoystickState
{
  public:
	/// Returns 'true' if the specified button is pressed
	virtual bool isButtonPressed(int buttonId) const = 0;
	/// Returns the state of the specified hat
	virtual unsigned char hatState(int hatId) const = 0;
	/// Returns a value between -32768 and 32767 for a joystick axis
	virtual short int axisValue(int axisId) const = 0;
	/// Returns a normalized value between -1.0 and 1.0 for a joystick axis
	virtual float axisNormValue(int axisId) const = 0;
};

/// Information about a joystick button event
class DLL_PUBLIC JoyButtonEvent
{
  public:
	/// Joystick id
	int joyId;
	/// Button id
	int buttonId;
};

/// A structure containing joystick hat values
struct HatState
{
	enum
	{
		CENTERED = 0,
		UP = 1,
		RIGHT = 2,
		DOWN = 4,
		LEFT = 8,
		RIGHT_UP = RIGHT | UP,
		RIGHT_DOWN = RIGHT | DOWN,
		LEFT_UP = LEFT | UP,
		LEFT_DOWN = LEFT | DOWN
	};
};

/// Information about a joystick hat event
class DLL_PUBLIC JoyHatEvent
{
  public:
	/// Joystick id
	int joyId;
	/// Hat id
	int hatId;
	/// Hat position state
	unsigned char hatState;
};

/// Information about a joystick axis event
class DLL_PUBLIC JoyAxisEvent
{
  public:
	/// Joystick id
	int joyId;
	/// Axis id
	int axisId;
	/// Axis value between -32768 and 32767
	short int value;
	/// Axis value normalized between -1.0f and 1.0f
	float normValue;
};

/// Information about a joystick connection event
class DLL_PUBLIC JoyConnectionEvent
{
  public:
	/// Joystick id
	int joyId;
};

enum class ButtonName : short int
{
	UNKNOWN = -1,
	A = 0,
	B,
	X,
	Y,
	BACK,
	GUIDE,
	START,
	LSTICK,
	RSTICK,
	LBUMPER,
	RBUMPER,
	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT
};

enum class AxisName : short int
{
	UNKNOWN = -1,
	LX = 0,
	LY,
	RX,
	RY,
	LTRIGGER,
	RTRIGGER,
};

/// Information about a mapped joystick state
class DLL_PUBLIC JoyMappedState
{
  public:
	/// The number of joystick buttons with a mapping name
	static const unsigned int NumButtons = 15;
	/// The number of joystick axes with a mapping name
	static const unsigned int NumAxes = 6;

	virtual ~JoyMappedState() {}

	/// Returns 'true' if the specified button is pressed
	virtual bool isButtonPressed(ButtonName name) const = 0;
	/// Returns the value of the specified axis
	virtual float axisValue(AxisName name) const = 0;
};

/// Information about a joystick mapped button event
class DLL_PUBLIC JoyMappedButtonEvent
{
  public:
	/// Joystick id
	int joyId;
	/// Button name
	ButtonName buttonName;
};

/// Information about a joystick mapped axis event
class DLL_PUBLIC JoyMappedAxisEvent
{
  public:
	/// Joystick id
	int joyId;
	/// Axis name
	AxisName axisName;
	/// Axis value between its minimum and maximum
	float value;
};

}

#endif
