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

AudioBuffer::AudioBuffer(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
    : Object(ObjectType::AUDIOBUFFER, bufferName),
      bytesPerSample_(0), numChannels_(0), frequency_(0), numSamples_(0), duration_(0.0f)
{
	ZoneScoped;
	ZoneText(bufferName, strnlen(bufferName, nctl::String::MaxCStringLength));

	alGetError();
	alGenBuffers(1, &bufferId_);
	const ALenum error = alGetError();
	FATAL_ASSERT_MSG_X(error == AL_NO_ERROR, "alGenBuffers failed: 0x%x", error);

	nctl::UniquePtr<IAudioLoader> audioLoader = IAudioLoader::createFromMemory(bufferName, bufferPtr, bufferSize);
	load(*audioLoader.get());
}

AudioBuffer::AudioBuffer(const char *filename)
    : Object(ObjectType::AUDIOBUFFER, filename),
      bytesPerSample_(0), numChannels_(0), frequency_(0), numSamples_(0), duration_(0.0f)
{
	ZoneScoped;
	ZoneText(filename, strnlen(filename, nctl::String::MaxCStringLength));

	alGetError();
	alGenBuffers(1, &bufferId_);
	const ALenum error = alGetError();
	FATAL_ASSERT_MSG_X(error == AL_NO_ERROR, "alGenBuffers failed: 0x%x", error);

	nctl::UniquePtr<IAudioLoader> audioLoader = IAudioLoader::createFromFile(filename);
	load(*audioLoader.get());
}

AudioBuffer::~AudioBuffer()
{
	alDeleteBuffers(1, &bufferId_);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AudioBuffer::load(IAudioLoader &audioLoader)
{
	bytesPerSample_ = audioLoader.bytesPerSample();
	numChannels_ = audioLoader.numChannels();
	frequency_ = audioLoader.frequency();

	FATAL_ASSERT_MSG_X(numChannels_ == 1 || numChannels_ == 2, "Unsupported number of channels: %d", numChannels_);
	const ALenum format = (numChannels_ == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	// Buffer size calculated as samples * channels * bytes per samples
	const unsigned long int bufferSize = audioLoader.bufferSize();
	nctl::UniquePtr<char[]> buffer = nctl::makeUnique<char[]>(bufferSize);

	nctl::UniquePtr<IAudioReader> audioReader = audioLoader.createReader();
	audioReader->read(buffer.get(), bufferSize);

	alGetError();
	// On iOS `alBufferDataStatic()` could be used instead
	alBufferData(bufferId_, format, buffer.get(), bufferSize, frequency_);
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alBufferData failed: 0x%x", error);
}

}
