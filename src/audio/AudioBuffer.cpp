#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "common_macros.h"
#include "AudioBuffer.h"
#include "IAudioLoader.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioBuffer::AudioBuffer()
    : Object(ObjectType::AUDIOBUFFER),
      numChannels_(0), frequency_(0), bufferSize_(0)
{
	alGetError();
	alGenBuffers(1, &bufferId_);
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alGenBuffers failed: %x", error);
}

AudioBuffer::AudioBuffer(const char *filename)
    : Object(ObjectType::AUDIOBUFFER, filename),
      numChannels_(0), frequency_(0), bufferSize_(0)
{
	ZoneScoped;
	ZoneText(filename, strnlen(filename, 256));

	alGetError();
	alGenBuffers(1, &bufferId_);
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alGenBuffers failed: %x", error);

	nctl::UniquePtr<IAudioLoader> audioLoader = IAudioLoader::createFromFile(filename);
	load(audioLoader.get());
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
	ASSERT(audioLoader);

	frequency_ = audioLoader->frequency();
	numChannels_ = audioLoader->numChannels();

	FATAL_ASSERT_MSG_X(numChannels_ == 1 || numChannels_ == 2, "Unsupported number of channels: %d", numChannels_);
	const ALenum format = (numChannels_ == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	// Buffer size calculated as samples * channels * 16bit
	bufferSize_ = audioLoader->bufferSize();
	nctl::UniquePtr<char[]> buffer = nctl::makeUnique<char[]>(bufferSize_);

	audioLoader->read(buffer.get(), bufferSize_);
	// On iOS `alBufferDataStatic()` could be used instead
	alBufferData(bufferId_, format, buffer.get(), bufferSize_, frequency_);
}

}
