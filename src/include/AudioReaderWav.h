#ifndef CLASS_NCINE_AUDIOREADERWAV
#define CLASS_NCINE_AUDIOREADERWAV

#include <nctl/UniquePtr.h>
#include "IAudioReader.h"

namespace ncine {

class IFile;

/// WAVE audio reader
class AudioReaderWav : public IAudioReader
{
  public:
	AudioReaderWav(nctl::UniquePtr<IFile> fileHandle);

	unsigned long int read(void *buffer, unsigned long int bufferSize) const override;
	void rewind() const override;

  private:
	/// Audio file handle
	nctl::UniquePtr<IFile> fileHandle_;

	/// Deleted copy constructor
	AudioReaderWav(const AudioReaderWav &) = delete;
	/// Deleted assignment operator
	AudioReaderWav &operator=(const AudioReaderWav &) = delete;
};

}

#endif
