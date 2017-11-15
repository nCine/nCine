#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "StaticArray.h"

namespace ncine {

class AppConfiguration;
class Texture;
class Sprite;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler :
	public nc::IAppEventHandler,
	public nc::IInputEventHandler
{
  public:
	virtual void onPreInit(nc::AppConfiguration &config);
	virtual void onInit();
	virtual void onFrameStart();
	virtual void onShutdown();

#ifdef __ANDROID__
	virtual void onTouchDown(const nc::TouchEvent &event);
	virtual void onTouchUp(const nc::TouchEvent &event);
#endif
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
	virtual void onMouseButtonPressed(const nc::MouseEvent &event);
	virtual void onMouseButtonReleased(const nc::MouseEvent &event);

  private:
	static const unsigned int NumTextures = 4;
	static const unsigned int NumSprites = 8;

	bool pause_;
	float angle_;
	nc::StaticArray<nc::Texture *, NumTextures> textures_;
	nc::StaticArray<nc::Sprite *, NumSprites> sprites_;
};

#endif
