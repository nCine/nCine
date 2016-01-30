#ifndef CLASS_NCINE_ALAUDIODEVICE
#define CLASS_NCINE_ALAUDIODEVICE

#define NCINE_INCLUDE_OPENALC
#include "common_headers.h"

#include "IAudioDevice.h"
#include "List.h"
#include "StaticArray.h"

namespace ncine {

/// It represents the interface to the OpenAL audio device
class ALAudioDevice : public IAudioDevice
{
  public:
	ALAudioDevice();
	virtual ~ALAudioDevice();

	virtual float gain() { return gain_; }
	virtual void setGain(float gain);

	virtual void stopPlayers();
	virtual void pausePlayers();
	virtual void stopPlayers(PlayerType playerType);
	virtual void pausePlayers(PlayerType playerType);

	virtual void freezePlayers();
	virtual void unfreezePlayers();

	virtual int nextAvailableSource();
	virtual void registerPlayer(IAudioPlayer *player);
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
	StaticArray<ALuint, MaxSources> sources_;

	/// A list of active audio players
	List<IAudioPlayer *> players_;

	/// Private copy constructor
	ALAudioDevice(const ALAudioDevice&);
	/// Private assignment operator
	ALAudioDevice& operator=(const ALAudioDevice&);
};

}

#endif
