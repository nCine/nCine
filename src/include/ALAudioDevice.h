#ifndef CLASS_NCINE_ALAUDIODEVICE
#define CLASS_NCINE_ALAUDIODEVICE

#define NCINE_INCLUDE_OPENALC
#include "common_headers.h"

#include "IAudioDevice.h"
#include <nctl/Array.h>
#include <nctl/HashSet.h>

namespace ncine {

class AppConfiguration;

/// It represents the interface to the OpenAL audio device
class ALAudioDevice : public IAudioDevice
{
  public:
	explicit ALAudioDevice(const AppConfiguration &appCfg);
	~ALAudioDevice() override;

	inline const Attributes &attributes() const override { return attributes_; }
	inline const char *name() const override { return attributes_.deviceName; }
	bool hasExtension(ALExtensions::Enum extensionName) const override;

	float gain() const override { return gain_; }
	void setGain(float gain) override;

	Vector3f position() const override { return position_; }
	void setPosition(const Vector3f &position) override;
	void setPosition(float x, float y, float z) override;

	Vector3f velocity() const override { return velocity_; }
	void setVelocity(const Vector3f &velocity) override;
	void setVelocity(float x, float y, float z) override;

	inline unsigned int maxNumSources() const override { return sources_.size(); }
	inline unsigned int numAvailableSources() const override { return sourcesPool_.size(); }

	inline unsigned int numPlayers() const override { return players_.size(); }
	const IAudioPlayer *player(unsigned int index) const override;
	IAudioPlayer *player(unsigned int index) override;

	void pausePlayers() override;
	void stopPlayers() override;
	void pausePlayers(PlayerType playerType) override;
	void stopPlayers(PlayerType playerType) override;
	void resumePlayers() override;

	void pauseDevice() override;
	void resumeDevice() override;

	void registerPlayer(IAudioPlayer *player) override;
	void unregisterPlayer(IAudioPlayer *player) override;
	void updatePlayers() override;

  private:
	/// The OpenAL device
	ALCdevice *device_;
	/// The OpenAL context for the device
	ALCcontext *context_;
	/// The listener gain value (master volume)
	ALfloat gain_;
	/// Listener position in space
	Vector3f position_;
	/// Listener velocity in space
	Vector3f velocity_;

	Attributes attributes_;

	/// The array of all audio sources
	nctl::Array<ALuint> sources_;
	/// The array of currently inactive audio sources
	nctl::Array<ALuint> sourcesPool_;
	/// The array of currently active audio players
	nctl::Array<IAudioPlayer *> players_;
	/// The array of audio players that have been paused by `pausePlayers()`
	/*! \note A separate container is required as to not resume players that were already in the paused state */
	nctl::HashSet<IAudioPlayer *> pausedPlayers_;

	/// Array of OpenAL extension availability flags
	bool alExtensions_[IAudioDevice::ALExtensions::COUNT];

	void retrieveAttributes();
	void retrieveExtensions();
	void logALAttributes();
	void logALExtensions();

	/// Deleted copy constructor
	ALAudioDevice(const ALAudioDevice &) = delete;
	/// Deleted assignment operator
	ALAudioDevice &operator=(const ALAudioDevice &) = delete;
};

/// Returns a cached value for the EFX extension availability flag
extern bool hasEfxExtension();

}

#endif
