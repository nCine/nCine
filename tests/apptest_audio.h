#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/UniquePtr.h>

namespace nctl {

class String;

}

namespace ncine {

class AppConfiguration;
class AudioBuffer;
class IAudioPlayer;
class SceneNode;
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
	void onTouchUp(const nc::TouchEvent &event) override;
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;

  private:
	float gain_;
	float pitch_;
	float xPos_;
	bool isLooping_;

	nctl::UniquePtr<nc::AudioBuffer> audioBuffer_;
	nctl::UniquePtr<nc::IAudioPlayer> soundPlayer_;
	nctl::UniquePtr<nc::IAudioPlayer> musicPlayer_;
	nctl::UniquePtr<nc::SceneNode> dummy_;
	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::TextNode> textNode_;
	nctl::UniquePtr<nctl::String> textString_;

	void toggleMusic();
	void toggleSound();
};

#endif
