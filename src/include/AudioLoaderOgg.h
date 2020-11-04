#ifndef CLASS_NCINE_AUDIOLOADEROGG
#define CLASS_NCINE_AUDIOLOADEROGG

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

#include "IAudioLoader.h"

namespace ncine {

/// Ogg Vorbis audio loader
class AudioLoaderOgg : public IAudioLoader
{
  public:
	explicit AudioLoaderOgg(nctl::UniquePtr<IFile> fileHandle);
	~AudioLoaderOgg() override;

	nctl::UniquePtr<IAudioReader> createReader() override;

  private:
	/// Vorbisfile handle
	OggVorbis_File oggFile_;

	/// Deleted copy constructor
	AudioLoaderOgg(const AudioLoaderOgg &) = delete;
	/// Deleted assignment operator
	AudioLoaderOgg &operator=(const AudioLoaderOgg &) = delete;
};

}

#endif
