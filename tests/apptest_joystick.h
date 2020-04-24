#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/StaticArray.h>

namespace nctl {

class String;

}

namespace ncine {

class AppConfiguration;
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
	MyEventHandler()
	    : axisValues_(nctl::StaticArrayMode::EXTEND_SIZE),
	      buttonStates_(nctl::StaticArrayMode::EXTEND_SIZE),
	      hatStates_(nctl::StaticArrayMode::EXTEND_SIZE)
	{}

	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;

	void onKeyReleased(const nc::KeyboardEvent &event) override;

  private:
	static const unsigned int MaxNumAxes = 16;
	nctl::StaticArray<float, MaxNumAxes> axisValues_;
	static const unsigned int MaxNumButtons = 20;
	nctl::StaticArray<unsigned char, MaxNumButtons> buttonStates_;
	static const unsigned int MaxNumHats = 2;
	nctl::StaticArray<unsigned char, MaxNumHats> hatStates_;
	static const unsigned int MaxNumChars = 384;
	nctl::UniquePtr<nctl::String> joyString_;

	nctl::UniquePtr<nc::Texture> texture_;
	nctl::UniquePtr<nc::SceneNode> joyNode_;
	static const unsigned int NumSprites = 17;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, NumSprites> sprites_;
	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::TextNode> textNode_;
};

#endif
