#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "IAudioPlayer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IAudioPlayer::IAudioPlayer()
	: Object(ObjectType::BASE), sourceId_(-1), state_(PlayerState::STOPPED),
	  isLooping_(false), gain_(1.0f), pitch_(1.0f)
{
	position_[0] = 0.0f;
	position_[1] = 0.0f;
	position_[2] = 0.0f;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

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
void IAudioPlayer::setPosition(float position[3])
{
	setPosition(position[0], position[1], position[2]);
}

/*! The change is applied to the OpenAL source only when playing. */
void IAudioPlayer::setPosition(float x, float y, float z)
{
	position_[0] = x;
	position_[1] = y;
	position_[2] = z;

	if (state_ == PlayerState::PLAYING)
		alSourcefv(sourceId_, AL_POSITION, position_.data());
}

}
