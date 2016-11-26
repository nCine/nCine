#ifndef CLASS_NCIINPUTMANAGER
#define CLASS_NCIINPUTMANAGER

#include "common_defines.h"
#include "Keys.h"

namespace ncine {

class IInputEventHandler;

#ifdef __ANDROID__
class DLL_PUBLIC TouchEvent
{
  public:
	static const unsigned int MaxPointers = 10;
	struct Pointer
	{
		int id;
		float x, y;
	};

	TouchEvent() : count(0) { }

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

class DLL_PUBLIC AccelerometerEvent
{
  public:
	AccelerometerEvent(): x(0.0f), y(0.0f), z(0.0f) { }

	float x, y, z;
};
#else
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
	virtual bool isKeyDown(KeySym key) const = 0;
};
#endif

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
	/// Unicode character
	unsigned short int unicode;

	KeyboardEvent() : scancode(0), sym(KEY_UNKNOWN), mod(0), unicode(0) { }
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
	/// Axis value normalized between -1.0f and 1.0
	float normValue;
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

/// Information about a joystick connection event
class DLL_PUBLIC JoyConnectionEvent
{
  public:
	/// Joystick id
	int joyId;
};

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
		/*! \note Does not work properly when using SDL 1.2.x */
		MOUSE_CURSOR_DISABLED
	};

	virtual ~IInputManager() { }
	inline static void setHandler(IInputEventHandler *inputEventHandler)
	{
		inputEventHandler_ = inputEventHandler;
	}

#ifndef __ANDROID__
	virtual const MouseState &mouseState() = 0;
	virtual const KeyboardState &keyboardState() const = 0;
#endif

	static short int MaxAxisValue;
	/// Returns true if the specified joystick is connected
	virtual bool isJoyPresent(int joyId) const = 0;
	/// Returns the name of the specified joystick
	virtual const char *joyName(int joyId) const = 0;
	/// Returns the number of available buttons for the specified joystick
	virtual int joyNumButtons(int joyId) const = 0;
	/// Returns the number of available axes for the specified joystick
	virtual int joyNumAxes(int joyId) const = 0;
	/// Returns the state of a joystick button
	virtual bool isJoyButtonPressed(int joyId, int buttonId) const = 0;
	/// Returns a value between -32768 and 32767 for a joystick axis
	virtual short int joyAxisValue(int joyId, int axisId) const = 0;
	/// Returns a normalized value between -1.0 and 1.0 for a joystick axis
	virtual float joyAxisNormValue(int joyId, int axisId) const = 0;

	/// Returns current mouse cursor mode
	inline MouseCursorMode mouseCursorMode() const { return mouseCursorMode_; }
	/// Sets the mouse cursor mode
	virtual void setMouseCursorMode(MouseCursorMode mode) = 0;

  protected:
	static IInputEventHandler *inputEventHandler_;
	MouseCursorMode mouseCursorMode_;

	IInputManager() : mouseCursorMode_(MOUSE_CURSOR_NORMAL) { }
};

}

#endif
