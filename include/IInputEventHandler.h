#ifndef CLASS_NCINE_IINPUTEVENTHANDLER
#define CLASS_NCINE_IINPUTEVENTHANDLER

#include "IInputManager.h"

namespace ncine {

/// The interface class for handling input events (i.e. mouse, keyboard, touch, sensors)
class DLL_PUBLIC IInputEventHandler
{
  public:
	IInputEventHandler() { IInputManager::setHandler(this); }
	inline virtual ~IInputEventHandler() { }

	inline virtual void onKeyPressed(const KeyboardEvent &event) { }
	inline virtual void onKeyReleased(const KeyboardEvent &event) { }
#ifdef __ANDROID__
	inline virtual void onTouchDown(const TouchEvent &event) { }
	inline virtual void onTouchUp(const TouchEvent &event) { }
	inline virtual void onTouchMove(const TouchEvent &event) { }
	inline virtual void onSecondaryTouchDown(const TouchEvent &event) { }
	inline virtual void onSecondaryTouchUp(const TouchEvent &event) { }
	inline virtual void onAcceleration(const AccelerometerEvent &event) { }
	inline virtual void onJoyButtonPressed(const JoyButtonEvent &event) { }
	inline virtual void onJoyButtonReleased(const JoyButtonEvent &event) { }
	inline virtual void onJoyAxisMoved(const JoyAxisEvent &event) { }
#else
	inline virtual void onMouseButtonPressed(const MouseEvent &event) { }
	inline virtual void onMouseButtonReleased(const MouseEvent &event) { }
	inline virtual void onMouseMoved(const MouseState &state) { }
	inline virtual void onJoyButtonPressed(const JoyButtonEvent &event) { }
	inline virtual void onJoyButtonReleased(const JoyButtonEvent &event) { }
	inline virtual void onJoyAxisMoved(const JoyAxisEvent &event) { }
#endif
};

}

#endif
