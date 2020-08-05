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
	/// A constructor creating a player from a named memory buffer
	AudioStreamPlayer(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	/// A constructor creating a player from a file
	explicit AudioStreamPlayer(const char *filename);
	~AudioStreamPlayer() override;

	inline unsigned int bufferId() const override { return audioStream_.bufferId(); }
	inline int numChannels() const override { return audioStream_.numChannels(); }
	inline int frequency() const override { return audioStream_.frequency(); }
	unsigned long bufferSize() const override { return audioStream_.bufferSize(); }

	void play() override;
	void pause() override;
	void stop() override;

	/// Updates the player state and the stream buffer queue
	void updateState() override;

	inline static ObjectType sType() { return ObjectType::AUDIOSTREAM_PLAYER; }

  private:
	AudioStream audioStream_;

	/// Deleted copy constructor
	AudioStreamPlayer(const AudioStreamPlayer &) = delete;
	/// Deleted assignment operator
	AudioStreamPlayer &operator=(const AudioStreamPlayer &) = delete;
};

}

#endif
