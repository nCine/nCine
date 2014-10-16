#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "ncIAppEventHandler.h"
#include "ncIInputEventHandler.h"
class ncSceneNode;
class ncTexture;
class ncSprite;

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

#ifdef __ANDROID__
	virtual void onTouchUp(const ncTouchEvent &event);
#else
	virtual void onKeyReleased(const ncKeyboardEvent &event);
	virtual void onMouseButtonReleased(const ncMouseEvent &event);
#endif

  private:
	bool pause_;
	ncSceneNode *dummy_;
	ncTexture *texture_;
	ncSprite *sprite_;
	unsigned int divider_;
};

#endif
