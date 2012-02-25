#ifndef CLASS_NCIAUDIOLOADER
#define CLASS_NCIAUDIOLOADER

#include "ncFile.h"

/// Audio loader class
class ncIAudioLoader
{
protected:
	/// Audio file handle
	ncFile m_fileHandle;

	/// Number of bytes per sample
	int m_iBytesPerSample;
	/// Number of channels
	int m_iChannels;
	/// Samples frequency
	int m_iFrequency;

	/// Number of samples
	unsigned long int m_ulNumSamples;
	/// Duration in seconds
	float m_fDuration;

	ncIAudioLoader(const char *pFilename)
		: m_fileHandle(pFilename), m_iBytesPerSample(0), m_iChannels(0), m_iFrequency(0), m_ulNumSamples(0L), m_fDuration(0.0f) { }

	/// Load the audio parameters from file
	virtual void Init() = 0;

public:
	virtual ~ncIAudioLoader() { }

	/// Decodes audio data in a specified buffer
	/*! \param pBuffer Buffer pointer
	 *  \param iBufSize Buffer size
	 *  \return Number of bytes read.
	 */
	virtual long Read(char *pBuffer, int iBufSize) const = 0;
	/// Resets the audio file seek value
	virtual void Rewind() const = 0;

	/// Returns the duration in seconds
	inline float Duration() const { return m_fDuration; }
	/// Returns number of samples
	inline unsigned long int NumSamples() const { return m_ulNumSamples; }
	/// Returns number of bytes per sample
	inline int BytesPerSample() const { return m_iBytesPerSample; }
	/// Returns number of channels
	inline int Channels() const { return m_iChannels; }
	/// Returns samples frequency
	inline int Frequency() const { return m_iFrequency; }

	/// Returns the decoded buffer size in bytes
	inline int BufferSize() const { return m_ulNumSamples * m_iChannels * m_iBytesPerSample; }

	// Returns the proper audio loader according to the file extension
	static ncIAudioLoader* CreateFromFile(const char *pFilename);
};

#endif
