#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "ncIAppEventHandler.h"
#include "ncIInputEventHandler.h"
class ncTexture;
class ncSprite;
class ncFont;
class ncTextNode;

/// My nCine event handler
class MyEventHandler
	: public ncIAppEventHandler,
	  public ncIInputEventHandler
{
  public:
	virtual void onInit();
	virtual void onFrameStart();
	virtual void onFrameEnd() { }
	virtual void onShutdown();

#ifndef __ANDROID__
	virtual void onKeyReleased(const ncKeyboardEvent &event);
#endif

  private:
	static const int NumJoysticks = 4;
	static const int NumAxes = 10;
	float axisValues_[NumAxes];
	static const int NumButtons = 12;
	unsigned char buttonStates_[NumButtons];
	static const int NumChars = 256;
	char joyString_[NumChars];

	ncTexture *texture_;
	ncSprite *sprites_[2];
	ncFont *font_;
	ncTextNode *textNode_;
};

#endif
