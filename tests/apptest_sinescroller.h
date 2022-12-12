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
	void onResizeWindow(int width, int height) override;

#ifdef __ANDROID__
	void onTouchDown(const nc::TouchEvent &event) override;
	void onTouchUp(const nc::TouchEvent &event) override;
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
	void onMouseButtonReleased(const nc::MouseEvent &event) override;

	void onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event) override;

  private:
	static const unsigned int NumChars = 20;

	bool pause_;
	float angle_;

	nctl::UniquePtr<nctl::String> debugString_;
	nctl::UniquePtr<nc::TextNode> debugText_;
	nctl::UniquePtr<nc::Font> font_;
	nctl::StaticArray<nctl::UniquePtr<nc::TextNode>, NumChars> characters_;

	void checkClick(float x, float y);
};

#endif
