#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler
	: public nc::IAppEventHandler,
	  public nc::IInputEventHandler
{
  public:
	virtual void onInit();

#ifndef __ANDROID__
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
#endif
};

#endif
