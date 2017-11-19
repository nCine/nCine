#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"

namespace ncine {

class AppConfiguration;
class AudioBuffer;
class IAudioPlayer;
class SceneNode;
class Font;
class TextNode;
class String;

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

#ifdef __ANDROID__
	void onTouchUp(const nc::TouchEvent &event) override;
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;

  private:
	float gain_;
	float pitch_;
	float xPos_;
	bool isLooping_;

	nc::AudioBuffer *audioBuffer_;
	nc::IAudioPlayer *soundPlayer_;
	nc::IAudioPlayer *musicPlayer_;
	nc::SceneNode *dummy_;
	nc::Font *font_;
	nc::TextNode *textNode_;
	nc::String *textString_;

	void toggleMusic();
	void toggleSound();
};

#endif
