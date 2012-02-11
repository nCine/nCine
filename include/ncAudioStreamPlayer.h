#ifndef CLASS_NCAUDIOSTREAMPLAYER
#define CLASS_NCAUDIOSTREAMPLAYER

#include "ncIAudioPlayer.h"
#include "ncAudioStream.h"

/// Audio stream player class
class ncAudioStreamPlayer : public ncIAudioPlayer
{
private:
	ncAudioStream m_stream;

public:
	// A constructor creating a player from a file
	ncAudioStreamPlayer(const char *pFilename);
	virtual ~ncAudioStreamPlayer();

	virtual void Play();
	virtual void Pause();
	virtual void Stop();

	// Updates the stream buffer queue
	void UpdateStream();

	inline static eObjectType sType() { return AUDIOSTREAMPLAYER_TYPE; }
	static ncAudioStreamPlayer* FromId(unsigned int uId);
};

#endif
