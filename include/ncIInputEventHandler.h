#ifndef CLASS_NCIINPUTEVENTHANDLER
#define CLASS_NCIINPUTEVENTHANDLER

#include "ncIInputManager.h"

/// The interface class for handling input events (i.e. mouse, keyboard, touch, sensors)
class ncIInputEventHandler
{
public:
	/// Pure virtual destructor in order to make the class abstract
	virtual ~ncIInputEventHandler() = 0;
#ifdef __ANDROID__
	virtual inline void OnKeyPressed(const ncKeyboardEvent &event) { }
	virtual inline void OnKeyReleased(const ncKeyboardEvent &event) { }
	virtual inline void OnTouchDown(const ncTouchEvent &event) { }
	virtual inline void OnTouchUp(const ncTouchEvent &event) { }
	virtual inline void OnTouchMove(const ncTouchEvent &event) { }
	virtual inline void OnSecondaryTouchDown(const ncTouchEvent &event) { }
	virtual inline void OnSecondaryTouchUp(const ncTouchEvent &event) { }
	virtual inline void OnAcceleration(const ncAccelerometerEvent &event) { }
#else
	virtual inline void OnKeyPressed(const ncKeyboardEvent &event) { }
	virtual inline void OnKeyReleased(const ncKeyboardEvent &event) { }
	virtual inline void OnMouseButtonPressed(const ncMouseEvent &event) { }
	virtual inline void OnMouseButtonReleased(const ncMouseEvent &event) { }
	virtual inline void OnMouseMoved(const ncMouseState &state) { }
#endif
};

inline ncIInputEventHandler::~ncIInputEventHandler() { }

#endif
