#ifndef CLASS_NCAUDIOLOADEROGG
#define CLASS_NCAUDIOLOADEROGG

#ifdef __ANDROID__
#include "ivorbisfile.h"
#else
#include <vorbis/vorbisfile.h>
#endif

#include "ncIAudioLoader.h"

/// Audio loader class
class ncAudioLoaderOgg : public ncIAudioLoader
{
private:
	/// Vorbisfile handle
	mutable OggVorbis_File m_oggFile;

	void Init();
public:
	ncAudioLoaderOgg(const char *pFilename);
	~ncAudioLoaderOgg();

	long Read(char *pBuffer, int iBufSize) const;
	void Rewind() const;
};

#endif
