#ifndef CLASS_NCIAUDIODEVICE
#define CLASS_NCIAUDIODEVICE

class ncIAudioPlayer;

/// It represents the interface to the audio device
class ncIAudioDevice
{
  public:
	virtual ~ncIAudioDevice() = 0;
	/// Returns the listener gain value
	virtual float gain() = 0;
	/// Sets the listener gain value
	virtual void setGain(float gain) = 0;

	/// Stops every player currently playing
	virtual void stopPlayers() = 0;
	/// Pauses every player currently playing
	virtual void pausePlayers() = 0;
	/// Stops every buffer player currently playing
	virtual void stopBufferPlayers() = 0;
	/// Pauses every buffer player currently playing
	virtual void pauseBufferPlayers() = 0;
	/// Stops every stream player currently playing
	virtual void stopStreamPlayers() = 0;
	/// Pauses every stream player currently playing
	virtual void pauseStreamPlayers() = 0;

	/// Returns the next available source index available for playing
	virtual int nextAvailableSource() = 0;
	/// Register a new stream player for buffer update
	virtual void registerPlayer(ncIAudioPlayer *player) = 0;
	/// Updates players state (and buffer queue in the case of stream players)
	virtual void updatePlayers() = 0;
};

inline ncIAudioDevice::~ncIAudioDevice() { }

#endif

#ifndef CLASS_NCNULLAUDIODEVICE
#define CLASS_NCNULLAUDIODEVICE

/// A fake audio device which doesn't play anything
class ncNullAudioDevice : public ncIAudioDevice
{
  public:
	virtual float gain() { return 1.0f; }
	virtual void setGain(float gain) { }

	virtual void stopPlayers() { }
	virtual void pausePlayers() { }
	virtual void stopBufferPlayers() { }
	virtual void pauseBufferPlayers() { }
	virtual void stopStreamPlayers() { }
	virtual void pauseStreamPlayers() { }

	virtual int nextAvailableSource() { return -1; }
	virtual void registerPlayer(ncIAudioPlayer *player) { }
	virtual void updatePlayers() { }
};

#endif
