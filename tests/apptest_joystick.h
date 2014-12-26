#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"

namespace ncine {

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
	virtual void onInit();
	virtual void onFrameStart();
	virtual void onFrameEnd() { }
	virtual void onShutdown();

#ifndef __ANDROID__
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
#endif

  private:
	static const int NumJoysticks = 4;
	static const int NumAxes = 10;
	float axisValues_[NumAxes];
	static const int NumButtons = 12;
	unsigned char buttonStates_[NumButtons];
	static const int NumChars = 256;
	nc::String *joyString_;

	nc::Texture *texture_;
	nc::Sprite *sprites_[2];
	nc::Font *font_;
	nc::TextNode *textNode_;
};

#endif
