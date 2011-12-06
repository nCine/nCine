#ifndef CLASS_NCIINPUTEVENTHANDLER
#define CLASS_NCIINPUTEVENTHANDLER

#include "ncIInputManager.h"

/// The interface class for handling input events (i.e. mouse, keyboard, touch, sensors)
class ncIInputEventHandler
{
public:
// TODO: virtual layer for events, something like ncEvent, common on all platforms
#ifdef __ANDROID__
	virtual void OnKeyPressed(const ncKeyboardEvent &event) = 0;
	virtual void OnKeyReleased(const ncKeyboardEvent &event) = 0;
	virtual void OnTouchDown(const ncTouchEvent &event) = 0;
	virtual void OnTouchUp(const ncTouchEvent &event) = 0;
	virtual void OnTouchMove(const ncTouchEvent &event) = 0;
	virtual void OnSecondaryTouchDown(const ncTouchEvent &event) = 0;
	virtual void OnSecondaryTouchUp(const ncTouchEvent &event) = 0;
#else
	virtual void OnKeyPressed(const ncKeyboardEvent &event) = 0;
	virtual void OnKeyReleased(const ncKeyboardEvent &event) = 0;
	virtual void OnMouseButtonPressed(const ncMouseEvent &event) = 0;
	virtual void OnMouseButtonReleased(const ncMouseEvent &event) = 0;
	virtual void OnMouseMoved(const ncMouseState &state) = 0;
#endif
};

#endif
