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
	virtual void OnFrameStart() { }
	virtual void OnFrameEnd() { }
	virtual void OnShutdown();

#ifndef __ANDROID__
	virtual void OnKeyReleased(const ncKeyboardEvent &event);
#endif

  private:
	ncSceneNode *m_pDummy;
	ncTexture *m_pMegaTexture;
	ncSprite **m_pSprites;

	ncTexture *m_pAlphaTexture;
	ncSprite *m_pAlphaSpriteBottom;
	ncSprite *m_pAlphaSpriteTop;
};

#endif
