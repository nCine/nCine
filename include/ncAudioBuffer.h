#ifndef CLASS_NCAUDIOBUFFER
#define CLASS_NCAUDIOBUFFER

#include <AL/al.h>
#include "ncObject.h"
#include "ncAudioLoader.h"

/// A class representing an OpenAL buffer
/*! It is a ncObject because a buffer can be shared by more than one ncAudioBufferPlayer  */
class ncAudioBuffer : public ncObject
{
private:
	/// The OpenAL buffer id
	ALuint m_uALId;
	/// Number of channels
	int m_iChannels;
	/// Samples frequency
	int m_iFrequency;

	// Loads audio samples based on information from the audio loader
	void Load(const ncAudioLoader &audioLoader);
public:
	ncAudioBuffer();
	// A constructor creating a buffer from a file
	ncAudioBuffer(const char *pFilename);
	virtual ~ncAudioBuffer();

	/// Returns the OpenAL buffer id
	inline ALuint ALId() const { return m_uALId; }
	/// Returns number of audio channels
	inline int Channels() const { return m_iChannels; }
	/// Returns samples frequency
	inline int Frequency() const { return m_iFrequency; }

	inline static eObjectType sType() { return AUDIOBUFFER_TYPE; }
	static ncAudioBuffer* FromId(unsigned int uId);
};

#endif
