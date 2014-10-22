#ifndef CLASS_NCAUDIOBUFFERPLAYER
#define CLASS_NCAUDIOBUFFERPLAYER

#include "ncIAudioPlayer.h"
class ncAudioBuffer;

/// Audio buffer player class
class ncAudioBufferPlayer : public ncIAudioPlayer
{
  public:
	// A constructor creating a player from a shared buffer
	explicit ncAudioBufferPlayer(ncAudioBuffer *audioBuffer);
	virtual ~ncAudioBufferPlayer() { stop(); }

	virtual void play();
	virtual void pause();
	virtual void stop();

	// Updates the player state
	virtual void updateState();

	inline static ObjectType sType() { return AUDIOBUFFERPLAYER_TYPE; }

  private:
	ncAudioBuffer *audioBuffer_;
};

#endif
