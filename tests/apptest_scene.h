#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "ncIAppEventHandler.h"
class ncTexture;
class ncSprite;

/// My nCine event handler
class MyEventHandler : public ncIAppEventHandler
{
private:
	float *m_fAngles;
	ncTexture **m_pTextures;
	ncSprite **m_pSprites;

public:
	virtual void OnInit();
	virtual void OnFrameStart();
	virtual void OnFrameEnd() { }
	virtual void OnShutdown();
};

#endif
