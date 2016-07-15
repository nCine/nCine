#ifndef CLASS_NCINE_SDLINPUTMANAGER
#define CLASS_NCINE_SDLINPUTMANAGER

#include <SDL/SDL_events.h>
#include "IInputManager.h"

namespace ncine {

/// Utility functions to convert between engine key enumerations and SDL ones
class SdlKeys
{
  public:
	static KeySym keySymValueToEnum(int keysym);
	static KeyMod keyModValueToEnum(int keymod);
	static int enumToKeySymValue(KeySym keysym);
	static int enumToKeyModValue(KeyMod keymod);
};

/// Information about SDL mouse state
class SdlMouseState : public MouseState
{
  public:
	SdlMouseState() : buttons_(0) { }

	inline bool isLeftButtonDown() const { return (buttons_ & LEFT_BUTTON) != 0; }
	inline bool isMiddleButtonDown() const { return (buttons_ & MIDDLE_BUTTON) != 0; }
	inline bool isRightButtonDown() const { return (buttons_ & RIGHT_BUTTON) != 0; }
	inline bool isWheelUpButtonDown() const { return (buttons_ & WHEELUP_BUTTON) != 0; }
	inline bool isWheelDownButtonDown() const { return (buttons_ & WHEELDOWN_BUTTON) != 0; }

  private:
	/// Enumeration extracted from `SDL/SDL_mouse.h`
	enum eMouseButtonMask
	{
		LEFT_BUTTON   = 1,
		MIDDLE_BUTTON = 2,
		RIGHT_BUTTON  = 4,
		WHEELUP_BUTTON = 8,
		WHEELDOWN_BUTTON = 16
	};

	unsigned char buttons_;

	friend class SdlInputManager;
};

/// Information about an SDL mouse event
class SdlMouseEvent : public MouseEvent
{
  public:
	SdlMouseEvent() : button_(0) { }

	inline bool isLeftButton() const { return button_ == LEFT_BUTTON; }
	inline bool isMiddleButton() const { return button_ == MIDDLE_BUTTON; }
	inline bool isRightButton() const { return button_ == RIGHT_BUTTON; }
	inline bool isWheelUpButton() const { return button_ == WHEELUP_BUTTON; }
	inline bool isWheelDownButton() const { return button_ == WHEELDOWN_BUTTON; }

  private:
	/// Enumeration extracted from `SDL/SDL_mouse.h`
	enum eMouseButtonIndex
	{
		LEFT_BUTTON = 1,
		MIDDLE_BUTTON = 2,
		RIGHT_BUTTON = 3,
		WHEELUP_BUTTON = 4,
		WHEELDOWN_BUTTON = 5
	};

	unsigned char button_;

	friend class SdlInputManager;
};

/// Information about SDL keyboard state
class SdlKeyboardState : public KeyboardState
{
  public:
	SdlKeyboardState() { keyState_ = SDL_GetKeyState(NULL); }

	inline bool isKeyDown(KeySym key) const { return keyState_[SdlKeys::enumToKeySymValue(key)] != 0; }

	friend class SdlInputManager;

  private:
	unsigned char *keyState_;
};

/// The class for parsing and dispatching SDL input events
class SdlInputManager : public IInputManager
{
  public:
	/// The constructor takes care of opening available joysticks
	SdlInputManager();
	/// The destructor releases every opened joystick
	~SdlInputManager();

	static void parseEvent(const SDL_Event &event);

	inline const MouseState &mouseState()
	{
		mouseState_.buttons_ = SDL_GetMouseState(&mouseState_.x, &mouseState_.y);
		return mouseState_;
	}

	inline const KeyboardState &keyboardState() const	{ return keyboardState_; }

	bool isJoyPresent(int joyId) const;
	const char *joyName(int joyId) const;
	int joyNumButtons(int joyId) const;
	int joyNumAxes(int joyId) const;
	bool isJoyButtonPressed(int joyId, int buttonId) const;
	short int joyAxisValue(int joyId, int axisId) const;
	float joyAxisNormValue(int joyId, int axisId) const;

  private:
	static const unsigned int MaxNumJoysticks = 16;

	static SdlMouseState mouseState_;
	static SdlMouseEvent mouseEvent_;
	static SdlKeyboardState keyboardState_;
	static KeyboardEvent	keyboardEvent_;

	static SDL_Joystick *sdlJoysticks_[MaxNumJoysticks];
	static JoyButtonEvent joyButtonEvent_;
	static JoyAxisEvent joyAxisEvent_;

	/// Private copy constructor
	SdlInputManager(const SdlInputManager &);
	/// Private assignment operator
	SdlInputManager &operator=(const SdlInputManager &);

	static short int hatEnumToAxisValue(unsigned char hatState, bool upDownAxis);
};

}

#endif
