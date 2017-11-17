#include <cstring>
#include <stdint.h> // for WAV header
#include "common_macros.h"
#include "AudioLoaderWav.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioLoaderWav::AudioLoaderWav(const char *filename)
	: IAudioLoader(filename)
{
	init();
}

AudioLoaderWav::AudioLoaderWav(IFile *fileHandle)
	: IAudioLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned long int AudioLoaderWav::read(char *buffer, unsigned long int bufferSize) const
{
	ASSERT(buffer);
	ASSERT(bufferSize > 0);

	long bytes_;
	long int bufferSeek = 0;

	do
	{
		// Read up to a buffer's worth of decoded sound data
		bytes_ = fileHandle_->read(buffer, bufferSize);
		FATAL_ASSERT_MSG(bytes_ > 0, "Zero bytes read from file");
		const int err = ferror(fileHandle_->ptr());
		FATAL_ASSERT_MSG(err == 0, "Error reading the file");

		bufferSeek += bytes_;
	}
	while (bytes_ > 0 && bufferSize - bufferSeek > 0);

	return bufferSeek;
}

void AudioLoaderWav::rewind() const
{
	clearerr(fileHandle_->ptr());
	fileHandle_->seek(sizeof(WavHeader), SEEK_SET);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AudioLoaderWav::init()
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());
	fileHandle_->open(IFile::MODE_READ | IFile::MODE_BINARY);

	WavHeader header;
	fileHandle_->read(&header, sizeof(WavHeader));

	if (strncmp(header.chunkId, "RIFF", 4) != 0)
		FATAL_MSG_X("\"%s\" is not a WAV file", fileHandle_->filename());
	if (strncmp(header.format, "WAVE", 4) != 0)
		FATAL_MSG_X("\"%s\" is not a WAV file", fileHandle_->filename());
	if (strncmp(header.subchunk1Id, "fmt ", 4) != 0)
		FATAL_MSG_X("\"%s\" is an invalid WAV file", fileHandle_->filename());

	if (IFile::int16FromLE(header.audioFormat) != 1)
		FATAL_MSG_X("Data in \"%s\" is not in PCM format", fileHandle_->filename());

	bytesPerSample_ = IFile::int16FromLE(header.bitsPerSample) / 8;
	numChannels_ = IFile::int16FromLE(header.numChannels);
	frequency_ = IFile::int32FromLE(header.sampleRate);

	numSamples_ = IFile::int32FromLE(header.subchunk2Size) / (numChannels_ * bytesPerSample_);
	duration_ = float(numSamples_) / frequency_;

	LOGI_X("duration: %.2f, channels: %d, frequency: %d", duration_, numChannels_, frequency_);
}

}
