#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "ncIAppEventHandler.h"
#include "ncIInputEventHandler.h"

/// My nCine event handler
class MyEventHandler
	: public ncIAppEventHandler,
	  public ncIInputEventHandler
{
  public:
	virtual void onInit();
	virtual void onFrameStart() { }
	virtual void onFrameEnd() { }
	virtual void onShutdown() { }

#ifndef __ANDROID__
	virtual void onKeyReleased(const ncKeyboardEvent &event);
#endif
};

#endif
