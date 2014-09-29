#ifndef CLASS_NCIINPUTEVENTHANDLER
#define CLASS_NCIINPUTEVENTHANDLER

#include "ncIInputManager.h"

/// The interface class for handling input events (i.e. mouse, keyboard, touch, sensors)
class ncIInputEventHandler
{
public:
	/// Pure virtual destructor in order to make the class abstract
	virtual ~ncIInputEventHandler() = 0;

	virtual inline void OnKeyPressed(const ncKeyboardEvent &event) { }
	virtual inline void OnKeyReleased(const ncKeyboardEvent &event) { }
#ifdef __ANDROID__
	virtual inline void OnTouchDown(const ncTouchEvent &event) { }
	virtual inline void OnTouchUp(const ncTouchEvent &event) { }
	virtual inline void OnTouchMove(const ncTouchEvent &event) { }
	virtual inline void OnSecondaryTouchDown(const ncTouchEvent &event) { }
	virtual inline void OnSecondaryTouchUp(const ncTouchEvent &event) { }
	virtual inline void OnAcceleration(const ncAccelerometerEvent &event) { }
	#if (__ANDROID_API__ >= 13)
	virtual inline void OnJoyButtonPressed(const ncJoyButtonEvent &event) { }
	virtual inline void OnJoyButtonReleased(const ncJoyButtonEvent &event) { }
	virtual inline void OnJoyAxisMoved(const ncJoyAxisEvent &event) { }
	#endif
#else
	virtual inline void OnMouseButtonPressed(const ncMouseEvent &event) { }
	virtual inline void OnMouseButtonReleased(const ncMouseEvent &event) { }
	virtual inline void OnMouseMoved(const ncMouseState &state) { }
	virtual inline void OnJoyButtonPressed(const ncJoyButtonEvent &event) { }
	virtual inline void OnJoyButtonReleased(const ncJoyButtonEvent &event) { }
	virtual inline void OnJoyAxisMoved(const ncJoyAxisEvent &event) { }
#endif
};

inline ncIInputEventHandler::~ncIInputEventHandler() { }

#endif
