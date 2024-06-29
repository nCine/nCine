#ifndef CLASS_NCINE_IAUDIOPLAYER
#define CLASS_NCINE_IAUDIOPLAYER

#include <ncine/config.h>
#include "Object.h"
#include "Vector3.h"

namespace ncine {

class AudioEffectSlot;
class AudioFilter;

/// Audio player interface class
class DLL_PUBLIC IAudioPlayer : public Object
{
  public:
	static const float MinGain;
	static const float DefaultGain;
	static const float MaxGain;

	static const float MinPitch;
	static const float DefaultPitch;
	static const float MaxPitch;

	static const float MinConeAngle;
	static const float DefaultConeAngle;
	static const float MaxConeAngle;

	static const float MinConeOuterGain;
	static const float DefaultConeOuterGain;
	static const float MaxConeOuterGain;

	static const float MinAirAbsorptionFactor;
	static const float DefaultAirAbsorptionFactor;
	static const float MaxAirAbsorptionFactor;

	static const float MinRoomRolloffFactor;
	static const float DefaultRoomRolloffFactor;
	static const float MaxRoomRolloffFactor;

	static const float MinConeOuterGainHF;
	static const float DefaultConeOuterGainHF;
	static const float MaxConeOuterGainHF;

	/// Player state
	enum class PlayerState
	{
		INITIAL = 0,
		PLAYING,
		PAUSED,
		STOPPED
	};

	IAudioPlayer(ObjectType type, const char *name);
	IAudioPlayer(ObjectType type);
	~IAudioPlayer() override;

	/// Default move constructor
	IAudioPlayer(IAudioPlayer &&) = default;
	/// Default move assignment operator
	IAudioPlayer &operator=(IAudioPlayer &&) = default;

	/// Returns true if a valid OpenAL source is currently assigned to the player
	bool hasSource() const;
	/// Returns the OpenAL id of the player source
	inline unsigned int sourceId() const { return sourceId_; }
	/// Returns the OpenAL id of the currently playing buffer
	virtual unsigned int bufferId() const = 0;

	/// Returns the number of bytes per sample
	virtual int bytesPerSample() const = 0;
	/// Returns the number of audio channels of the currently playing buffer
	virtual int numChannels() const = 0;
	/// Returns the samples frequency of the currently playing buffer
	virtual int frequency() const = 0;

	/// Returns the number of samples
	virtual unsigned long int numSamples() const = 0;
	/// Returns the duration in seconds
	virtual float duration() const = 0;

	/// Returns the size of the currently playing buffer in bytes
	virtual unsigned long bufferSize() const = 0;

	/// Returns the playback position expressed in samples
	int sampleOffset() const;
	/// Sets the playback position expressed in samples
	void setSampleOffset(int offset);

	/// Returns true if the OpenAL source is locked
	inline bool isSourceLocked() const { return sourceLocked_; }
	/// Locks an OpenAL source so it is not released to the pool when the player stops
	void setSourceLocked(bool sourceLocked);

	/// Starts playing
	virtual void play() = 0;
	/// Pauses playing
	virtual void pause() = 0;
	/// Stops playing and rewind
	virtual void stop() = 0;

	/// Returns the state of the player
	inline PlayerState state() const { return state_; }
	/// Queries the playing state of the player
	inline bool isPlaying() const { return state_ == PlayerState::PLAYING; }
	/// Queries the paused state of the player
	inline bool isPaused() const { return state_ == PlayerState::PAUSED; }
	/// Queries the stopped state of the player
	inline bool isStopped() const { return state_ == PlayerState::STOPPED; }

	/// Queries the looping property of the player
	inline bool isLooping() const { return isLooping_; }
	/// Sets player looping property
	inline void setLooping(bool isLooping) { isLooping_ = isLooping; }

	/// Returns player gain value
	inline float gain() const { return gain_; }
	/// Sets player gain value
	void setGain(float gain);
	/// Returns player pitch value
	inline float pitch() const { return pitch_; }
	/// Sets player pitch value
	void setPitch(float pitch);

	/// Returns player position vector
	inline Vector3f position() const { return position_; }
	/// Sets player position vector
	void setPosition(const Vector3f &position);
	/// Sets player position vector through components
	void setPosition(float x, float y, float z);
	/// Returns player velocity vector
	inline Vector3f velocity() const { return velocity_; }
	/// Sets player velocity vector
	void setVelocity(const Vector3f &velocity);
	/// Sets player velocity vector through components
	void setVelocity(float x, float y, float z);
	/// Returns player direction vector
	inline Vector3f direction() const { return direction_; }
	/// Sets player direction vector
	void setDirection(const Vector3f &direction);
	/// Sets player direction vector through components
	void setDirection(float x, float y, float z);

