#ifndef CLASS_NCINE_SDLINPUTMANAGER
#define CLASS_NCINE_SDLINPUTMANAGER

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include "IInputManager.h"
#include <nctl/StaticArray.h>

namespace ncine {

/// Utility functions to convert between engine key enumerations and SDL ones
class SdlKeys
{
  public:
	static KeySym keySymValueToEnum(int keysym);
	static int keyModMaskToEnumMask(int keymod);
	static int enumToKeySymValue(KeySym keysym);
	static int enumToScancode(KeySym keysym);
};

/// Information about SDL mouse state
class SdlMouseState : public MouseState
{
  public:
	SdlMouseState()
	    : buttons_(0) {}

	inline bool isLeftButtonDown() const override { return (buttons_ & SDL_BUTTON_LMASK) != 0; }
	inline bool isMiddleButtonDown() const override { return (buttons_ & SDL_BUTTON_MMASK) != 0; }
	inline bool isRightButtonDown() const override { return (buttons_ & SDL_BUTTON_RMASK) != 0; }
	inline bool isFourthButtonDown() const override { return (buttons_ & SDL_BUTTON_X1MASK) != 0; }
	inline bool isFifthButtonDown() const override { return (buttons_ & SDL_BUTTON_X2MASK) != 0; }

  private:
	unsigned int buttons_;

	friend class SdlInputManager;
};

/// Information about an SDL mouse event
class SdlMouseEvent : public MouseEvent
{
  public:
	SdlMouseEvent()
	    : button_(0) {}

	inline bool isLeftButton() const override { return button_ == SDL_BUTTON_LEFT; }
	inline bool isMiddleButton() const override { return button_ == SDL_BUTTON_MIDDLE; }
	inline bool isRightButton() const override { return button_ == SDL_BUTTON_RIGHT; }
	inline bool isFourthButton() const override { return button_ == SDL_BUTTON_X1; }
	inline bool isFifthButton() const override { return button_ == SDL_BUTTON_X2; }

  private:
	unsigned char button_;

	friend class SdlInputManager;
};

/// Information about an SDL scroll event
class SdlScrollEvent : public ScrollEvent
{
  public:
	SdlScrollEvent() {}

	friend class SdlInputManager;
};

/// Information about SDL keyboard state
class SdlKeyboardState : public KeyboardState
{
  public:
	SdlKeyboardState() { keyState_ = SDL_GetKeyboardState(nullptr); }

	inline bool isKeyDown(KeySym key) const override
	{
		const int sdlKey = SdlKeys::enumToScancode(key);
		if (sdlKey == SDL_SCANCODE_UNKNOWN)
			return false;
		else
			return keyState_[SdlKeys::enumToScancode(key)] != 0;
	}

	friend class SdlInputManager;

  private:
	const unsigned char *keyState_;
};

/// Information about SDL joystick state
class SdlJoystickState : public JoystickState
{
  public:
	SdlJoystickState()
	    : sdlJoystick_(nullptr) {}

	bool isButtonPressed(int buttonId) const override;
	unsigned char hatState(int hatId) const override;
	short int axisValue(int axisId) const override;
	float axisNormValue(int axisId) const override;

  private:
	SDL_Joystick *sdlJoystick_;

	friend class SdlInputManager;
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

	inline const MouseState &mouseState() const override
	{
		mouseState_.buttons_ = SDL_GetMouseState(&mouseState_.x, &mouseState_.y);
		return mouseState_;
	}

	inline const KeyboardState &keyboardState() const override { return keyboardState_; }

	bool isJoyPresent(int joyId) const override;
	const char *joyName(int joyId) const override;
	const char *joyGuid(int joyId) const override;
	int joyNumButtons(int joyId) const override;
	int joyNumHats(int joyId) const override;
	int joyNumAxes(int joyId) const override;
	const JoystickState &joystickState(int joyId) const override;

	void setMouseCursorMode(MouseCursorMode mode) override;

  private:
	static const int MaxNumJoysticks = 16;

	static SDL_Window *windowHandle_;

	static TouchEvent touchEvent_;
	static SdlMouseState mouseState_;
	static SdlMouseEvent mouseEvent_;
	static SdlScrollEvent scrollEvent_;
	static SdlKeyboardState keyboardState_;
	static KeyboardEvent keyboardEvent_;

	static SDL_Joystick *sdlJoysticks_[MaxNumJoysticks];
	static nctl::StaticArray<SdlJoystickState, MaxNumJoysticks> joystickStates_;
	static JoyButtonEvent joyButtonEvent_;
	static JoyHatEvent joyHatEvent_;
	static JoyAxisEvent joyAxisEvent_;
	static JoyConnectionEvent joyConnectionEvent_;

	static char joyGuidString_[33];

	/// Deleted copy constructor
	SdlInputManager(const SdlInputManager &) = delete;
	/// Deleted assignment operator
	SdlInputManager &operator=(const SdlInputManager &) = delete;

	static void handleJoyDeviceEvent(const SDL_Event &event);
	static int joyInstanceIdToDeviceIndex(SDL_JoystickID instanceId);
};

}

#endif
