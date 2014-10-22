#ifndef CLASS_NCAUDIOSTREAM
#define CLASS_NCAUDIOSTREAM

#ifdef __APPLE__
	#include <OpenAL/al.h>
#else
	#include <AL/al.h>
#endif
#include "ncIAudioLoader.h"

/// Audio stream class
class ncAudioStream
{
  public:
	virtual ~ncAudioStream();

	// Enqueues new buffers and unqueues processed ones
	bool enqueue(ALuint source, bool looping);
	// Unqueues any left buffer and rewinds the loader
	void stop(ALuint source);

	/// Returns samples frequency
	inline int frequency() const { return frequency_; }

  private:
	/// Number of buffers for streaming
	static const int NumBuffers = 3;
	/// OpenAL buffer queue for streaming
	ALuint alBuffers_[NumBuffers];
	/// Index of the next available OpenAL buffer
	int nextAvailALBuffer_;

	/// Size in bytes of each streaming buffer
	static const int BufferSize = 16 * 1024;
	/// Memory buffer to feed OpenAL ones
	char *memBuffer_;

	/// Samples frequency
	int frequency_;
	/// OpenAL channel format enumeration
	ALenum format_;
	/// The associated loader to continuosly stream decoded data
	ncIAudioLoader *audioLoader_;

	// Constructor creating an audio stream from an audio file
	ncAudioStream(const char *filename);
	/// Preventing construction by copy
	ncAudioStream(const ncAudioStream& other);

	friend class ncAudioStreamPlayer;
};

#endif
