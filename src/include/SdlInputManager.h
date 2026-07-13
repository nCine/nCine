#ifndef CLASS_NCINE_SDLINPUTMANAGER
#define CLASS_NCINE_SDLINPUTMANAGER

#include "IInputManager.h"
#include <nctl/StaticArray.h>

#ifdef WITH_SDL2
typedef struct _SDL_Joystick SDL_Joystick;
#else
typedef struct SDL_Joystick SDL_Joystick;
typedef uint32_t SDL_JoystickID;
#endif
union SDL_Event;

namespace ncine {

/// Utility functions to convert between engine key enumerations and SDL ones
class SdlKeys
{
  public:
	static KeySym keySymValueToEnum(int keysym);
#ifdef WITH_SDL2
	static int keyModMaskToEnumMask(int keymod);
#else
	static int keyModMaskToEnumMask(uint16_t keymod);
#endif
	static int enumToKeySymValue(KeySym keysym);
	static int enumToScancode(KeySym keysym);
};

/// Information about SDL mouse state
class SdlMouseState : public MouseState
{
  public:
	SdlMouseState();

	bool isButtonDown(MouseButton button) const override;
	bool isButtonPressed(MouseButton button) const override;
	bool isButtonReleased(MouseButton button) const override;

  private:
	unsigned int currentStateIndex_;
	unsigned int buttons_[2];

	void copyButtonStateToPrev();

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
	SdlKeyboardState();

	bool isKeyDown(KeySym key) const override;
	bool isKeyPressed(KeySym key) const override;
	bool isKeyReleased(KeySym key) const override;

  private:
	static const unsigned int MaxKeyStateArrayLength = 512;

	int keyStateArrayLength_;
#ifdef WITH_SDL2
	const unsigned char *keyState_;
	unsigned char prevKeyState_[MaxKeyStateArrayLength];
#else
	const bool *keyState_;
	bool prevKeyState_[MaxKeyStateArrayLength];
#endif

	void copyKeyStateToPrev();

	friend class SdlInputManager;
};

/// Information about SDL joystick state
class SdlJoystickState : public JoystickState
{
  public:
	SdlJoystickState();

	bool isButtonDown(int buttonId) const override;
	bool isButtonPressed(int buttonId) const override;
	bool isButtonReleased(int buttonId) const override;

	unsigned char hatState(int hatId) const override;
	short int axisValue(int axisId) const override;
	float axisNormValue(int axisId) const override;

  private:
	static const unsigned int MaxNumButtons = 32;

	SDL_Joystick *sdlJoystick_;
#ifdef WITH_SDL2
	unsigned char prevButtonState_[MaxNumButtons];
#else
	bool prevButtonState_[MaxNumButtons];
#endif

	void copyButtonStateToPrev();
	void resetPrevButtonState();

	friend class SdlInputManager;
};

/// The class for parsing and dispatching SDL input events
class SdlInputManager : public IInputManager
{
  public:
	/// The constructor takes care of opening available joysticks
	SdlInputManager();
	/// The destructor releases every opened joystick
	~SdlInputManager() override;

	static bool shouldQuitOnRequest();
	static void copyButtonStatesToPrev();
	static void parseEvent(const SDL_Event &event);

	const MouseState &mouseState() const override;
	inline const KeyboardState &keyboardState() const override { return keyboardState_; }

	bool isJoyPresent(int joyId) const override;
	const char *joyName(int joyId) const override;
	const char *joyGuid(int joyId) const override;
	int joyNumButtons(int joyId) const override;
	int joyNumHats(int joyId) const override;
	int joyNumAxes(int joyId) const override;
	const JoystickState &joystickState(int joyId) const override;

	bool hasJoyVibration(int joyId) const override;
	void joyVibrate(int joyId, float lowFreqIntensity, float highFreqIntensity, unsigned int duration) const override;

	void setMouseCursorMode(MouseCursorMode mode) override;

  private:
	static const int MaxNumJoysticks = 16;

	static TouchEvent touchEvent_;
	static SdlMouseState mouseState_;
	static MouseEvent mouseEvent_;
	static SdlScrollEvent scrollEvent_;
	static SdlKeyboardState keyboardState_;
	static KeyboardEvent keyboardEvent_;
	static TextInputEvent textInputEvent_;

	static SDL_Joystick *sdlJoysticks_[MaxNumJoysticks];
	static nctl::StaticArray<SdlJoystickState, MaxNumJoysticks> joystickStates_;
	static JoyButtonEvent joyButtonEvent_;
	static JoyHatEvent joyHatEvent_;
	static JoyAxisEvent joyAxisEvent_;
	static JoyConnectionEvent joyConnectionEvent_;

#ifdef WITH_SDL3
	static int joystickIDCount_;
	static SDL_JoystickID *joystickIDs_;
#endif

	static char joyGuidString_[33];

	/// Deleted copy constructor
	SdlInputManager(const SdlInputManager &) = delete;
	/// Deleted assignment operator
	SdlInputManager &operator=(const SdlInputManager &) = delete;

	static void handleJoyDeviceEvent(const SDL_Event &event);
#ifdef WITH_SDL2
	static int joyInstanceIdToDeviceIndex(int32_t instanceId);
#endif
};

}

#endif
