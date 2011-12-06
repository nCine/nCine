#ifndef CLASS_NCSDLINPUTMANAGER
#define CLASS_NCSDLINPUTMANAGER

#include "ncIInputManager.h"

/// Information about an SDL mouse event
class ncSDLMouseEvent : public ncMouseEvent
{
private:
	enum eMouseButtonIndex {
		LEFT_BUTTON = 1,
		MIDDLE_BUTTON = 2,
		RIGHT_BUTTON = 3,
		WHEELUP_BUTTON = 4,
		WHEELDOWN_BUTTON = 5
	};

	unsigned char button;
public:
	inline bool isLeftButton() const { return button == LEFT_BUTTON; }
	inline bool isMiddleButton() const { return button == MIDDLE_BUTTON; }
	inline bool isRightButton() const { return button == RIGHT_BUTTON; }
	inline bool isWheelUpButton() const { return button == WHEELUP_BUTTON; }
	inline bool isWheelDownButton() const { return button == WHEELDOWN_BUTTON; }

	friend class ncSDLInputManager;
};

/// Information about SDL mouse state
class ncSDLMouseState : public ncMouseState
{
private:
	unsigned char buttons;

public:
	inline bool isButtonDown(unsigned char uButtonMask) const
	{
		return (buttons & uButtonMask);
	}

	friend class ncSDLInputManager;
};

/// The class for parsing and dispatching SDL input events
class ncSDLInputManager : public ncIInputManager
{
private:
	ncSDLMouseEvent m_mouseEvent;
	ncSDLMouseState m_mouseState;
	ncKeyboardEvent	m_keyboardEvent;

public:
	ncSDLInputManager();

	void ParseEvent(const SDL_Event &event);

	inline unsigned char* KeyboardState() const
	{
		return SDL_GetKeyState(NULL);
	}

	inline const ncMouseState& MouseState()
	{
		m_mouseState.buttons = SDL_GetMouseState(&m_mouseState.x, &m_mouseState.y);
		return m_mouseState;
	}
};

#endif
