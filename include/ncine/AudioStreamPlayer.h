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
	/// Default constructor
	AudioStreamPlayer();
	/// A constructor creating a player from a named memory buffer
	AudioStreamPlayer(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	/// A constructor creating a player from a file
	explicit AudioStreamPlayer(const char *filename);
	~AudioStreamPlayer() override;

	bool loadFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	bool loadFromFile(const char *filename);

	inline unsigned int bufferId() const override { return audioStream_.bufferId(); }

	inline int bytesPerSample() const override { return audioStream_.bytesPerSample(); }
	inline int numChannels() const override { return audioStream_.numChannels(); }
	inline int frequency() const override { return audioStream_.frequency(); }

	inline unsigned long int numSamples() const override { return audioStream_.numSamples(); }
	inline float duration() const override { return audioStream_.duration(); }

	inline unsigned long bufferSize() const override { return audioStream_.bufferSize(); }

	inline unsigned long int numStreamSamples() const { return audioStream_.numStreamSamples(); }
	inline int streamBufferSize() const { return audioStream_.streamBufferSize(); }

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
