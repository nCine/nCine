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
	ncAudioLoaderOgg(const char *filename);
	ncAudioLoaderOgg(ncIFile *fileHandle);
	~ncAudioLoaderOgg();

	long read(char *buffer, int bufferSize) const;
	void rewind() const;

  private:
	/// Vorbisfile handle
	mutable OggVorbis_File oggFile_;

	void init();
};

#endif
