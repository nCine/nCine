#include "AudioReaderOgg.h"
#include "IFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioReaderOgg::AudioReaderOgg(nctl::UniquePtr<IFile> fileHandle, const OggVorbis_File &oggFile)
    : fileHandle_(nctl::move(fileHandle)), oggFile_(oggFile)
{
	ASSERT(fileHandle_->isOpened());
}

AudioReaderOgg::~AudioReaderOgg()
{
	ov_clear(&oggFile_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned long int AudioReaderOgg::read(void *buffer, unsigned long int bufferSize) const
{
	ASSERT(buffer);
	ASSERT(bufferSize > 0);

	static int bitStream = 0;
	long bytes = 0;
	unsigned long int bufferSeek = 0;

	do
	{
		// Read up to a buffer's worth of decoded sound data
		// (0: little endian, 2: 16bit, 1: signed)
		bytes = ov_read(&oggFile_, static_cast<char *>(buffer) + bufferSeek, bufferSize - bufferSeek, 0, 2, 1, &bitStream);

		if (bytes < 0)
		{
			ov_clear(&oggFile_);
			FATAL_MSG_X("Error decoding at bitstream %d", bitStream);
		}

		// Reset the static variable at the end of a decoding process
		if (bytes <= 0)
			bitStream = 0;

		bufferSeek += bytes;
	} while (bytes > 0 && bufferSize - bufferSeek > 0);

	return bufferSeek;
}

void AudioReaderOgg::rewind() const
{
	ov_raw_seek(&oggFile_, 0);
}

}
