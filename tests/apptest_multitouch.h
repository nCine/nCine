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

#ifdef __ANDROID__
	void handleEvent(const nc::TouchEvent &event, nctl::String *string, const char *eventName);
	void onTouchDown(const nc::TouchEvent &event) override;
	void onTouchUp(const nc::TouchEvent &event) override;
	void onTouchMove(const nc::TouchEvent &event) override;
	void onPointerDown(const nc::TouchEvent &event) override;
	void onPointerUp(const nc::TouchEvent &event) override;
#else
	void onKeyReleased(const nc::KeyboardEvent &event) override;
#endif

  private:
	static const unsigned int MaxNumChars = 1024;
	nctl::UniquePtr<nctl::String> multitouchString_;

#ifdef __ANDROID__
	nctl::UniquePtr<nc::Texture> texture_;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, nc::TouchEvent::MaxPointers> sprites_;
#endif
	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::TextNode> textNode_;
};

#endif