	/// Returns the player inside angle of the sound cone in degrees
	inline float coneInnerAngle() const { return coneInnerAngle_; }
	/// Sets the player inside angle of the sound cone in degrees
	void setConeInnerAngle(float coneAngle);
	/// Returns the player outside angle of the sound cone in degrees
	inline float coneOuterAngle() const { return coneOuterAngle_; }
	/// Sets the player outside angle of the sound cone in degrees
	void setConeOuterAngle(float coneAngle);
	/// Returns the player multiplication factor to determine the gain outside the cone
	inline float coneOuterGain() const { return coneOuterGain_; }
	/// Sets the player multiplication factor to determine the gain outside the cone
	void setConeOuterGain(float gain);

#if NCINE_WITH_OPENAL_EXT
	/// Returns the player air absorption factor
	inline float airAbsorptionFactor() const { return airAbsorptionFactor_; }
	/// Sets the player air absorption factor
	void setAirAbsorptionFactor(float factor);

	/// Returns the player room rolloff factor
	inline float roomRolloffFactor() const { return roomRooloffFactor_; }
	/// Sets the player room rolloff factor
	void setRoomRolloffFactor(float factor);

	/// Returns the player cone outer gain HF value
	inline float coneOuterGainHF() const { return coneOuterGainHF_; }
	/// Sets the player cone outer gain HF value
	void setConeOuterGainHF(float gain);

	/// Returns true if a valid OpenAL effect slot is currently assigned to the player
	bool hasEffectSlot() const;
	/// Returns the OpenAL id of the effect slot
	inline unsigned int effectSlotId() const { return effectSlotId_; }
	/// Returns true if a valid OpenAL auxiliary filter is currently assigned to the player
	bool hasAuxFilter() const;
	/// Returns the OpenAL id of the auxiliary filter
	inline unsigned int auxFilterId() const { return auxFilterId_; }
	/// Returns true if a valid OpenAL direct filter is currently assigned to the player
	bool hasDirectFilter() const;
	/// Returns the OpenAL id of the direct filter
	inline unsigned int directFilterId() const { return directFilterId_; }

	/// Sets or removes an affect slot, with an optional auxiliary filter, to the player
	void setEffectSlot(const AudioEffectSlot *effectSlot, const AudioFilter *filter);
	/// Sets or removes an affect slot to the player
	inline void setEffectSlot(const AudioEffectSlot *effectSlot) { setEffectSlot(effectSlot, nullptr); }
	/// Sets or remove the filter parameters from a filter object to the direct signal
	void setDirectFilter(const AudioFilter *audioFilter);
#endif

  protected:
	/// The OpenAL source id
	unsigned int sourceId_;
	/// The flag indicating if an OpenAL source is locked and never released to the pool
	bool sourceLocked_;
	/// Current player state
	PlayerState state_;
	/// Looping status flag
	bool isLooping_;

	/// Player gain value
	float gain_;
	/// Player pitch value
	float pitch_;

	/// Player position in space
	Vector3f position_;
	/// Player velocity in space
	Vector3f velocity_;
	/// Player direction in space
	/*! \note If all components are zero the player is omnidirectional */
	Vector3f direction_;

	/// Inside angle of the sound cone in degrees
	/*! \note The default value of 360 means that the inner angle covers the entire world. */
	float coneInnerAngle_;
	/// Outside angle of the sound cone in degrees
	/*! \note The default value of 360 means that the outer angle covers the entire world. */
	float coneOuterAngle_;
	/// Multiplication factor to determine the gain outside the cone
	float coneOuterGain_;

	/// Player air absorption factor value
	float airAbsorptionFactor_;
	/// Player room rolloff factor value
	float roomRooloffFactor_;
	/// Player cone outer gain HF value
	float coneOuterGainHF_;

	/// The OpenAL effect slot id to use when playing
	unsigned int effectSlotId_;
	/// The OpenAL auxiliary filter id to use when playing
	unsigned int auxFilterId_;
	/// The OpenAL direct filter id to use when playing
	unsigned int directFilterId_;

	/// Updates the state of the player if the source has finished playing
	/*! It is called every frame by the `IAudioDevice` class and it is
	 *  also responsible for buffer queueing/unqueueing in stream players. */
	virtual void updateState() = 0;

	/// Applies source properties after registering a player
	void applySourceProperties();

	friend class ALAudioDevice;
};

}

#endif
