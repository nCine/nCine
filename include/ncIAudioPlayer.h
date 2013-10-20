#ifndef CLASS_NCIAUDIOPLAYER
#define CLASS_NCIAUDIOPLAYER

#ifdef __APPLE__
	#include <OpenAL/al.h>
#else
	#include <AL/al.h>
#endif
#include "ncObject.h"

/// Audio player interface class
class ncIAudioPlayer : public ncObject
{
public:

	/// Player state
	enum ePlayerState {
		STATE_INITIAL = 0,
		STATE_PLAYING,
		STATE_PAUSED,
		STATE_STOPPED
	};

protected:
	/// The OpenAL source id
	unsigned int m_uSource;
	/// Current player state
	ePlayerState m_eState;
	/// Looping status flag
	bool m_bLooping;
	/// Player gain value
	float m_fGain;
	/// Player pitch value
	float m_fPitch;
	/// Player position in space
	float m_fPosition[3];

public:
	ncIAudioPlayer()
		: m_uSource(-1), m_eState(STATE_STOPPED),
		  m_bLooping(false), m_fGain(1.0f), m_fPitch(1.0f)
	{
		m_fPosition[0] = 0.0f;
		m_fPosition[1] = 0.0f;
		m_fPosition[2] = 0.0f;
	}
	virtual ~ncIAudioPlayer() { }

	/// Starts playing
	virtual void Play() = 0;
	/// Pauses playing
	virtual void Pause() = 0;
	/// Stop playing and rewind
	virtual void Stop() = 0;

	/// Updates the state of the player if the source has done playing
	/*! It is called every frame by the ncIAudioDevice class and it is
		also responsible for buffer queueing/unqueueing in stream players */
	virtual void UpdateState() = 0;

	/// Sets player looping property
	inline void SetLooping(bool bLooping) { m_bLooping = bLooping; }

	/// Returns player gain value
	inline float Gain() const { return m_fGain; }
	/// Sets player gain value
	/*! It gets applied to the OpenAL source only when playing */
	inline void SetGain(float fGain)
	{
		m_fGain = fGain;
		if (m_eState == STATE_PLAYING)
			alSourcef(m_uSource, AL_GAIN, m_fGain);
	}
	/// Returns player pitch value
	inline float Pitch() const { return m_fPitch; }
	/// Sets player pitch value
	/*! It gets applied to the OpenAL source only when playing */
	inline void SetPitch(float fPitch)
	{
		m_fPitch = fPitch;
		if (m_eState == STATE_PLAYING)
			alSourcef(m_uSource, AL_PITCH, m_fPitch);
	}
	/// Returns player position value
	inline const float* Position() const { return m_fPosition; }
	/// Sets player position value through vector
	/*! It gets applied to the OpenAL source only when playing */
	inline void SetPosition(float fPosition[3])
	{
		m_fPosition[0] = fPosition[0];
		m_fPosition[1] = fPosition[1];
		m_fPosition[2] = fPosition[2];

		if (m_eState == STATE_PLAYING)
			alSourcefv(m_uSource, AL_POSITION, m_fPosition);
	}
	/// Sets player position value through components
	/*! It gets applied to the OpenAL source only when playing */
	inline void SetPosition(float fX, float fY, float fZ)
	{
		m_fPosition[0] = fX;
		m_fPosition[1] = fY;
		m_fPosition[2] = fZ;

		if (m_eState == STATE_PLAYING)
			alSourcefv(m_uSource, AL_POSITION, m_fPosition);
	}

	/// Queries the playing state of the player
	inline bool isPlaying() const { return m_eState == STATE_PLAYING; }
	/// Queries the paused state of the player
	inline bool isPaused() const { return m_eState == STATE_PAUSED; }
	/// Queries the stopped state of the player
	inline bool isStopped() const { return m_eState == STATE_STOPPED; }
	/// Queries the looping property of the player
	inline bool isLooping() const { return m_bLooping; }
};

#endif
