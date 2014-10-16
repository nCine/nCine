#ifndef CLASS_NCAUDIOSTREAMPLAYER
#define CLASS_NCAUDIOSTREAMPLAYER

#include "ncIAudioPlayer.h"
#include "ncAudioStream.h"

/// Audio stream player class
class ncAudioStreamPlayer : public ncIAudioPlayer
{
  public:
	// A constructor creating a player from a file
	ncAudioStreamPlayer(const char *filename);
	virtual ~ncAudioStreamPlayer();

	virtual void play();
	virtual void pause();
	virtual void stop();

	// Updates the player state and the stream buffer queue
	virtual void updateState();

	inline static ObjectType sType() { return AUDIOSTREAMPLAYER_TYPE; }

  private:
	ncAudioStream audioStream_;
};

#endif
