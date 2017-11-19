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
	~ALAudioDevice() override;

	float gain() override { return gain_; }
	void setGain(float gain) override;

	void stopPlayers() override;
	void pausePlayers() override;
	void stopPlayers(PlayerType playerType) override;
	void pausePlayers(PlayerType playerType) override;

	void freezePlayers() override;
	void unfreezePlayers() override;

	int nextAvailableSource() override;
	void registerPlayer(IAudioPlayer *player) override;
	void updatePlayers() override;

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

	/// Deleted copy constructor
	ALAudioDevice(const ALAudioDevice &) = delete;
	/// Deleted assignment operator
	ALAudioDevice &operator=(const ALAudioDevice &) = delete;
};

}

#endif
