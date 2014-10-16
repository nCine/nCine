#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "ncIAppEventHandler.h"
#include "ncIInputEventHandler.h"
#include "ncVector2f.h"
class ncAudioStreamPlayer;
class ncTexture;
class ncAnimatedSprite;

/// My nCine application
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
	virtual void onTouchMove(const ncTouchEvent &event);
#else
	virtual void onKeyReleased(const ncKeyboardEvent &event);
	virtual void onMouseButtonPressed(const ncMouseEvent &event);
	virtual void onMouseMoved(const ncMouseState &state);
#endif

  private:
	ncAudioStreamPlayer *audioPlayer_;
	ncTexture *texture_;
	ncAnimatedSprite *animSprite_;
	ncVector2f destVector_;
};

#endif
