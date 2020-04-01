#ifndef CLASS_NCINE_ALAUDIODEVICE
#define CLASS_NCINE_ALAUDIODEVICE

#define NCINE_INCLUDE_OPENALC
#include "common_headers.h"

#include "IAudioDevice.h"
#include <nctl/List.h>
#include <nctl/StaticArray.h>

namespace ncine {

/// It represents the interface to the OpenAL audio device
class ALAudioDevice : public IAudioDevice
{
  public:
	ALAudioDevice();
	~ALAudioDevice() override;

	inline const char *name() const override { return deviceName_; }

	float gain() const override { return gain_; }
	void setGain(float gain) override;

	inline unsigned int numPlayers() const override { return players_.size(); }
	const IAudioPlayer *player(unsigned int index) const override;

	void stopPlayers() override;
	void pausePlayers() override;
	void stopPlayers(PlayerType playerType) override;
	void pausePlayers(PlayerType playerType) override;

	void freezePlayers() override;
	void unfreezePlayers() override;

	unsigned int nextAvailableSource() override;
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
	nctl::StaticArray<ALuint, MaxSources> sources_;
	/// The array of currently active audio players
	nctl::StaticArray<IAudioPlayer *, MaxSources> players_;

	/// The OpenAL device name string
	const char *deviceName_;

	/// Deleted copy constructor
	ALAudioDevice(const ALAudioDevice &) = delete;
	/// Deleted assignment operator
	ALAudioDevice &operator=(const ALAudioDevice &) = delete;
};

}

#endif
