#ifndef CLASS_NCIINPUTMANAGER
#define CLASS_NCIINPUTMANAGER

#ifdef __ANDROID__
	#include <android/input.h>
	#include "ncAndroidKeys.h"
#else
	#include <SDL/SDL_events.h>
	#include "ncSDLKeys.h"
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
#else
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

/// Information about mouse state
class ncMouseState
{
public:
	// Mouse button bitmasks
	static const unsigned char LEFT_BUTTON   = 1;
	static const unsigned char MIDDLE_BUTTON = 2;
	static const unsigned char RIGHT_BUTTON  = 4;
	static const unsigned char WHEELUP_BUTTON = 8;
	static const unsigned char WHEELDOWN_BUTTON = 16;

	/// Pointer position on the X axis
	int x;
	/// Pointer position on the Y axis
	int y;

	virtual bool isButtonDown(unsigned char uButtonMask) const = 0;
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
	ncIInputEventHandler *m_pInputEventHandler;

	ncIInputManager() : m_pInputEventHandler(NULL) { }

public:
	virtual ~ncIInputManager() { }

	inline void SetHandler(ncIInputEventHandler *pInputEventHandler)
	{
		m_pInputEventHandler = pInputEventHandler;
	}

#ifdef __ANDROID__
	virtual void ParseEvent(const AInputEvent* event) = 0;
#else
	virtual void ParseEvent(const SDL_Event &event) = 0;
	virtual unsigned char* KeyboardState() const = 0;
	virtual const ncMouseState& MouseState() = 0;
#endif
};

#endif
