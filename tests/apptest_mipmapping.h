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
	virtual void OnInit();
	virtual void OnFrameStart();
	virtual void OnFrameEnd() { }
	virtual void OnShutdown();

#ifdef __ANDROID__
	virtual void OnTouchUp(const ncTouchEvent &event);
#else
	virtual void OnKeyReleased(const ncKeyboardEvent &event);
	virtual void OnMouseButtonReleased(const ncMouseEvent &event);
#endif

  private:
	bool m_bPause;
	ncSceneNode *m_pDummy;
	ncTexture *m_pTexture;
	ncSprite *m_pSprite;
	unsigned int m_uDivider;
};

#endif
