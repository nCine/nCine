#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "common_macros.h"
#include "AudioStream.h"
#include "IAudioLoader.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! Private constructor called only by `AudioStreamPlayer`. */
AudioStream::AudioStream(const char *filename)
    : nextAvailableBufferIndex_(0), currentBufferId_(0), frequency_(0)
{
	ZoneScoped;
	ZoneText(filename, strnlen(filename, nctl::String::MaxCStringLength));

	alGetError();
	alGenBuffers(NumBuffers, buffersIds_.data());
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alGenBuffers failed: %x", error);
	memBuffer_ = nctl::makeUnique<char[]>(BufferSize);

	audioLoader_ = IAudioLoader::createFromFile(filename);
	numChannels_ = audioLoader_->numChannels();
	frequency_ = audioLoader_->frequency();

	FATAL_ASSERT_MSG_X(numChannels_ == 1 || numChannels_ == 2, "Unsupported number of channels: %d", numChannels_);
	format_ = (numChannels_ == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
}

AudioStream::~AudioStream()
{
	alDeleteBuffers(NumBuffers, buffersIds_.data());
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! \return A flag indicating whether the stream has been entirely decoded and played or not. */
bool AudioStream::enqueue(unsigned int source, bool looping)
{
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

		unsigned long bytes = audioLoader_->read(memBuffer_.get(), BufferSize);

		// EOF reached
		if (bytes < BufferSize)
		{
			if (looping)
			{
				audioLoader_->rewind();
				const unsigned long moreBytes = audioLoader_->read(memBuffer_.get() + bytes, BufferSize - bytes);
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

	audioLoader_->rewind();
	currentBufferId_ = 0;
}

}
