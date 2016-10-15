#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "StaticArray.h"

namespace ncine {

class AppConfiguration;
class String;
class Texture;
class Sprite;
class Font;
class TextNode;

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

#ifndef __ANDROID__
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
#endif

  private:
	static const int MaxNumJoysticks = 4;
	static const int MaxNumAxes = 16;
	nc::StaticArray<float, MaxNumAxes> axisValues_;
	static const int MaxNumButtons = 16;
	nc::StaticArray<unsigned char, MaxNumButtons> buttonStates_;
	static const int MaxNumChars = 256;
	nc::String *joyString_;

	nc::Texture *texture_;
	nc::StaticArray<nc::Sprite *, 2> sprites_;
	nc::Font *font_;
	nc::TextNode *textNode_;
};

#endif
