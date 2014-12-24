#ifndef CLASS_NCINE_AUDIOLOADEROGG
#define CLASS_NCINE_AUDIOLOADEROGG

#ifdef __ANDROID__
#include "ivorbisfile.h"
#else
#include <vorbis/vorbisfile.h>
#endif

#include "IAudioLoader.h"

namespace ncine {

/// Ogg Vorbis audio loader
class AudioLoaderOgg : public IAudioLoader
{
  public:
	AudioLoaderOgg(const char *filename);
	AudioLoaderOgg(IFile *fileHandle);
	~AudioLoaderOgg();

	long read(char *buffer, int bufferSize) const;
	void rewind() const;

  private:
	/// Vorbisfile handle
	mutable OggVorbis_File oggFile_;

	/// Private copy constructor
	AudioLoaderOgg(const AudioLoaderOgg&);
	/// Private assignment operator
	AudioLoaderOgg& operator=(const AudioLoaderOgg&);

	void init();
};

}

#endif
