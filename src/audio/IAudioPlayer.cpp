#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "IAudioPlayer.h"
#include "Vector3.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IAudioPlayer::IAudioPlayer(ObjectType type, const char *name)
    : Object(type, name), sourceId_(IAudioDevice::UnavailableSource),
      state_(PlayerState::STOPPED), isLooping_(false),
      gain_(1.0f), pitch_(1.0f), position_(0.0f, 0.0f, 0.0f)
{
}

IAudioPlayer::IAudioPlayer(ObjectType type)
    : Object(type), sourceId_(IAudioDevice::UnavailableSource),
      state_(PlayerState::STOPPED), isLooping_(false),
      gain_(1.0f), pitch_(1.0f), position_(0.0f, 0.0f, 0.0f)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

int IAudioPlayer::sampleOffset() const
{
	int byteOffset = 0;
	alGetSourcei(sourceId_, AL_SAMPLE_OFFSET, &byteOffset);
	return byteOffset;
}

void IAudioPlayer::setSampleOffset(int byteOffset)
{
	alSourcei(sourceId_, AL_SAMPLE_OFFSET, byteOffset);
}

/*! The change is applied to the OpenAL source only when playing. */
void IAudioPlayer::setGain(float gain)
{
	gain_ = gain;
	if (state_ == PlayerState::PLAYING)
		alSourcef(sourceId_, AL_GAIN, gain_);
}

/*! The change is applied to the OpenAL source only when playing. */
void IAudioPlayer::setPitch(float pitch)
{
	pitch_ = pitch;
	if (state_ == PlayerState::PLAYING)
		alSourcef(sourceId_, AL_PITCH, pitch_);
}

/*! The change is applied to the OpenAL source only when playing. */
void IAudioPlayer::setPosition(const Vector3f &position)
{
	position_ = position;
	if (state_ == PlayerState::PLAYING)
		alSourcefv(sourceId_, AL_POSITION, position_.data());
}

/*! The change is applied to the OpenAL source only when playing. */
void IAudioPlayer::setPosition(float x, float y, float z)
{
	position_.set(x, y, z);
	if (state_ == PlayerState::PLAYING)
		alSourcefv(sourceId_, AL_POSITION, position_.data());
}

}
