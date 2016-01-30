#ifndef CLASS_NCINE_IAUDIODEVICE
#define CLASS_NCINE_IAUDIODEVICE

#include "common_defines.h"

namespace ncine {

class IAudioPlayer;

/// It represents the interface to the audio device
class DLL_PUBLIC IAudioDevice
{
  public:
	enum PlayerType
	{
		BUFFER_PLAYER,
		AUDIOSTREAM_PLAYER
	};

	virtual ~IAudioDevice() = 0;
	/// Returns the listener gain value
	virtual float gain() = 0;
	/// Sets the listener gain value
	virtual void setGain(float gain) = 0;

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
	virtual int nextAvailableSource() = 0;
	/// Register a new stream player for buffer update
	virtual void registerPlayer(IAudioPlayer *player) = 0;
	/// Updates players state (and buffer queue in the case of stream players)
	virtual void updatePlayers() = 0;
};

inline IAudioDevice::~IAudioDevice() { }


/// A fake audio device which doesn't play anything
class DLL_PUBLIC NullAudioDevice : public IAudioDevice
{
  public:
	virtual float gain() { return 1.0f; }
	virtual void setGain(float gain) { }

	virtual void stopPlayers() { }
	virtual void pausePlayers() { }
	virtual void stopPlayers(PlayerType playerType) { }
	virtual void pausePlayers(PlayerType playerType) { }

	virtual void freezePlayers() { }
	virtual void unfreezePlayers() { }

	virtual int nextAvailableSource() { return -1; }
	virtual void registerPlayer(IAudioPlayer *player) { }
	virtual void updatePlayers() { }
};

}

#endif
