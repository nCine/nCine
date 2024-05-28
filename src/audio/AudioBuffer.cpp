#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "return_macros.h"
#include <nctl/CString.h>
#include "AudioBuffer.h"
#include "IAudioLoader.h"
#include "tracy.h"

namespace ncine {

namespace {

	ALenum alFormat(int bytesPerSample, int numChannels)
	{
		ALenum format = AL_FORMAT_MONO8;
		if (bytesPerSample == 1 && numChannels == 2)
			format = AL_FORMAT_STEREO8;
		else if (bytesPerSample == 2 && numChannels == 1)
			format = AL_FORMAT_MONO16;
		else if (bytesPerSample == 2 && numChannels == 2)
			format = AL_FORMAT_STEREO16;

		return format;
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioBuffer::AudioBuffer()
    : Object(ObjectType::AUDIOBUFFER), bufferId_(0),
      bytesPerSample_(0), numChannels_(0), frequency_(0), numSamples_(0), duration_(0.0f)
{
	alGetError();
	alGenBuffers(1, &bufferId_);
	const ALenum error = alGetError();
	FATAL_ASSERT_MSG_X(error == AL_NO_ERROR, "alGenBuffers failed: 0x%x", error);

	ASSERT(alIsBuffer(bufferId_) == AL_TRUE);
}

AudioBuffer::AudioBuffer(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
    : AudioBuffer()
{
	const bool hasLoaded = loadFromMemory(bufferName, bufferPtr, bufferSize);
	if (hasLoaded == false)
		LOGE_X("Audio buffer \"%s\" cannot be loaded", bufferName);
}

AudioBuffer::AudioBuffer(const char *filename)
    : AudioBuffer()
{
	const bool hasLoaded = loadFromFile(filename);
	if (hasLoaded == false)
		LOGE_X("Audio file \"%s\" cannot be loaded", filename);
}

AudioBuffer::~AudioBuffer()
{
	// Moved out objects have their buffer id set to zero
	alDeleteBuffers(1, &bufferId_);
}

AudioBuffer::AudioBuffer(AudioBuffer &&other)
    : Object(nctl::move(other)), bufferId_(other.bufferId_),
      bytesPerSample_(other.bytesPerSample_), numChannels_(other.numChannels_),
      frequency_(other.frequency_), numSamples_(other.numSamples_), duration_(other.duration_)
{
	other.bufferId_ = 0;
}

AudioBuffer &AudioBuffer::operator=(AudioBuffer &&other)
{
	Object::operator=(nctl::move(other));

	bufferId_ = other.bufferId_;
	bytesPerSample_ = other.bytesPerSample_;
	numChannels_ = other.numChannels_;
	frequency_ = other.frequency_;
	numSamples_ = other.numSamples_;
	duration_ = other.duration_;

	other.bufferId_ = 0;
	return *this;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AudioBuffer::init(const char *name, Format format, int frequency)
{
	ZoneScoped;
	if (name)
	{
		// When Tracy is disabled the statement body is empty and braces are needed
		ZoneText(name, nctl::strnlen(name, nctl::String::MaxCStringLength));
	}

	setName(name);

	switch (format)
	{
		case Format::MONO8:
			bytesPerSample_ = 1;
			numChannels_ = 1;
			break;
		case Format::STEREO8:
			bytesPerSample_ = 1;
			numChannels_ = 2;
			break;
		case Format::MONO16:
			bytesPerSample_ = 2;
			numChannels_ = 1;
			break;
		case Format::STEREO16:
			bytesPerSample_ = 2;
			numChannels_ = 2;
			break;
	}
	frequency_ = frequency;

	loadFromSamples(nullptr, 0);
}

bool AudioBuffer::loadFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
{
	ZoneScoped;
	if (bufferName)
	{
		// When Tracy is disabled the statement body is empty and braces are needed
		ZoneText(bufferName, nctl::strnlen(bufferName, nctl::String::MaxCStringLength));
	}

	nctl::UniquePtr<IAudioLoader> audioLoader = IAudioLoader::createFromMemory(bufferName, bufferPtr, bufferSize);
	if (audioLoader->hasLoaded() == false)
		return false;

	const bool samplesHaveLoaded = load(*audioLoader.get());
	if (samplesHaveLoaded == false)
		return false;

	setName(bufferName);
	return true;
}

bool AudioBuffer::loadFromFile(const char *filename)
{
	ZoneScoped;
	ZoneText(filename, nctl::strnlen(filename, nctl::String::MaxCStringLength));

	nctl::UniquePtr<IAudioLoader> audioLoader = IAudioLoader::createFromFile(filename);
	if (audioLoader->hasLoaded() == false)
		return false;

	const bool samplesHaveLoaded = load(*audioLoader.get());
	if (samplesHaveLoaded == false)
		return false;

	setName(filename);
	return true;
}

bool AudioBuffer::loadFromSamples(const unsigned char *bufferPtr, unsigned long int bufferSize)
{
	if (bytesPerSample_ == 0 || numChannels_ == 0 || frequency_ == 0)
		return false;

	if (bufferSize % (bytesPerSample_ * numChannels_) != 0)
		LOGW("Buffer size is incompatible with format");
	const ALenum format = alFormat(bytesPerSample_, numChannels_);

	alGetError();
	// On iOS `alBufferDataStatic()` could be used instead
	alBufferData(bufferId_, format, bufferPtr, bufferSize, frequency_);
	const ALenum error = alGetError();
	RETURNF_ASSERT_MSG_X(error == AL_NO_ERROR, "alBufferData failed: 0x%x", error);

	numSamples_ = bufferSize / (numChannels_ * bytesPerSample_);
	duration_ = float(numSamples_) / frequency_;

	return (error == AL_NO_ERROR);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool AudioBuffer::load(IAudioLoader &audioLoader)
{
	RETURNF_ASSERT_MSG_X(audioLoader.bytesPerSample() == 1 || audioLoader.bytesPerSample() == 2,
	                     "Unsupported number of bytes per sample: %d", audioLoader.bytesPerSample());
	RETURNF_ASSERT_MSG_X(audioLoader.numChannels() == 1 || audioLoader.numChannels() == 2,
	                     "Unsupported number of channels: %d", audioLoader.numChannels());

	bytesPerSample_ = audioLoader.bytesPerSample();
	numChannels_ = audioLoader.numChannels();
	frequency_ = audioLoader.frequency();

	// Buffer size calculated as samples * channels * bytes per samples
	const unsigned long int bufferSize = audioLoader.bufferSize();
	nctl::UniquePtr<unsigned char[]> buffer = nctl::makeUnique<unsigned char[]>(bufferSize);

	nctl::UniquePtr<IAudioReader> audioReader = audioLoader.createReader();
	audioReader->read(buffer.get(), bufferSize);

	return loadFromSamples(buffer.get(), bufferSize);
}

}
