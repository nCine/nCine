#ifndef CLASS_NCINE_AUDIOREADEROGG
#define CLASS_NCINE_AUDIOREADEROGG

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

#include <nctl/UniquePtr.h>
#include "IAudioReader.h"

namespace ncine {

class IFile;

/// Ogg Vorbis audio reader
class AudioReaderOgg : public IAudioReader
{
  public:
	AudioReaderOgg(nctl::UniquePtr<IFile> fileHandle, const OggVorbis_File &oggFile);
	~AudioReaderOgg() override;

	unsigned long int read(void *buffer, unsigned long int bufferSize) const override;
	void rewind() const override;

  private:
	/// Audio file handle
	nctl::UniquePtr<IFile> fileHandle_;
	/// Vorbisfile handle
	mutable OggVorbis_File oggFile_;

	/// Deleted copy constructor
	AudioReaderOgg(const AudioReaderOgg &) = delete;
	/// Deleted assignment operator
	AudioReaderOgg &operator=(const AudioReaderOgg &) = delete;
};

}

#endif
