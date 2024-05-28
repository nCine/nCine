#define NCINE_INCLUDE_OPENAL
#define NCINE_INCLUDE_OPENAL_EFX
#include "common_headers.h"
#include <nctl/algorithms.h>
#include "ALAudioDevice.h" // for `hasEfxExtension()`
#include "IAudioPlayer.h"
#include "Vector3.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const float IAudioPlayer::MinGain = 0.0f;
const float IAudioPlayer::DefaultGain = 1.0f;
const float IAudioPlayer::MaxGain = 1.0f;

const float IAudioPlayer::MinPitch = 0.5f;
const float IAudioPlayer::DefaultPitch = 1.0f;
const float IAudioPlayer::MaxPitch = 2.0f;

const float IAudioPlayer::MinConeAngle = 0.0f;
const float IAudioPlayer::DefaultConeAngle = 360.0f;
const float IAudioPlayer::MaxConeAngle = 360.0f;

const float IAudioPlayer::MinConeOuterGain = 0.0f;
const float IAudioPlayer::DefaultConeOuterGain = 0.0f;
const float IAudioPlayer::MaxConeOuterGain = 1.0f;

const float IAudioPlayer::MinAirAbsorptionFactor = 0.0f;
const float IAudioPlayer::DefaultAirAbsorptionFactor = 0.0f;
const float IAudioPlayer::MaxAirAbsorptionFactor = 10.0f;

const float IAudioPlayer::MinRoomRolloffFactor = 0.0f;
const float IAudioPlayer::DefaultRoomRolloffFactor = 0.0f;
const float IAudioPlayer::MaxRoomRolloffFactor = 10.0f;

const float IAudioPlayer::MinConeOuterGainHF = 0.0f;
const float IAudioPlayer::DefaultConeOuterGainHF = 0.0f;
const float IAudioPlayer::MaxConeOuterGainHF = 1.0f;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IAudioPlayer::IAudioPlayer(ObjectType type, const char *name)
    : Object(type, name), sourceId_(IAudioDevice::InvalidSource),
      sourceLocked_(false), state_(PlayerState::STOPPED), isLooping_(false),
      gain_(DefaultGain), pitch_(DefaultPitch),
      position_(0.0f, 0.0f, 0.0f), velocity_(0.0f, 0.0f, 0.0f), direction_(0.0f, 0.0f, 0.0f),
      coneInnerAngle_(DefaultConeAngle), coneOuterAngle_(DefaultConeAngle), coneOuterGain_(DefaultConeOuterGain),
      airAbsorptionFactor_(DefaultAirAbsorptionFactor), roomRooloffFactor_(DefaultRoomRolloffFactor),
      coneOuterGainHF_(DefaultConeOuterGainHF)
{
}

IAudioPlayer::IAudioPlayer(ObjectType type)
    : IAudioPlayer(type, "")
{
}

/* \note Defined here to fix a `no out-of-line virtual method definitions` warning */
IAudioPlayer::~IAudioPlayer()
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool IAudioPlayer::hasSource() const
{
	// `alIsSource()` is not needed, sources belong to a pool and are never deleted
	return (sourceId_ != IAudioDevice::InvalidSource);
}

int IAudioPlayer::sampleOffset() const
{
	int byteOffset = 0;
	if (hasSource())
		alGetSourcei(sourceId_, AL_SAMPLE_OFFSET, &byteOffset);
	return byteOffset;
}

void IAudioPlayer::setSampleOffset(int byteOffset)
{
	if (hasSource())
		alSourcei(sourceId_, AL_SAMPLE_OFFSET, byteOffset);
}

/*! \note Locking an OpenAL source can be useful to retain source properties */
void IAudioPlayer::setSourceLocked(bool sourceLocked)
{
	if (sourceLocked_ != sourceLocked)
	{
		IAudioDevice &device = theServiceLocator().audioDevice();
		if (sourceLocked && hasSource() == false && device.numAvailableSources() > 0)
			device.registerPlayer(this);
		else if (sourceLocked == false && hasSource() && isPlaying() == false && isPaused() == false)
			device.unregisterPlayer(this);

		// Set the flag to `true` only if a source was obtained
		sourceLocked_ = hasSource();
	}
}

void IAudioPlayer::setGain(float gain)
{
	gain_ = nctl::clamp(gain, MinGain, MaxGain);
	if (hasSource())
		alSourcef(sourceId_, AL_GAIN, gain_);
}

void IAudioPlayer::setPitch(float pitch)
{
	pitch_ = nctl::clamp(pitch, MinPitch, MaxPitch);
	if (hasSource())
		alSourcef(sourceId_, AL_PITCH, pitch_);
}

void IAudioPlayer::setPosition(const Vector3f &position)
{
	position_ = position;
	if (hasSource())
		alSourcefv(sourceId_, AL_POSITION, position_.data());
}

