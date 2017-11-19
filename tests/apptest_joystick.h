#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "StaticArray.h"

namespace ncine {

class AppConfiguration;
class String;
class Texture;
class SceneNode;
class Sprite;
class Font;
class TextNode;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler :
	public nc::IAppEventHandler,
	public nc::IInputEventHandler
{
  public:
	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;
	void onShutdown() override;

	void onKeyReleased(const nc::KeyboardEvent &event) override;

  private:
	static const unsigned int MaxNumAxes = 16;
	nc::StaticArray<float, MaxNumAxes> axisValues_;
	static const unsigned int MaxNumButtons = 20;
	nc::StaticArray<unsigned char, MaxNumButtons> buttonStates_;
	static const unsigned int MaxNumChars = 384;
	nc::String *joyString_;

	nc::Texture *texture_;
	nc::SceneNode *joyNode_;
	static const unsigned int NumSprites = 13;
	nc::StaticArray<nc::Sprite *, NumSprites> sprites_;
	nc::Font *font_;
	nc::TextNode *textNode_;
};

#endif
