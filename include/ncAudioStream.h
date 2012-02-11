#ifndef CLASS_NCAUDIOSTREAM
#define CLASS_NCAUDIOSTREAM

#include <AL/al.h>
#include "ncAudioLoader.h"

/// Audio stream class
class ncAudioStream
{
private:
	/// Number of buffers for streaming
	static const int s_iNumBuffers = 3;
	/// OpenAL buffer queue for streaming
	ALuint m_uALBuffers[s_iNumBuffers];
	/// Index of the next available OpenAL buffer
	int m_iNextAvailALBuffer;

	/// Size in bytes of each streaming buffer
	static const int s_iBufSize = 16 * 1024;
	/// Memory buffer to feed OpenAL ones
	char *m_pBuffer;

	/// Number of channels
	int m_iChannels;
	/// Samples frequency
	int m_iFrequency;
	/// OpenAL format enumaration
	ALenum m_eFormat;
	/// The associated loader to continuosly stream decoded data
	ncAudioLoader m_audioLoader;

	// Initializes the class with information from the audio loader
	void InitLoader();

	ncAudioStream();
	// Constructor creating an audio stream from an audio file
	ncAudioStream(const char *pFilename);
public:
	virtual ~ncAudioStream();

	// Enqueues new buffers and unqueues processed ones
	bool Enqueue(ALuint uSource, bool bLooping);
	// Unqueues any left buffer and rewinds the loader
	void Stop(ALuint uSource);

	/// Returns number of audio channels
	inline int Channels() const { return m_iChannels; }
	/// Returns samples frequency
	inline int Frequency() const { return m_iFrequency; }

	friend class ncAudioStreamPlayer;
};

#endif
