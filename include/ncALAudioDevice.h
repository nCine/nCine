#ifndef CLASS_NCALAUDIODEVICE
#define CLASS_NCALAUDIODEVICE

#ifdef __APPLE__
	#include <OpenAL/alc.h>
	#include <OpenAL/al.h>
#else
	#include <AL/alc.h>
	#include <AL/al.h>
#endif
#include "ncIAudioDevice.h"
#include "ncList.h"

/// It represents the interface to the OpenAL audio device
class ncALAudioDevice : public ncIAudioDevice
{
 public:
	ncALAudioDevice();
	virtual ~ncALAudioDevice();

	virtual float Gain() { return m_fGain; }
	virtual void SetGain(float fGain);

	virtual void StopPlayers();
	virtual void PausePlayers();
	inline virtual void StopBufferPlayers() { StopOrPauseBufferPlayers(true); }
	inline virtual void PauseBufferPlayers() { StopOrPauseBufferPlayers(false); }
	inline virtual void StopStreamPlayers() { StopOrPauseStreamPlayers(true); }
	inline virtual void PauseStreamPlayers() { StopOrPauseStreamPlayers(false); }

	virtual int NextAvailableSource();
	virtual void RegisterPlayer(ncIAudioPlayer *pPlayer);
	virtual void UpdatePlayers();

 private:
	/// Maximum number of OpenAL sources (HACK: should use a query)
	static const unsigned int s_uMaxSources = 16;

	/// The OpenAL device
	ALCdevice *m_pDevice;
	/// The OpenAL context for the device
	ALCcontext *m_pContext;
	/// The listener gain value (master volume)
	ALfloat m_fGain;
	/// The sources pool
	ALuint m_uSources[s_uMaxSources];

	/// A list of active audio players
	ncList<ncIAudioPlayer *> m_players;

	// Stops or pauses all buffer players
	void StopOrPauseBufferPlayers(bool bStop);
	// Stops or pauses all stream players
	void StopOrPauseStreamPlayers(bool bStop);
};

#endif
