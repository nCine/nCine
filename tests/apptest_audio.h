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
class MyEventHandler
	: public nc::IAppEventHandler,
	  public nc::IInputEventHandler
{
  public:
	virtual void onPreInit(nc::AppConfiguration &config);
	virtual void onInit();
	virtual void onFrameStart();
	virtual void onShutdown();

#ifdef __ANDROID__
	virtual void onTouchUp(const nc::TouchEvent &event);
#else
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
#endif

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
