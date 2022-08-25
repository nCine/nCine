#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/config.h>
#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/StaticArray.h>

#if defined(__ANDROID__) || NCINE_WITH_QT5 || NCINE_WITH_SDL
	#define HAS_TOUCH (1)
#endif

#include <nctl/String.h>

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
	MyEventHandler();
	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;

#if HAS_TOUCH
	void handleEvent(const nc::TouchEvent &event, nctl::String &string, const char *eventName);
	void onTouchDown(const nc::TouchEvent &event) override;
	void onTouchUp(const nc::TouchEvent &event) override;
	void onTouchMove(const nc::TouchEvent &event) override;
	void onPointerDown(const nc::TouchEvent &event) override;
	void onPointerUp(const nc::TouchEvent &event) override;
#endif

#ifndef __ANDROID__
	void onKeyReleased(const nc::KeyboardEvent &event) override;
#endif

  private:
	nctl::String multitouchString_;
	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::TextNode> textNode_;

#if HAS_TOUCH
	nctl::UniquePtr<nc::Texture> texture_;
	nctl::StaticArray<nctl::UniquePtr<nc::SceneNode>, nc::TouchEvent::MaxPointers> groupNodes_;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, nc::TouchEvent::MaxPointers> sprites_;
	nctl::StaticArray<nctl::UniquePtr<nc::TextNode>, nc::TouchEvent::MaxPointers> spriteTexts_;
#endif
};

#endif
