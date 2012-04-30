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
	ncTouchEvent() : count(0), id(0), x(0), y(0), id2(1), x2(0), y2(0) { }

	int count;
	int id, x, y;
	int id2, x2, y2;
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
	ncKeyboardEvent() : sym(NCKEY_UNKNOWN), mod(0), unicode(0) { }

	/// Key symbol code
	ncKeySym sym;
	/// Key modifiers mask
	int mod;
	/// Unicode character
	unsigned short int unicode;
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
};

#endif
