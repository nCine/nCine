#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "common_macros.h"
#include "AudioBuffer.h"
#include "IAudioLoader.h"
#include "Array.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioBuffer::AudioBuffer()
	: Object(AUDIOBUFFER_TYPE), numChannels_(0), frequency_(0)
{
	alGenBuffers(1, &bufferId_);
}

AudioBuffer::AudioBuffer(const char *filename)
	: Object(AUDIOBUFFER_TYPE, filename), numChannels_(0), frequency_(0)
{
	alGenBuffers(1, &bufferId_);

	IAudioLoader *audioLoader = IAudioLoader::createFromFile(filename);
	load(audioLoader);
	delete audioLoader;
}

AudioBuffer::~AudioBuffer()
{
	alDeleteBuffers(1, &bufferId_);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AudioBuffer::load(const IAudioLoader *audioLoader)
{
	char *buffer;
	ALenum format;
	frequency_ = audioLoader->frequency();
	numChannels_ = audioLoader->numChannels();

	FATAL_ASSERT_MSG_X(numChannels_ == 1 || numChannels_ == 2, "Unsupported number of channels: %d", numChannels_);
	if (numChannels_ == 1)
	{
		format = AL_FORMAT_MONO16;
	}
	else
	{
		format = AL_FORMAT_STEREO16;
	}

	// Buffer size calculated as samples * channels * 16bit
	unsigned long int bufferSize = audioLoader->bufferSize();
	buffer = new char[bufferSize];

	audioLoader->read(buffer, bufferSize);
	// On iOS alBufferDataStatic could be used instead
	alBufferData(bufferId_, format, buffer, bufferSize, frequency_);

	delete[] buffer;
}

}
