#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "common_macros.h"
#include <nctl/CString.h>
#include "AudioStream.h"
#include "IAudioLoader.h"
#include "IAudioReader.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! Private constructor called only by `AudioStreamPlayer`. */
AudioStream::AudioStream()
    : buffersIds_(nctl::StaticArrayMode::EXTEND_SIZE), nextAvailableBufferIndex_(0),
      currentBufferId_(0), bytesPerSample_(0), numChannels_(0),
      frequency_(0), numSamples_(0), duration_(0.0f)
{
	alGetError();
	alGenBuffers(NumBuffers, buffersIds_.data());
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alGenBuffers failed: 0x%x", error);
	memBuffer_ = nctl::makeUnique<char[]>(BufferSize);
}

/*! Private constructor called only by `AudioStreamPlayer`. */
AudioStream::AudioStream(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
    : AudioStream()
{
	const bool hasLoaded = loadFromMemory(bufferName, bufferPtr, bufferSize);
	if (hasLoaded == false)
		LOGE_X("Audio buffer \"%s\" cannot be loaded", bufferName);
}

/*! Private constructor called only by `AudioStreamPlayer`. */
AudioStream::AudioStream(const char *filename)
    : AudioStream()
{
	const bool hasLoaded = loadFromFile(filename);
	if (hasLoaded == false)
		LOGE_X("Audio file \"%s\" cannot be loaded", filename);
}

AudioStream::~AudioStream()
{
	alDeleteBuffers(NumBuffers, buffersIds_.data());
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned long int AudioStream::numStreamSamples() const
{
	if (numChannels_ * bytesPerSample_ > 0)
		return BufferSize / (numChannels_ * bytesPerSample_);
	return 0UL;
}

/*! \return A flag indicating whether the stream has been entirely decoded and played or not. */
bool AudioStream::enqueue(unsigned int source, bool looping)
{
	if (audioReader_ == nullptr)
		return false;

	// Set to false when the queue is empty and there is no more data to decode
	bool shouldKeepPlaying = true;

	ALint numProcessedBuffers;
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &numProcessedBuffers);

	// Unqueueing
	while (numProcessedBuffers > 0)
	{
		ALuint unqueuedAlBuffer;
		alSourceUnqueueBuffers(source, 1, &unqueuedAlBuffer);
		nextAvailableBufferIndex_--;
		buffersIds_[nextAvailableBufferIndex_] = unqueuedAlBuffer;
		numProcessedBuffers--;
	}

	// Queueing
	if (nextAvailableBufferIndex_ < NumBuffers)
	{
		currentBufferId_ = buffersIds_[nextAvailableBufferIndex_];

		unsigned long bytes = audioReader_->read(memBuffer_.get(), BufferSize);

		// EOF reached
		if (bytes < BufferSize)
		{
			if (looping)
			{
				audioReader_->rewind();
				const unsigned long moreBytes = audioReader_->read(memBuffer_.get() + bytes, BufferSize - bytes);
				bytes += moreBytes;
			}
		}

		// If it is still decoding data then enqueue
		if (bytes > 0)
		{
			// On iOS `alBufferDataStatic()` could be used instead
			alBufferData(currentBufferId_, format_, memBuffer_.get(), bytes, frequency_);
			alSourceQueueBuffers(source, 1, &currentBufferId_);
			nextAvailableBufferIndex_++;
		}
		// If there is no more data left to decode and the queue is empty
		else if (nextAvailableBufferIndex_ == 0)
		{
			shouldKeepPlaying = false;
			stop(source);
		}
	}

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);

	// Handle buffer underrun case
	if (state != AL_PLAYING)
	{
		ALint numQueuedBuffers = 0;
		alGetSourcei(source, AL_BUFFERS_QUEUED, &numQueuedBuffers);
		if (numQueuedBuffers > 0)
		{
			// Need to restart play
			alSourcePlay(source);
		}
	}

	return shouldKeepPlaying;
}

void AudioStream::stop(unsigned int source)
{
	// In order to unqueue all the buffers, the source must be stopped first
	alSourceStop(source);

	ALint numProcessedBuffers;
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &numProcessedBuffers);

	// Unqueueing
	while (numProcessedBuffers > 0)
	{
		ALuint unqueuedAlBuffer;
		alSourceUnqueueBuffers(source, 1, &unqueuedAlBuffer);
		nextAvailableBufferIndex_--;
		buffersIds_[nextAvailableBufferIndex_] = unqueuedAlBuffer;
		numProcessedBuffers--;
	}

	audioReader_->rewind();
	currentBufferId_ = 0;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool AudioStream::loadFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
{
	ZoneScoped;
	ZoneText(bufferName, nctl::strnlen(bufferName, nctl::String::MaxCStringLength));

	nctl::UniquePtr<IAudioLoader> audioLoader = IAudioLoader::createFromMemory(bufferName, bufferPtr, bufferSize);
	if (audioLoader->hasLoaded() == false)
		return false;

	createReader(*audioLoader);
	return true;
}

bool AudioStream::loadFromFile(const char *filename)
{
	ZoneScoped;
	ZoneText(filename, nctl::strnlen(filename, nctl::String::MaxCStringLength));

	nctl::UniquePtr<IAudioLoader> audioLoader = IAudioLoader::createFromFile(filename);
	if (audioLoader->hasLoaded() == false)
		return false;

	createReader(*audioLoader);
	return true;
}

void AudioStream::createReader(IAudioLoader &audioLoader)
{
	bytesPerSample_ = audioLoader.bytesPerSample();
	numChannels_ = audioLoader.numChannels();
	frequency_ = audioLoader.frequency();
	numSamples_ = audioLoader.numSamples();
	duration_ = float(numSamples_) / frequency_;
	format_ = (numChannels_ == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	audioReader_ = audioLoader.createReader();
}

}
