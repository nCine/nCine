#include "return_macros.h"
#include "AudioLoaderWav.h"
#include "AudioReaderWav.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioLoaderWav::AudioLoaderWav(nctl::UniquePtr<IFile> fileHandle)
    : IAudioLoader(nctl::move(fileHandle))
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());
	fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	RETURN_ASSERT_MSG_X(fileHandle_->isOpened(), "File \"%s\" cannot be opened", fileHandle_->filename());

	WavHeader header;
	fileHandle_->read(&header, sizeof(WavHeader));

	if (strncmp(header.chunkId, "RIFF", 4) != 0 || strncmp(header.format, "WAVE", 4) != 0)
		RETURN_MSG_X("\"%s\" is not a WAV file", fileHandle_->filename());
	if (strncmp(header.subchunk1Id, "fmt ", 4) != 0)
		RETURN_MSG_X("\"%s\" is an invalid WAV file", fileHandle_->filename());

	if (IFile::int16FromLE(header.audioFormat) != 1)
		RETURN_MSG_X("Data in \"%s\" is not in PCM format", fileHandle_->filename());

	bytesPerSample_ = IFile::int16FromLE(header.bitsPerSample) / 8;
	numChannels_ = IFile::int16FromLE(header.numChannels);
	frequency_ = IFile::int32FromLE(header.sampleRate);

	numSamples_ = IFile::int32FromLE(header.subchunk2Size) / (numChannels_ * bytesPerSample_);
	duration_ = float(numSamples_) / frequency_;

	RETURN_ASSERT_MSG_X(numChannels_ == 1 || numChannels_ == 2, "Unsupported number of channels: %d", numChannels_);
	LOGI_X("duration: %.2fs, channels: %d, frequency: %dHz", duration_, numChannels_, frequency_);

	hasLoaded_ = true;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

nctl::UniquePtr<IAudioReader> AudioLoaderWav::createReader()
{
	return nctl::makeUnique<AudioReaderWav>(nctl::move(fileHandle_));
}

}
