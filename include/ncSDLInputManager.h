#ifndef CLASS_NCSDLINPUTMANAGER
#define CLASS_NCSDLINPUTMANAGER

#include <SDL/SDL_events.h>
#include "ncIInputManager.h"

/// Information about SDL mouse state
class ncSDLMouseState : public ncMouseState
{
  public:
	ncSDLMouseState() : buttons_(0) { }

	inline bool isLeftButtonDown() const { return (buttons_ & LEFT_BUTTON); }
	inline bool isMiddleButtonDown() const { return (buttons_ & MIDDLE_BUTTON); }
	inline bool isRightButtonDown() const { return (buttons_ & RIGHT_BUTTON); }
	inline bool isWheelUpButtonDown() const { return (buttons_ & WHEELUP_BUTTON); }
	inline bool isWheelDownButtonDown() const { return (buttons_ & WHEELDOWN_BUTTON); }

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

	unsigned char buttons_;

	friend class ncSDLInputManager;
};

/// Information about an SDL mouse event
class ncSDLMouseEvent : public ncMouseEvent
{
  public:
	ncSDLMouseEvent() : button_(0) { }

	inline bool isLeftButton() const { return button_ == LEFT_BUTTON; }
	inline bool isMiddleButton() const { return button_ == MIDDLE_BUTTON; }
	inline bool isRightButton() const { return button_ == RIGHT_BUTTON; }
	inline bool isWheelUpButton() const { return button_ == WHEELUP_BUTTON; }
	inline bool isWheelDownButton() const { return button_ == WHEELDOWN_BUTTON; }

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

	unsigned char button_;

	friend class ncSDLInputManager;
};

/// Information about SDL keyboard state
class ncSDLKeyboardState : public ncKeyboardState
{
  public:
	ncSDLKeyboardState() { keyState_ = SDL_GetKeyState(NULL); }

	inline bool isKeyDown(ncKeySym key) const { return keyState_[key]; }

	friend class ncSDLInputManager;

  private:
	unsigned char* keyState_;
};

/// The class for parsing and dispatching SDL input events
class ncSDLInputManager : public ncIInputManager
{
  public:
	// The constructor takes care of opening available joysticks
	ncSDLInputManager();
	// The destructor releases every opened joystick
	~ncSDLInputManager();

	static void parseEvent(const SDL_Event &event);

	inline const ncMouseState& mouseState()
	{
		mouseState_.buttons_ = SDL_GetMouseState(&mouseState_.x, &mouseState_.y);
		return mouseState_;
	}

	inline const ncKeyboardState& keyboardState() const	{ return keyboardState_; }

	bool isJoyPresent(int joyId) const;
	const char* joyName(int joyId) const;
	int joyNumButtons(int joyId) const;
	int joyNumAxes(int joyId) const;
	bool isJoyButtonPressed(int joyId, int buttonId) const;
	short int joyAxisValue(int joyId, int axisId) const;
	float joyAxisNormValue(int joyId, int axisId) const;

  private:
	static const unsigned int MaxNumJoysticks = 16;

	static ncSDLMouseState mouseState_;
	static ncSDLMouseEvent mouseEvent_;
	static ncSDLKeyboardState keyboardState_;
	static ncKeyboardEvent	keyboardEvent_;

	static SDL_Joystick* sdlJoysticks_[MaxNumJoysticks];
	static ncJoyButtonEvent joyButtonEvent_;
	static ncJoyAxisEvent joyAxisEvent_;

	static short int hatEnumToAxisValue(unsigned char hatState, bool upDownAxis);
};

#endif
