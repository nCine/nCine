#ifndef CLASS_NCIINPUTMANAGER
#define CLASS_NCIINPUTMANAGER

#if defined(__ANDROID__)
	#include "ncAndroidKeys.h"
#elif defined(WITH_SDL)
	#include "ncSDLKeys.h"
#elif defined(WITH_GLFW)
	#include "ncGLFWKeys.h"
#endif

class ncIInputEventHandler;

#ifdef __ANDROID__
class ncTouchEvent
{
public:
	ncTouchEvent() : count(0), id(0), id2(1), x(0.0f), y(0.0f), x2(0.0f), y2(0.0f) { }

	int count;
	int id, id2;
	float x, y, x2, y2;
};

class ncAccelerometerEvent
{
public:
	ncAccelerometerEvent(): x(0.0f), y(0.0f), z(0.0f) { }

	float x, y, z;
};
#else
/// Information about mouse state
class ncMouseState
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
class ncMouseEvent
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
class ncKeyboardState
{
public:
	ncKeyboardState() { }

	virtual bool isKeyDown(ncKeySym key) const = 0;
};
#endif

/// Information about a keyboard event
class ncKeyboardEvent
{
public:
	ncKeyboardEvent() : scancode(0), sym(NCKEY_UNKNOWN), mod(0), unicode(0) { }

	/// Key scan code
	int scancode;
	/// Key symbol code
	ncKeySym sym;
	/// Key modifiers mask
	int mod;
	/// Unicode character
	unsigned short int unicode;
};

/// Information about a joystick axis event
class ncJoyAxisEvent
{
public:
	int joyId;
	int axisId;
	short int value;
};

/// Information about a joystick button event
class ncJoyButtonEvent
{
public:
	int joyId;
	int buttonId;
};

/// The interface class for parsing and dispatching input events
class ncIInputManager
{
protected:
	static ncIInputEventHandler *s_pInputEventHandler;
	ncIInputManager() { }

public:
	virtual ~ncIInputManager() { }

	static inline void SetHandler(ncIInputEventHandler *pInputEventHandler)
	{
		s_pInputEventHandler = pInputEventHandler;
	}

#ifndef __ANDROID__
	virtual const ncMouseState& MouseState() = 0;
	virtual const ncKeyboardState& KeyboardState() const = 0;
#endif

	static short int s_iMaxAxisValue;
	/// Returns true if the specified joystick is connected
	virtual bool isJoyPresent(int iJoyId) const = 0;
	/// Returns the name of the specified joystick
	virtual const char* JoyName(int iJoyId) const = 0;
	/// Returns the number of available buttons for the specified joystick
	virtual int JoyNumButtons(int iJoyId) const = 0;
	/// Returns the number of available axes for the specified joystick
	virtual int JoyNumAxes(int iJoyId) const = 0;
	/// Returns the state of a joystick button
	virtual bool isJoyButtonPressed(int iJoyId, int iButtonId) const = 0;
	/// Returns a value between -32768 and 32767 for a joystick axis
	virtual short int JoyAxisValue(int iJoyId, int iAxisId) const = 0;
	/// Returns a normalized value between -1 and 1 for a joystick axis
	virtual float JoyAxisNormValue(int iJoyId, int iAxisId) const = 0;
};

#endif
