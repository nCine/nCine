#include <cstdlib> // for exit()
#include "AudioBuffer.h"
#include "IAudioLoader.h"
#include "ServiceLocator.h"
#include "Array.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioBuffer::AudioBuffer()
	: numChannels_(0), frequency_(0)
{
	type_ = AUDIOBUFFER_TYPE;
	alGenBuffers(1, &bufferId_);
}

/// A constructor creating a buffer from a file
AudioBuffer::AudioBuffer(const char *filename)
	: numChannels_(0), frequency_(0)
{
	type_ = AUDIOBUFFER_TYPE;
	alGenBuffers(1, &bufferId_);
	setName(filename);

	IAudioLoader* audioLoader = IAudioLoader::createFromFile(filename);
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

/// Loads audio samples based on information from the audio loader
void AudioBuffer::load(const IAudioLoader *audioLoader)
{
	char *buffer;
	ALenum format;
	frequency_ = audioLoader->frequency();
	numChannels_ = audioLoader->numChannels();

	if (numChannels_ == 1)
	{
		format = AL_FORMAT_MONO16;
	}
	else if (numChannels_ == 2)
	{
		format = AL_FORMAT_STEREO16;
	}
	else
	{
		LOGF_X("Unsupported number of channels: %d", numChannels_);
		exit(EXIT_FAILURE);
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
