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
	virtual void onFrameStart() { }
	virtual void onFrameEnd() { }
	virtual void onShutdown();

#ifndef __ANDROID__
	virtual void onKeyReleased(const ncKeyboardEvent &event);
#endif

  private:
	static const int NumSprites = 100;

	ncSceneNode *dummy_;
	ncTexture *megaTexture_;
	ncSprite **sprites_;

	ncTexture *alphaTexture_;
	ncSprite *alphaSpriteBottom_;
	ncSprite *alphaSpriteTop_;
};

#endif
