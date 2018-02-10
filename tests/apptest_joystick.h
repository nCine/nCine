#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "nctl/StaticArray.h"
#include "nctl/UniquePtr.h"

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
	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;

	void onKeyReleased(const nc::KeyboardEvent &event) override;

  private:
	static const unsigned int MaxNumAxes = 16;
	nctl::StaticArray<float, MaxNumAxes> axisValues_;
	static const unsigned int MaxNumButtons = 20;
	nctl::StaticArray<unsigned char, MaxNumButtons> buttonStates_;
	static const unsigned int MaxNumChars = 384;
	nctl::UniquePtr<nctl::String> joyString_;

	nctl::UniquePtr<nc::Texture> texture_;
	nctl::UniquePtr<nc::SceneNode> joyNode_;
	static const unsigned int NumSprites = 13;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, NumSprites> sprites_;
	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::TextNode> textNode_;
};

#endif
