#ifndef CLASS_NCAUDIOLOADER
#define CLASS_NCAUDIOLOADER

#ifdef __ANDROID__
#include "ivorbisfile.h"
#else
#include <vorbis/vorbisfile.h>
#endif

/// Audio loader class
class ncAudioLoader
{
private:
	/// Duration in seconds
	float m_fDuration;
	/// Number of samples
	unsigned long int m_ulNumSamples;
	/// Number of channels
	int m_iChannels;
	/// Samples frequency
	int m_iFrequency;
	/// Vorbisfile handle
	mutable OggVorbis_File m_oggFile;

	/// Sets up the loader for an Ogg Vorbis file
	void LoadOgg(const char *pFilename);
public:
	ncAudioLoader()
		: m_fDuration(0.0f), m_ulNumSamples(0L), m_iChannels(0), m_iFrequency(0) { }
	ncAudioLoader(const char *pFilename);
	~ncAudioLoader();

	// Loads audio data from file
	void Load(const char *pFilename);
	// Decodes audio data in a specified buffer
	long Read(char *pBuffer, int iBufSize) const;
	// Resets the audio file seek value
	void Rewind() const;

	/// Returns the duration in seconds
	inline float Duration() const { return m_fDuration; }
	/// Returns number of samples
	inline unsigned long int NumSamples() const { return m_ulNumSamples; }
	/// Returns number of channels
	inline int Channels() const { return m_iChannels; }
	/// Returns samples frequency
	inline int Frequency() const { return m_iFrequency; }
};

#endif
