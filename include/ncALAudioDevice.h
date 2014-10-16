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

	virtual float gain() { return gain_; }
	virtual void setGain(float gain);

	virtual void stopPlayers();
	virtual void pausePlayers();
	inline virtual void stopBufferPlayers() { stopOrPauseBufferPlayers(true); }
	inline virtual void pauseBufferPlayers() { stopOrPauseBufferPlayers(false); }
	inline virtual void stopStreamPlayers() { stopOrPauseStreamPlayers(true); }
	inline virtual void pauseStreamPlayers() { stopOrPauseStreamPlayers(false); }

	virtual int nextAvailableSource();
	virtual void registerPlayer(ncIAudioPlayer *player);
	virtual void updatePlayers();

  private:
	/// Maximum number of OpenAL sources (HACK: should use a query)
	static const unsigned int MaxSources = 16;

	/// The OpenAL device
	ALCdevice *device_;
	/// The OpenAL context for the device
	ALCcontext *context_;
	/// The listener gain value (master volume)
	ALfloat gain_;
	/// The sources pool
	ALuint sources_[MaxSources];

	/// A list of active audio players
	ncList<ncIAudioPlayer *> players_;

	// Stops or pauses all buffer players
	void stopOrPauseBufferPlayers(bool shouldStop);
	// Stops or pauses all stream players
	void stopOrPauseStreamPlayers(bool shouldStop);
};

#endif
