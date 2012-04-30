#ifndef CLASS_NCAUDIOBUFFERPLAYER
#define CLASS_NCAUDIOBUFFERPLAYER

#include "ncIAudioPlayer.h"
class ncAudioBuffer;

/// Audio buffer player class
class ncAudioBufferPlayer : public ncIAudioPlayer
{
private:
	ncAudioBuffer *m_pBuffer;

public:
	// A constructor creating a player from a shared buffer
	ncAudioBufferPlayer(ncAudioBuffer *pBuffer);
	virtual ~ncAudioBufferPlayer() { Stop(); }

	virtual void Play();
	virtual void Pause();
	virtual void Stop();

	// Updates the player state
	virtual void UpdateState();

	inline static eObjectType sType() { return AUDIOBUFFERPLAYER_TYPE; }
};

#endif
