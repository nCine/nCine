#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"

namespace ncine {

class SceneNode;
class Texture;
class Sprite;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler
	: public nc::IAppEventHandler,
	  public nc::IInputEventHandler
{
  public:
	virtual void onInit();
	virtual void onFrameStart() { }
	virtual void onFrameEnd() { }
	virtual void onShutdown();

#ifndef __ANDROID__
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
#endif

  private:
	static const int NumSprites = 100;

	nc::SceneNode *dummy_;
	nc::Texture *megaTexture_;
	nc::Sprite **sprites_;

	nc::Texture *alphaTexture_;
	nc::Sprite *alphaSpriteBottom_;
	nc::Sprite *alphaSpriteTop_;
};

#endif