void IAudioPlayer::setPosition(float x, float y, float z)
{
	position_.set(x, y, z);
	if (hasSource())
		alSourcefv(sourceId_, AL_POSITION, position_.data());
}

void IAudioPlayer::setVelocity(const Vector3f &velocity)
{
	velocity_ = velocity;
	if (hasSource())
		alSourcefv(sourceId_, AL_VELOCITY, velocity_.data());
}

void IAudioPlayer::setVelocity(float x, float y, float z)
{
	velocity_.set(x, y, z);
	if (hasSource())
		alSourcefv(sourceId_, AL_VELOCITY, velocity_.data());
}

/*! \note Setting all components to zero will make the player omnidirectional */
void IAudioPlayer::setDirection(const Vector3f &direction)
{
	direction_ = direction;
	if (hasSource())
		alSourcefv(sourceId_, AL_DIRECTION, direction_.data());
}

/*! \note Setting all components to zero will make the player omnidirectional */
void IAudioPlayer::setDirection(float x, float y, float z)
{
	direction_.set(x, y, z);
	if (hasSource())
		alSourcefv(sourceId_, AL_DIRECTION, direction_.data());
}

void IAudioPlayer::setConeInnerAngle(float coneAngle)
{
	coneInnerAngle_ = nctl::clamp(coneAngle, MinConeAngle, MaxConeAngle);
	if (hasSource())
		alSourcef(sourceId_, AL_CONE_INNER_ANGLE, coneInnerAngle_);
}

void IAudioPlayer::setConeOuterAngle(float coneAngle)
{
	coneOuterAngle_ = nctl::clamp(coneAngle, MinConeAngle, MaxConeAngle);
	if (hasSource())
		alSourcef(sourceId_, AL_CONE_OUTER_ANGLE, coneOuterAngle_);
}

void IAudioPlayer::setConeOuterGain(float gain)
{
	coneOuterGain_ = nctl::clamp(gain, MinConeOuterGain, MaxConeOuterGain);
	if (hasSource())
		alSourcef(sourceId_, AL_CONE_OUTER_GAIN, coneOuterGain_);
}

void IAudioPlayer::setAirAbsorptionFactor(float factor)
{
	airAbsorptionFactor_ = nctl::clamp(factor, AL_MIN_AIR_ABSORPTION_FACTOR, AL_MAX_AIR_ABSORPTION_FACTOR);
	if (hasEfxExtension() && hasSource())
		alSourcef(sourceId_, AL_AIR_ABSORPTION_FACTOR, airAbsorptionFactor_);
}

void IAudioPlayer::setRoomRolloffFactor(float factor)
{
	roomRooloffFactor_ = nctl::clamp(factor, AL_MIN_ROOM_ROLLOFF_FACTOR, AL_MAX_ROOM_ROLLOFF_FACTOR);
	if (hasEfxExtension() && hasSource())
		alSourcef(sourceId_, AL_ROOM_ROLLOFF_FACTOR, roomRooloffFactor_);
}

void IAudioPlayer::setConeOuterGainHF(float gain)
{
	coneOuterGainHF_ = nctl::clamp(gain, AL_MIN_CONE_OUTER_GAINHF, AL_MAX_CONE_OUTER_GAINHF);
	if (hasEfxExtension() && hasSource())
		alSourcef(sourceId_, AL_CONE_OUTER_GAINHF, coneOuterGainHF_);
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void IAudioPlayer::applySourceProperties()
{
	if (hasSource())
	{
		alGetError();

		alSourcef(sourceId_, AL_GAIN, gain_);
		alSourcef(sourceId_, AL_PITCH, pitch_);
		alSourcefv(sourceId_, AL_POSITION, position_.data());
		alSourcefv(sourceId_, AL_VELOCITY, velocity_.data());
		alSourcefv(sourceId_, AL_DIRECTION, direction_.data());
		alSourcef(sourceId_, AL_CONE_INNER_ANGLE, coneInnerAngle_);
		alSourcef(sourceId_, AL_CONE_OUTER_ANGLE, coneOuterAngle_);
		alSourcef(sourceId_, AL_CONE_OUTER_GAIN, coneOuterGain_);

		if (hasEfxExtension())
		{
			alSourcef(sourceId_, AL_AIR_ABSORPTION_FACTOR, airAbsorptionFactor_);
			alSourcef(sourceId_, AL_ROOM_ROLLOFF_FACTOR, roomRooloffFactor_);
			alSourcef(sourceId_, AL_CONE_OUTER_GAINHF, coneOuterGainHF_);
		}

		const ALenum error = alGetError(); // Checking the error only once, after setting all properties
		ASSERT_MSG_X(error == AL_NO_ERROR, "Error while applying OpenAL source properties: 0x%x", error);
	}
}

}
