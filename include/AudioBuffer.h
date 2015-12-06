#ifndef CLASS_NCINE_AUDIOBUFFER
#define CLASS_NCINE_AUDIOBUFFER

#include "Object.h"

namespace ncine {

class IAudioLoader;

/// A class representing an OpenAL buffer
/*! It inherits from Object because a buffer can be shared by more than one AudioBufferPlayer object */
class DLL_PUBLIC AudioBuffer : public Object
{
  public:
	AudioBuffer();
	// A constructor creating a buffer from a file
	explicit AudioBuffer(const char *filename);
	virtual ~AudioBuffer();

	/// Returns the OpenAL buffer id
	inline unsigned int bufferId() const { return bufferId_; }
	/// Returns number of audio channels
	inline int numChannels() const { return numChannels_; }
	/// Returns samples frequency
	inline int frequency() const { return frequency_; }

	inline static ObjectType sType() { return AUDIOBUFFER_TYPE; }

  private:
	/// The OpenAL buffer id
	unsigned int bufferId_;
	/// Number of channels
	int numChannels_;
	/// Samples frequency
	int frequency_;

	/// Private copy constructor
	AudioBuffer(const AudioBuffer&);
	/// Private assignment operator
	AudioBuffer& operator=(const AudioBuffer&);

	// Loads audio samples based on information from the audio loader
	void load(const IAudioLoader *audioLoader);
};

}

#endif
