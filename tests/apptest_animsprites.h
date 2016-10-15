#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "Vector2.h"

namespace ncine {

class AppConfiguration;
class Texture;
class AnimatedSprite;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler
	: public nc::IAppEventHandler,
	  public nc::IInputEventHandler
{
  public:
	virtual void onPreInit(nc::AppConfiguration &config);
	virtual void onInit();
	virtual void onFrameStart();
	virtual void onShutdown();

#ifdef __ANDROID__
	virtual void onTouchDown(const nc::TouchEvent &event);
	virtual void onTouchMove(const nc::TouchEvent &event);
#else
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
	virtual void onMouseButtonPressed(const nc::MouseEvent &event);
	virtual void onMouseMoved(const nc::MouseState &state);
#endif
	virtual void onJoyAxisMoved(const nc::JoyAxisEvent &event);
	virtual void onJoyDisconnected(const nc::JoyConnectionEvent &event);

  private:
	nc::Texture *texture_;
	nc::AnimatedSprite *animSprite_;
	nc::Vector2f destVector_;
	nc::Vector2f joyVector_;
};

#endif
