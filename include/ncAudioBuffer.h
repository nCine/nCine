#ifndef CLASS_NCAUDIOBUFFER
#define CLASS_NCAUDIOBUFFER

#ifdef __APPLE__
	#include <OpenAL/al.h>
#else
	#include <AL/al.h>
#endif

#include "ncObject.h"
class ncIAudioLoader;

/// A class representing an OpenAL buffer
/*! It is a ncObject because a buffer can be shared by more than one ncAudioBufferPlayer  */
class ncAudioBuffer : public ncObject
{
  public:
	ncAudioBuffer();
	// A constructor creating a buffer from a file
	ncAudioBuffer(const char *filename);
	virtual ~ncAudioBuffer();

	/// Returns the OpenAL buffer id
	inline ALuint bufferId() const { return bufferId_; }
	/// Returns number of audio channels
	inline int numChannels() const { return numChannels_; }
	/// Returns samples frequency
	inline int frequency() const { return frequency_; }

	inline static ObjectType sType() { return AUDIOBUFFER_TYPE; }

  private:
	/// The OpenAL buffer id
	ALuint bufferId_;
	/// Number of channels
	int numChannels_;
	/// Samples frequency
	int frequency_;

	// Loads audio samples based on information from the audio loader
	void load(const ncIAudioLoader *audioLoader);
};

#endif
