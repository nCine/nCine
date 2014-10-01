#ifndef CLASS_NCIINPUTEVENTHANDLER
#define CLASS_NCIINPUTEVENTHANDLER

#include "ncIInputManager.h"

/// The interface class for handling input events (i.e. mouse, keyboard, touch, sensors)
class ncIInputEventHandler
{
 public:
	/// Pure virtual destructor in order to make the class abstract
	virtual ~ncIInputEventHandler() = 0;

	inline virtual void OnKeyPressed(const ncKeyboardEvent &event) { }
	inline virtual void OnKeyReleased(const ncKeyboardEvent &event) { }
#ifdef __ANDROID__
	inline virtual void OnTouchDown(const ncTouchEvent &event) { }
	inline virtual void OnTouchUp(const ncTouchEvent &event) { }
	inline virtual void OnTouchMove(const ncTouchEvent &event) { }
	inline virtual void OnSecondaryTouchDown(const ncTouchEvent &event) { }
	inline virtual void OnSecondaryTouchUp(const ncTouchEvent &event) { }
	inline virtual void OnAcceleration(const ncAccelerometerEvent &event) { }
	#if (__ANDROID_API__ >= 13)
	inline virtual void OnJoyButtonPressed(const ncJoyButtonEvent &event) { }
	inline virtual void OnJoyButtonReleased(const ncJoyButtonEvent &event) { }
	inline virtual void OnJoyAxisMoved(const ncJoyAxisEvent &event) { }
	#endif
#else
	inline virtual void OnMouseButtonPressed(const ncMouseEvent &event) { }
	inline virtual void OnMouseButtonReleased(const ncMouseEvent &event) { }
	inline virtual void OnMouseMoved(const ncMouseState &state) { }
	inline virtual void OnJoyButtonPressed(const ncJoyButtonEvent &event) { }
	inline virtual void OnJoyButtonReleased(const ncJoyButtonEvent &event) { }
	inline virtual void OnJoyAxisMoved(const ncJoyAxisEvent &event) { }
#endif
};

inline ncIInputEventHandler::~ncIInputEventHandler() { }

#endif
