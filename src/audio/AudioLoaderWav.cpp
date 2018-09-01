#include <cstring>
#include "common_macros.h"
#include "AudioLoaderWav.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioLoaderWav::AudioLoaderWav(const char *filename)
	: AudioLoaderWav(IFile::createFileHandle(filename))
{

}

AudioLoaderWav::AudioLoaderWav(nctl::UniquePtr<IFile> fileHandle)
	: IAudioLoader(nctl::move(fileHandle))
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());
	fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);

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

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned long int AudioLoaderWav::read(char *buffer, unsigned long int bufferSize) const
{
	ASSERT(buffer);
	ASSERT(bufferSize > 0);

	long bytes = 0;
	long int bufferSeek = 0;

	do
	{
		// Read up to a buffer's worth of decoded sound data
		bytes = fileHandle_->read(buffer, bufferSize);
		FATAL_ASSERT_MSG(bytes > 0, "Zero bytes read from file");
		bufferSeek += bytes;
	}
	while (bytes > 0 && bufferSize - bufferSeek > 0);

	return bufferSeek;
}

void AudioLoaderWav::rewind() const
{
	clearerr(fileHandle_->ptr());
	fileHandle_->seek(sizeof(WavHeader), SEEK_SET);
}

}
