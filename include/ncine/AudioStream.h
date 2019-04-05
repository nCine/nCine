#ifndef CLASS_NCINE_AUDIOSTREAM
#define CLASS_NCINE_AUDIOSTREAM

#include "IAudioLoader.h"
#include <nctl/StaticArray.h>

namespace ncine {

/// Audio stream class
class DLL_PUBLIC AudioStream
{
  public:
	virtual ~AudioStream();

	/// Enqueues new buffers and unqueues processed ones
	bool enqueue(unsigned int source, bool looping);
	/// Unqueues any left buffer and rewinds the loader
	void stop(unsigned int source);

	/// Returns samples frequency
	inline int frequency() const { return frequency_; }

  private:
	/// Number of buffers for streaming
	static const int NumBuffers = 3;
	/// OpenAL buffer queue for streaming
	nctl::StaticArray<unsigned int, NumBuffers> alBuffers_;
	/// Index of the next available OpenAL buffer
	int nextAvailALBuffer_;

	/// Size in bytes of each streaming buffer
	static const int BufferSize = 16 * 1024;
	/// Memory buffer to feed OpenAL ones
	nctl::UniquePtr<char []> memBuffer_;

	/// Samples frequency
	int frequency_;
	/// OpenAL channel format enumeration
	int format_;
	/// The associated loader to continuosly stream decoded data
	nctl::UniquePtr<IAudioLoader> audioLoader_;

	/// Constructor creating an audio stream from an audio file
	explicit AudioStream(const char *filename);
	/// Deleted copy constructor
	AudioStream(const AudioStream &) = delete;
	/// Deleted assignment operator
	AudioStream &operator=(const AudioStream &) = delete;

	friend class AudioStreamPlayer;
};

}

#endif
