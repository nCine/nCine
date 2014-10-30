#ifndef CLASS_NCINE_AUDIOBUFFERPLAYER
#define CLASS_NCINE_AUDIOBUFFERPLAYER

#include "IAudioPlayer.h"

namespace ncine {

class AudioBuffer;

/// Audio buffer player class
class AudioBufferPlayer : public IAudioPlayer
{
  public:
	// A constructor creating a player from a shared buffer
	explicit AudioBufferPlayer(AudioBuffer *audioBuffer);
	virtual ~AudioBufferPlayer() { stop(); }

	virtual void play();
	virtual void pause();
	virtual void stop();

	// Updates the player state
	virtual void updateState();

	inline static ObjectType sType() { return AUDIOBUFFERPLAYER_TYPE; }

  private:
	AudioBuffer *audioBuffer_;
};

}

#endif
