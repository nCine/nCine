#ifndef CLASS_NCALAUDIODEVICE
#define CLASS_NCALAUDIODEVICE

#include <AL/alc.h>
#include <AL/al.h>
#include "ncIAudioDevice.h"
#include "ncList.h"

/// It represents the interface to the OpenAL audio device
class ncALAudioDevice : public ncIAudioDevice
{
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

	/// A list of active stream players
	ncList<ncAudioStreamPlayer *> m_streamPlayers;
public:
	ncALAudioDevice();
	virtual ~ncALAudioDevice();

	virtual float Gain() { return m_fGain; }
	virtual void SetGain(float fGain);

	virtual int NextAvailableSource();
	virtual void RegisterStreamPlayer(ncAudioStreamPlayer *pStreamPlayer);
	virtual void UpdateStreams();
};

#endif
