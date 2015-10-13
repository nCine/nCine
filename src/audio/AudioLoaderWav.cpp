#include <cstdlib> // for exit()
#include <cstring>
#include <stdint.h> // for WAV header
#include "AudioLoaderWav.h"
#include "ServiceLocator.h"

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
	long bytes_;
	long int bufferSeek = 0;

	do
	{
		// Read up to a buffer's worth of decoded sound data
		bytes_ = fileHandle_->read(buffer, bufferSize);

		if (bytes_ == 0 && ferror(fileHandle_->ptr()))
		{
			LOGF("Error reading the file");
			exit(EXIT_FAILURE);
		}

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
	{
		LOGF_X("\"%s\" is not a WAV file", fileHandle_->filename());
		exit(EXIT_FAILURE);
	}
	if (strncmp(header.format, "WAVE", 4) != 0)
	{
		LOGF_X("\"%s\" is not a WAV file", fileHandle_->filename());
		exit(EXIT_FAILURE);
	}
	if (strncmp(header.subchunk1Id, "fmt ", 4) != 0)
	{
		LOGF_X("\"%s\" is an invalid WAV file", fileHandle_->filename());
		exit(EXIT_FAILURE);
	}

	if (IFile::int16FromLE(header.audioFormat) != 1)
	{
		LOGF_X("Data in \"%s\" is not in PCM format", fileHandle_->filename());
		exit(EXIT_FAILURE);
	}

	bytesPerSample_ = IFile::int16FromLE(header.bitsPerSample) / 8;
	numChannels_ = IFile::int16FromLE(header.numChannels);
	frequency_ = IFile::int32FromLE(header.sampleRate);

	numSamples_ = IFile::int32FromLE(header.subchunk2Size) / (numChannels_ * bytesPerSample_);
	duration_ = float(numSamples_) / frequency_;

	LOGI_X("duration: %.2f, channels: %d, frequency: %d", duration_, numChannels_, frequency_);
}

}
