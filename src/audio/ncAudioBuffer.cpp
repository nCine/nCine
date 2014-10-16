#include <cstdlib> // for exit()
#include "ncAudioBuffer.h"
#include "ncIAudioLoader.h"
#include "ncServiceLocator.h"
#include "ncArray.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAudioBuffer::ncAudioBuffer()
	: numChannels_(0), frequency_(0)
{
	type_ = AUDIOBUFFER_TYPE;
	alGenBuffers(1, &bufferId_);
}

/// A constructor creating a buffer from a file
ncAudioBuffer::ncAudioBuffer(const char *filename)
	: numChannels_(0), frequency_(0)
{
	type_ = AUDIOBUFFER_TYPE;
	alGenBuffers(1, &bufferId_);
	setName(filename);

	ncIAudioLoader* audioLoader = ncIAudioLoader::createFromFile(filename);
	load(audioLoader);
	delete audioLoader;
}

ncAudioBuffer::~ncAudioBuffer()
{
	alDeleteBuffers(1, &bufferId_);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Loads audio samples based on information from the audio loader
void ncAudioBuffer::load(const ncIAudioLoader *audioLoader)
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
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, "ncAudioBuffer::load - Unsupported number of channels: %d", numChannels_);
		exit(EXIT_FAILURE);
	}

	// Buffer size calculated as samples * channels * 16bit
	int bufferSize = audioLoader->bufferSize();
	buffer = new char[bufferSize];

	audioLoader->read(buffer, bufferSize);
	// On iOS alBufferDataStatic could be used instead
	alBufferData(bufferId_, format, buffer, bufferSize, frequency_);

	delete[] buffer;
}
