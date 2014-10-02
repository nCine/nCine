#ifndef CLASS_NCAUDIOLOADEROGG
#define CLASS_NCAUDIOLOADEROGG

#ifdef __ANDROID__
#include "ivorbisfile.h"
#else
#include <vorbis/vorbisfile.h>
#endif

#include "ncIAudioLoader.h"

/// Ogg Vorbis audio loader
class ncAudioLoaderOgg : public ncIAudioLoader
{
 public:
	ncAudioLoaderOgg(const char *pFilename);
	ncAudioLoaderOgg(ncIFile *pFileHandle);
	~ncAudioLoaderOgg();

	long Read(char *pBuffer, int iBufSize) const;
	void Rewind() const;

 private:
	/// Vorbisfile handle
	mutable OggVorbis_File m_oggFile;

	void Init();
};

#endif
