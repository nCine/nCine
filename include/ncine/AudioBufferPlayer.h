#ifndef CLASS_NCINE_AUDIOBUFFERPLAYER
#define CLASS_NCINE_AUDIOBUFFERPLAYER

#include "common_defines.h"
#include "IAudioPlayer.h"

namespace ncine {

class AudioBuffer;

/// Audio buffer player class
class DLL_PUBLIC AudioBufferPlayer : public IAudioPlayer
{
  public:
	/// A constructor creating a player from a shared buffer
	explicit AudioBufferPlayer(AudioBuffer *audioBuffer);
	inline ~AudioBufferPlayer() override { stop(); }

	unsigned int bufferId() const override;

	int bytesPerSample() const override;
	int numChannels() const override;
	int frequency() const override;

	unsigned long int numSamples() const override;
	float duration() const override;

	unsigned long int bufferSize() const override;

	void play() override;
	void pause() override;
	void stop() override;

	/// Updates the player state
	void updateState() override;

	inline static ObjectType sType() { return ObjectType::AUDIOBUFFER_PLAYER; }

  private:
	AudioBuffer *audioBuffer_;

	/// Deleted copy constructor
	AudioBufferPlayer(const AudioBufferPlayer &) = delete;
	/// Deleted assignment operator
	AudioBufferPlayer &operator=(const AudioBufferPlayer &) = delete;
};

}

#endif
