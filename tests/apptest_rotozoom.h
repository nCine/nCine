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
	virtual void onTouchDown(const ncTouchEvent &event);
	virtual void onTouchUp(const ncTouchEvent &event);
#else
	virtual void onKeyReleased(const ncKeyboardEvent &event);
	virtual void onMouseButtonPressed(const ncMouseEvent &event);
	virtual void onMouseButtonReleased(const ncMouseEvent &event);
#endif

  private:
	static const int NumTextures = 4;
	static const int NumRowSprites = 8;
	static const int NumColSprites = 8;

	bool pause_;
	float angle_;
	ncSceneNode *dummy_;
	ncTexture *megaTexture_;
	ncSprite **sprites_;
};

#endif
