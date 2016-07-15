#ifndef CLASS_NCINE_AUDIOSTREAMPLAYER
#define CLASS_NCINE_AUDIOSTREAMPLAYER

#include "common_defines.h"
#include "IAudioPlayer.h"
#include "AudioStream.h"

namespace ncine {

/// Audio stream player class
class DLL_PUBLIC AudioStreamPlayer : public IAudioPlayer
{
  public:
	/// A constructor creating a player from a file
	explicit AudioStreamPlayer(const char *filename);
	virtual ~AudioStreamPlayer();

	virtual void play();
	virtual void pause();
	virtual void stop();

	/// Updates the player state and the stream buffer queue
	virtual void updateState();

	inline static ObjectType sType() { return AUDIOSTREAMPLAYER_TYPE; }

  private:
	AudioStream audioStream_;

	/// Private copy constructor
	AudioStreamPlayer(const AudioStreamPlayer &);
	/// Private assignment operator
	AudioStreamPlayer &operator=(const AudioStreamPlayer &);
};

}

#endif
