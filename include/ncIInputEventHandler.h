#ifndef CLASS_NCIINPUTEVENTHANDLER
#define CLASS_NCIINPUTEVENTHANDLER

#include "ncIInputManager.h"

/// The interface class for handling input events (i.e. mouse, keyboard, touch, sensors)
class ncIInputEventHandler
{
  public:
	/// Pure virtual destructor in order to make the class abstract
	virtual ~ncIInputEventHandler() = 0;

	inline virtual void onKeyPressed(const ncKeyboardEvent &event) { }
	inline virtual void onKeyReleased(const ncKeyboardEvent &event) { }
#ifdef __ANDROID__
	inline virtual void onTouchDown(const ncTouchEvent &event) { }
	inline virtual void onTouchUp(const ncTouchEvent &event) { }
	inline virtual void onTouchMove(const ncTouchEvent &event) { }
	inline virtual void onSecondaryTouchDown(const ncTouchEvent &event) { }
	inline virtual void onSecondaryTouchUp(const ncTouchEvent &event) { }
	inline virtual void onAcceleration(const ncAccelerometerEvent &event) { }
	inline virtual void onJoyButtonPressed(const ncJoyButtonEvent &event) { }
	inline virtual void onJoyButtonReleased(const ncJoyButtonEvent &event) { }
	inline virtual void onJoyAxisMoved(const ncJoyAxisEvent &event) { }
#else
	inline virtual void onMouseButtonPressed(const ncMouseEvent &event) { }
	inline virtual void onMouseButtonReleased(const ncMouseEvent &event) { }
	inline virtual void onMouseMoved(const ncMouseState &state) { }
	inline virtual void onJoyButtonPressed(const ncJoyButtonEvent &event) { }
	inline virtual void onJoyButtonReleased(const ncJoyButtonEvent &event) { }
	inline virtual void onJoyAxisMoved(const ncJoyAxisEvent &event) { }
#endif
};

inline ncIInputEventHandler::~ncIInputEventHandler() { }

#endif
