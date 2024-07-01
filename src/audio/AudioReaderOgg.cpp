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

const char *vorbisErrorToString(long errorCode)
{
	switch (errorCode)
	{
		case OV_HOLE: return "OV_HOLE";
		case OV_EBADLINK: return "OV_EBADLINK";
		case OV_EINVAL: return "OV_EINVAL";
		default: return "unknown";
	}
}

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
			LOGW_X("Error decoding buffer at %u bytes in bitstream %d (%s)", bufferSeek, bitStream, vorbisErrorToString(bytes));

		// Reset the static variable at the end of a decoding process
		if (bytes <= 0)
			bitStream = 0;
		else
			bufferSeek += bytes;
	} while ((bytes > 0 || bytes == OV_HOLE) && bufferSize - bufferSeek > 0); // In case of a dropout in audio (OV_HOLE), decoding continues.

	return bufferSeek;
}

void AudioReaderOgg::rewind() const
{
	ov_raw_seek(&oggFile_, 0);
}

}
