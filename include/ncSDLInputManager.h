#ifndef CLASS_NCSDLINPUTMANAGER
#define CLASS_NCSDLINPUTMANAGER

#include <SDL/SDL_events.h>
#include "ncIInputManager.h"

/// Information about SDL mouse state
class ncSDLMouseState : public ncMouseState
{
 public:
	ncSDLMouseState() : m_ubButtons(0) { }

	inline bool isLeftButtonDown() const { return (m_ubButtons & LEFT_BUTTON); }
	inline bool isMiddleButtonDown() const { return (m_ubButtons & MIDDLE_BUTTON); }
	inline bool isRightButtonDown() const { return (m_ubButtons & RIGHT_BUTTON); }
	inline bool isWheelUpButtonDown() const { return (m_ubButtons & WHEELUP_BUTTON); }
	inline bool isWheelDownButtonDown() const { return (m_ubButtons & WHEELDOWN_BUTTON); }

 private:
	// Extracted from SDL/SDL_mouse.h
	enum eMouseButtonMask
	{
		LEFT_BUTTON   = 1,
		MIDDLE_BUTTON = 2,
		RIGHT_BUTTON  = 4,
		WHEELUP_BUTTON = 8,
		WHEELDOWN_BUTTON = 16
	};

	unsigned char m_ubButtons;

	friend class ncSDLInputManager;
};

/// Information about an SDL mouse event
class ncSDLMouseEvent : public ncMouseEvent
{
 public:
	ncSDLMouseEvent() : m_ubButton(0) { }

	inline bool isLeftButton() const { return m_ubButton == LEFT_BUTTON; }
	inline bool isMiddleButton() const { return m_ubButton == MIDDLE_BUTTON; }
	inline bool isRightButton() const { return m_ubButton == RIGHT_BUTTON; }
	inline bool isWheelUpButton() const { return m_ubButton == WHEELUP_BUTTON; }
	inline bool isWheelDownButton() const { return m_ubButton == WHEELDOWN_BUTTON; }

 private:
	// Extracted from SDL/SDL_mouse.h
	enum eMouseButtonIndex
	{
		LEFT_BUTTON = 1,
		MIDDLE_BUTTON = 2,
		RIGHT_BUTTON = 3,
		WHEELUP_BUTTON = 4,
		WHEELDOWN_BUTTON = 5
	};

	unsigned char m_ubButton;

	friend class ncSDLInputManager;
};

/// Information about SDL keyboard state
class ncSDLKeyboardState : public ncKeyboardState
{
 public:
	ncSDLKeyboardState() { m_ubKeyState = SDL_GetKeyState(NULL); }

	inline bool isKeyDown(ncKeySym key) const { return m_ubKeyState[key]; }

	friend class ncSDLInputManager;

 private:
	unsigned char* m_ubKeyState;
};

/// The class for parsing and dispatching SDL input events
class ncSDLInputManager : public ncIInputManager
{
 public:
	// The constructor takes care of opening available joysticks
	ncSDLInputManager();
	// The destructor releases every opened joystick
	~ncSDLInputManager();

	static void ParseEvent(const SDL_Event &event);

	inline const ncMouseState& MouseState()
	{
		s_mouseState.m_ubButtons = SDL_GetMouseState(&s_mouseState.x, &s_mouseState.y);
		return s_mouseState;
	}

	inline const ncKeyboardState& KeyboardState() const	{ return s_keyboardState; }

	bool isJoyPresent(int iJoyId) const;
	const char* JoyName(int iJoyId) const;
	int JoyNumButtons(int iJoyId) const;
	int JoyNumAxes(int iJoyId) const;
	bool isJoyButtonPressed(int iJoyId, int iButtonId) const;
	short int JoyAxisValue(int iJoyId, int iAxisId) const;
	float JoyAxisNormValue(int iJoyId, int iAxisId) const;

 private:
	static const unsigned int s_uMaxNumJoysticks = 16;

	static short int HatEnumToAxisValue(unsigned char ubHatState, bool bUpDownAxis);

	static ncSDLMouseState s_mouseState;
	static ncSDLMouseEvent s_mouseEvent;
	static ncSDLKeyboardState s_keyboardState;
	static ncKeyboardEvent	s_keyboardEvent;

	static SDL_Joystick* s_pJoysticks[s_uMaxNumJoysticks];
	static ncJoyButtonEvent s_joyButtonEvent;
	static ncJoyAxisEvent s_joyAxisEvent;
};

#endif
