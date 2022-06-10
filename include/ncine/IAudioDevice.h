#ifndef CLASS_NCINE_IAUDIODEVICE
#define CLASS_NCINE_IAUDIODEVICE

#include "common_defines.h"

namespace ncine {

class IAudioPlayer;

/// Audio device interface class
class DLL_PUBLIC IAudioDevice
{
  public:
	static const unsigned int UnavailableSource = ~0U;

	enum class PlayerType
	{
		BUFFER,
		AUDIOSTREAM
	};

	virtual const char *name() const = 0;

	virtual ~IAudioDevice() = 0;
	/// Returns the listener gain value
	virtual float gain() const = 0;
	/// Sets the listener gain value
	virtual void setGain(float gain) = 0;

	/// Returns the maximum number of active players
	virtual unsigned int maxNumPlayers() const = 0;
	/// Returns the number of active players
	virtual unsigned int numPlayers() const = 0;
	/// Returns the specified running player object
	virtual const IAudioPlayer *player(unsigned int index) const = 0;

	/// Stops every player currently playing
	virtual void stopPlayers() = 0;
	/// Pauses every player currently playing
	virtual void pausePlayers() = 0;
	/// Stops every player of the specified type
	virtual void stopPlayers(PlayerType playerType) = 0;
	/// Pauses every player of the specified type
	virtual void pausePlayers(PlayerType playerType) = 0;

	/// Pauses every player currently playing without unregistering it
	virtual void freezePlayers() = 0;
	/// Resumes every player previoulsy "frozen" to a playing state
	virtual void unfreezePlayers() = 0;

	/// Returns the next available source index available for playing
	virtual unsigned int nextAvailableSource() = 0;
	/// Registers a new stream player for buffer update
	virtual void registerPlayer(IAudioPlayer *player) = 0;
	/// Updates players state (and buffer queue in the case of stream players)
	virtual void updatePlayers() = 0;
};

inline IAudioDevice::~IAudioDevice() {}

/// A fake audio device which doesn't play anything
class DLL_PUBLIC NullAudioDevice : public IAudioDevice
{
  public:
	const char *name() const override { return "NullAudioDevice"; }

	float gain() const override { return 1.0f; }
	void setGain(float gain) override {}

	unsigned int maxNumPlayers() const override { return 0; }
	unsigned int numPlayers() const override { return 0; }
	const IAudioPlayer *player(unsigned int index) const override { return nullptr; }

	void stopPlayers() override {}
	void pausePlayers() override {}
	void stopPlayers(PlayerType playerType) override {}
	void pausePlayers(PlayerType playerType) override {}

	void freezePlayers() override {}
	void unfreezePlayers() override {}

	unsigned int nextAvailableSource() override { return UnavailableSource; }
	void registerPlayer(IAudioPlayer *player) override {}
	void updatePlayers() override {}
};

}

#endif
