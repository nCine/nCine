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
	virtual void OnInit();
	virtual void OnFrameStart();
	virtual void OnFrameEnd() { }
	virtual void OnShutdown();

#ifdef __ANDROID__
	virtual void OnTouchDown(const ncTouchEvent &event);
	virtual void OnTouchMove(const ncTouchEvent &event);
#else
	virtual void OnKeyReleased(const ncKeyboardEvent &event);
	virtual void OnMouseButtonPressed(const ncMouseEvent &event);
	virtual void OnMouseMoved(const ncMouseState &state);
#endif

 private:
	ncAudioStreamPlayer *m_pAudioPlayer;
	ncTexture *m_pTexture;
	ncAnimatedSprite *m_pAnimSprite;
	ncVector2f m_destVector;
};

#endif
