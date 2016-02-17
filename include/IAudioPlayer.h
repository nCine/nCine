#ifndef CLASS_NCINE_IAUDIOPLAYER
#define CLASS_NCINE_IAUDIOPLAYER

#include "Object.h"
#include "StaticArray.h"

namespace ncine {

/// Audio player interface class
class DLL_PUBLIC IAudioPlayer : public Object
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

	IAudioPlayer();
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

	/// Returns the state of the player
	inline PlayerState state() const { return state_; }
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

}

#endif
