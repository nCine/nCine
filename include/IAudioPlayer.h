#ifndef CLASS_NCINE_IAUDIOPLAYER
#define CLASS_NCINE_IAUDIOPLAYER

#ifdef __APPLE__
	#include <OpenAL/al.h>
#else
	#include <AL/al.h>
#endif
#include "Object.h"
#include "StaticArray.h"

namespace ncine {

/// Audio player interface class
class IAudioPlayer : public Object
{
  public:
	/// Player state
	enum PlayerState
	{
		STATE_INITIAL = 0,
		STATE_PLAYING,
		STATE_PAUSED,
		STATE_STOPPED
	};

	IAudioPlayer()
		: sourceId_(-1), state_(STATE_STOPPED),
		  isLooping_(false), gain_(1.0f), pitch_(1.0f)
	{
		position_[0] = 0.0f;
		position_[1] = 0.0f;
		position_[2] = 0.0f;
	}
	virtual ~IAudioPlayer() { }

	/// Starts playing
	virtual void play() = 0;
	/// Pauses playing
	virtual void pause() = 0;
	/// Stop playing and rewind
	virtual void stop() = 0;

	/// Updates the state of the player if the source has done playing
	/*! It is called every frame by the IAudioDevice class and it is
		also responsible for buffer queueing/unqueueing in stream players */
	virtual void updateState() = 0;

	/// Sets player looping property
	inline void setLooping(bool isLooping) { isLooping_ = isLooping; }

	/// Returns player gain value
	inline float gain() const { return gain_; }
	// Sets player gain value
	void setGain(float gain);
	/// Returns player pitch value
	inline float pitch() const { return pitch_; }
	// Sets player pitch value
	void setPitch(float pitch);
	/// Returns player position value
	inline const float* Position() const { return position_.data(); }
	// Sets player position value through vector
	void setPosition(float position[3]);
	// Sets player position value through components
	void setPosition(float x, float y, float z);

	/// Queries the playing state of the player
	inline bool isPlaying() const { return state_ == STATE_PLAYING; }
	/// Queries the paused state of the player
	inline bool isPaused() const { return state_ == STATE_PAUSED; }
	/// Queries the stopped state of the player
	inline bool isStopped() const { return state_ == STATE_STOPPED; }
	/// Queries the looping property of the player
	inline bool isLooping() const { return isLooping_; }

  protected:
	/// The OpenAL source id
	unsigned int sourceId_;
	/// Current player state
	PlayerState state_;
	/// Looping status flag
	bool isLooping_;
	/// Player gain value
	float gain_;
	/// Player pitch value
	float pitch_;
	/// Player position in space
	StaticArray<float, 3> position_;
};

/// Sets player gain value
/*! It gets applied to the OpenAL source only when playing */
inline void IAudioPlayer::setGain(float gain)
{
	gain_ = gain;
	if (state_ == STATE_PLAYING)
	{
		alSourcef(sourceId_, AL_GAIN, gain_);
	}
}

/// Sets player pitch value
/*! It gets applied to the OpenAL source only when playing */
inline void IAudioPlayer::setPitch(float pitch)
{
	pitch_ = pitch;
	if (state_ == STATE_PLAYING)
	{
		alSourcef(sourceId_, AL_PITCH, pitch_);
	}
}

/// Sets player position value through vector
/*! It gets applied to the OpenAL source only when playing */
inline void IAudioPlayer::setPosition(float position[3])
{
	position_[0] = position[0];
	position_[1] = position[1];
	position_[2] = position[2];

	if (state_ == STATE_PLAYING)
	{
		alSourcefv(sourceId_, AL_POSITION, position_.data());
	}
}

/// Sets player position value through components
/*! It gets applied to the OpenAL source only when playing */
inline void IAudioPlayer::setPosition(float x, float y, float z)
{
	position_[0] = x;
	position_[1] = y;
	position_[2] = z;

	if (state_ == STATE_PLAYING)
	{
		alSourcefv(sourceId_, AL_POSITION, position_.data());
	}
}

}

#endif
