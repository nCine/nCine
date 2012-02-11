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
private:
	ncAudioStreamPlayer *m_pAudioPlayer;
	ncTexture *m_pTexture;
	ncAnimatedSprite *m_pAnimSprite;
	ncVector2f m_destVector;

public:
	virtual void OnInit();
	virtual void OnFrameStart();
	virtual void OnFrameEnd() { }
	virtual void OnShutdown();

	virtual void OnKeyPressed(const ncKeyboardEvent &event) { }
#ifdef __ANDROID__
	virtual void OnKeyReleased(const ncKeyboardEvent &event) { }
	virtual void OnTouchDown(const ncTouchEvent &event);
	virtual void OnTouchUp(const ncTouchEvent &event) { }
	virtual void OnTouchMove(const ncTouchEvent &event);
	virtual void OnSecondaryTouchDown(const ncTouchEvent &event) { }
	virtual void OnSecondaryTouchUp(const ncTouchEvent &event) { }
#else
	virtual void OnKeyReleased(const ncKeyboardEvent &event);
	virtual void OnMouseButtonPressed(const ncMouseEvent &event);
	virtual void OnMouseButtonReleased(const ncMouseEvent &event) { }
	virtual void OnMouseMoved(const ncMouseState &state);
#endif
};

#endif
