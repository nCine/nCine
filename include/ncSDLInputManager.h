#ifndef CLASS_NCSDLINPUTMANAGER
#define CLASS_NCSDLINPUTMANAGER

#include <SDL/SDL_events.h>
#include "ncIInputManager.h"

/// Information about SDL mouse state
class ncSDLMouseState : public ncMouseState
{
private:
	// Extracted from SDL/SDL_mouse.h
	enum eMouseButtonMask {
		LEFT_BUTTON   = 1,
		MIDDLE_BUTTON = 2,
		RIGHT_BUTTON  = 4,
		WHEELUP_BUTTON = 8,
		WHEELDOWN_BUTTON = 16
	};

	unsigned char buttons;
public:
	ncSDLMouseState() : buttons(0) { }

	inline bool isLeftButtonDown() const { return (buttons & LEFT_BUTTON); }
	inline bool isMiddleButtonDown() const { return (buttons & MIDDLE_BUTTON); }
	inline bool isRightButtonDown() const { return (buttons & RIGHT_BUTTON); }
	inline bool isWheelUpButtonDown() const { return (buttons & WHEELUP_BUTTON); }
	inline bool isWheelDownButtonDown() const { return (buttons & WHEELDOWN_BUTTON); }

	friend class ncSDLInputManager;
};

/// Information about an SDL mouse event
class ncSDLMouseEvent : public ncMouseEvent
{
private:
	// Extracted from SDL/SDL_mouse.h
	enum eMouseButtonIndex {
		LEFT_BUTTON = 1,
		MIDDLE_BUTTON = 2,
		RIGHT_BUTTON = 3,
		WHEELUP_BUTTON = 4,
		WHEELDOWN_BUTTON = 5
	};

	unsigned char button;
public:
	ncSDLMouseEvent() : button(0) { }

	inline bool isLeftButton() const { return button == LEFT_BUTTON; }
	inline bool isMiddleButton() const { return button == MIDDLE_BUTTON; }
	inline bool isRightButton() const { return button == RIGHT_BUTTON; }
	inline bool isWheelUpButton() const { return button == WHEELUP_BUTTON; }
	inline bool isWheelDownButton() const { return button == WHEELDOWN_BUTTON; }

	friend class ncSDLInputManager;
};

/// Information about SDL keyboard state
class ncSDLKeyboardState : public ncKeyboardState
{
private:
	unsigned char* m_ubKeyState;
public:
	ncSDLKeyboardState() { m_ubKeyState = SDL_GetKeyState(NULL); }

	inline bool isKeyDown(ncKeySym key) const { return m_ubKeyState[key]; }

	friend class ncSDLInputManager;
};

/// The class for parsing and dispatching SDL input events
class ncSDLInputManager : public ncIInputManager
{
private:
	static ncSDLMouseState s_mouseState;
	static ncSDLMouseEvent s_mouseEvent;
	static ncSDLKeyboardState s_keyboardState;
	static ncKeyboardEvent	s_keyboardEvent;

public:
	static void ParseEvent(const SDL_Event &event);

	inline const ncMouseState& MouseState()
	{
		s_mouseState.buttons = SDL_GetMouseState(&s_mouseState.x, &s_mouseState.y);
		return s_mouseState;
	}

	inline const ncKeyboardState& KeyboardState() const
	{
		return s_keyboardState;
	}
};

#endif
