#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "StaticArray.h"

namespace ncine {

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
	virtual void onFrameStart();
	virtual void onShutdown();

#ifndef __ANDROID__
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
#endif

  private:
	static const unsigned int NumTextures = 4;
	static const unsigned int NumSprites = 8;

	nc::StaticArray<float, NumTextures> angles_;
	nc::StaticArray<nc::Texture*, NumTextures> textures_;
	nc::StaticArray<nc::Sprite*, NumSprites> sprites_;
};

#endif
