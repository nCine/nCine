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
private:
	ncTexture *m_pTexture;
	ncSprite *m_pSprites[2];
	ncFont *m_pFont;
	ncTextNode *m_pTextNode;

	static const int numAxes = 8;
	float m_fAxisValues[numAxes];
	static const int numButtons = 12;
	unsigned char m_ubButtonStates[numButtons];
	static const int numChars = 256;
	char m_vJoyString[numChars];
public:
	virtual void OnInit();
	virtual void OnFrameStart();
	virtual void OnFrameEnd() { }
	virtual void OnShutdown();

#ifndef __ANDROID__
	virtual void OnKeyReleased(const ncKeyboardEvent &event);
#endif
};

#endif
