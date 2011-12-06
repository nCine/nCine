#ifndef CLASS_NCANDROIDINPUTMANAGER
#define CLASS_NCANDROIDINPUTMANAGER

#include "ncIInputManager.h"

/// The class for parsing and dispatching Android input events
class ncAndroidInputManager : public ncIInputManager
{
private:
	ncTouchEvent m_touchEvent;
	ncKeyboardEvent m_keyboardEvent;

public:
	ncAndroidInputManager() { }

	void ParseEvent(const AInputEvent* event);
};

#endif
