#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"

namespace ncine {

class AudioBuffer;
class IAudioPlayer;

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
	virtual void onShutdown();

#ifndef __ANDROID__
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
#endif

  private:
	static const float DefaultGain;
	float gain_;
	static const float DefaultPitch;
	float pitch_;
	static const float DefaultXPos;
	float xPos_;
	bool isLooping_;

	nc::AudioBuffer *audioBuffer_;
	nc::IAudioPlayer *player_;
};

#endif
