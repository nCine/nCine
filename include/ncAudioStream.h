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

	/// Samples frequency
	int m_iFrequency;
	/// OpenAL channel format enumeration
	ALenum m_eFormat;
	/// The associated loader to continuosly stream decoded data
	ncIAudioLoader *m_pAudioLoader;

	// Constructor creating an audio stream from an audio file
	ncAudioStream(const char *pFilename);
public:
	virtual ~ncAudioStream();

	// Enqueues new buffers and unqueues processed ones
	bool Enqueue(ALuint uSource, bool bLooping);
	// Unqueues any left buffer and rewinds the loader
	void Stop(ALuint uSource);

	/// Returns samples frequency
	inline int Frequency() const { return m_iFrequency; }

	friend class ncAudioStreamPlayer;
};

#endif
