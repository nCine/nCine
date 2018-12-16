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
	: nextAvailALBuffer_(0), frequency_(0)
{
	ZoneScoped;
	ZoneText(filename, strnlen(filename, 256));
	alGenBuffers(NumBuffers, alBuffers_.data());
	memBuffer_ = nctl::makeUnique<char []>(BufferSize);

	audioLoader_ = IAudioLoader::createFromFile(filename);
	frequency_ = audioLoader_->frequency();
	const int numChannels = audioLoader_->numChannels();

	FATAL_ASSERT_MSG_X(numChannels == 1 || numChannels == 2, "Unsupported number of channels: %d", numChannels);
	if (numChannels == 1)
		format_ = AL_FORMAT_MONO16;
	else
		format_ = AL_FORMAT_STEREO16;
}

AudioStream::~AudioStream()
{
	alDeleteBuffers(NumBuffers, alBuffers_.data());
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
		nextAvailALBuffer_--;
		alBuffers_[nextAvailALBuffer_] = unqueuedAlBuffer;
		numProcessedBuffers--;
	}


	// Queueing
	if (nextAvailALBuffer_ < NumBuffers)
	{
		ALuint currentBuffer = alBuffers_[nextAvailALBuffer_];

		long bytes = audioLoader_->read(memBuffer_.get(), BufferSize);

		// EOF reached
		if (bytes < BufferSize)
		{
			if (looping)
			{
				audioLoader_->rewind();
				long moreBytes = audioLoader_->read(memBuffer_.get() + bytes, BufferSize - bytes);
				bytes += moreBytes;
			}
		}

		// If still decoding data then enqueue
		if (bytes > 0)
		{
			// On iOS alBufferDataStatic could be used instead
			alBufferData(currentBuffer, format_, memBuffer_.get(), bytes, frequency_);
			alSourceQueueBuffers(source, 1, &currentBuffer);
			nextAvailALBuffer_++;
		}
		// If it has no more data to decode and the queue is empty
		else if (nextAvailALBuffer_ == 0)
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
		ALuint uuqueuedAlBuffer;
		alSourceUnqueueBuffers(source, 1, &uuqueuedAlBuffer);
		nextAvailALBuffer_--;
		alBuffers_[nextAvailALBuffer_] = uuqueuedAlBuffer;
		numProcessedBuffers--;
	}

	audioLoader_->rewind();
}

}
