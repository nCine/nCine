#ifndef CLASS_NCAUDIOSTREAMPLAYER
#define CLASS_NCAUDIOSTREAMPLAYER

#include "ncIAudioPlayer.h"
#include "ncAudioStream.h"

/// Audio stream player class
class ncAudioStreamPlayer : public ncIAudioPlayer
{
 public:
	// A constructor creating a player from a file
	ncAudioStreamPlayer(const char *pFilename);
	virtual ~ncAudioStreamPlayer();

	virtual void Play();
	virtual void Pause();
	virtual void Stop();

	// Updates the player state and the stream buffer queue
	virtual void UpdateState();

	inline static eObjectType sType() { return AUDIOSTREAMPLAYER_TYPE; }

 private:
	ncAudioStream m_stream;
};

#endif
