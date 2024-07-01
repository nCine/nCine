#ifndef CLASS_NCINE_IAUDIODEVICE
#define CLASS_NCINE_IAUDIODEVICE

#include "common_defines.h"
#include "Vector3.h"

namespace ncine {

class IAudioPlayer;

/// Audio device interface class
class DLL_PUBLIC IAudioDevice
{
  public:
	static const unsigned int InvalidSource = ~0U;

	/// Audio device attributes class
	struct Attributes
	{
		const char *deviceName = nullptr;

		int majorVersion = 0;
		int minorVersion = 0;
		int outputFrequency = 0;
		int numMonoSources = 0;
		int numStereoSources = 0;
		int refreshRate = 0;
		int synchronous = 0;

		// EFX attributes
		int efxMajorVersion = 0;
		int efxMinorVersion = 0;
		int maxAuxiliarySends = 0;
	};

	/// OpenAL queryable extensions
	struct ALExtensions
	{
		enum Enum
		{
			EXT_EFX = 0,
			SOFT_PAUSE_DEVICE,
			SOFT_DEFERRED_UPDATES,
			SOFT_SOURCE_SPATIALIZE,

			COUNT
		};
	};

	enum class PlayerType
	{
		BUFFER,
		STREAM
	};

	virtual const Attributes &attributes() const = 0;
	virtual const char *name() const = 0;
	/// Returns true if the specified OpenAL extension is available
	virtual bool hasExtension(ALExtensions::Enum extensionName) const = 0;

	virtual ~IAudioDevice() = 0;
	/// Returns the listener gain value
	virtual float gain() const = 0;
	/// Sets the listener gain value
	virtual void setGain(float gain) = 0;

	/// Returns the listener position vector
	virtual Vector3f position() const = 0;
	/// Sets the listener position vector
	virtual void setPosition(const Vector3f &position) = 0;
	/// Sets the listener position vector through components
	virtual void setPosition(float x, float y, float z) = 0;

	/// Returns the listener velocity vector
	virtual Vector3f velocity() const = 0;
	/// Sets the listener velocity vector
	virtual void setVelocity(const Vector3f &velocity) = 0;
	/// Sets the listener velocity vector through components
	virtual void setVelocity(float x, float y, float z) = 0;

	/// Returns the maximum number of audio sources
	virtual unsigned int maxNumSources() const = 0;
	/// Returns the number of available audio sources
	virtual unsigned int numAvailableSources() const = 0;

	/// Returns the number of active players
	virtual unsigned int numPlayers() const = 0;
	/// Returns the specified running player object (const version)
	virtual const IAudioPlayer *player(unsigned int index) const = 0;
	/// Returns the specified running player object
	virtual IAudioPlayer *player(unsigned int index) = 0;

	/// Pauses every player currently playing
	/*! \note Paused players can be resumed with `resumePlayers()` */
	virtual void pausePlayers() = 0;
	/// Stops every player currently playing
	virtual void stopPlayers() = 0;
	/// Pauses every player of the specified type
	virtual void pausePlayers(PlayerType playerType) = 0;
	/// Stops every player of the specified type
	virtual void stopPlayers(PlayerType playerType) = 0;
	/// Resumes all and only the players that were paused by a `pausePlayers()` call
	virtual void resumePlayers() = 0;

	/// Pauses all audio device activities using an OpenAL-soft extension (if available)
	/*! \note If the extension is not available, all players are paused */
	virtual void pauseDevice() = 0;
	/// Resumes all audio device activities using an OpenAL-soft extension (if available)
	/// /*! \note If the extension is not available, all paused players resume playing */
	virtual void resumeDevice() = 0;

	/// Registers a new stream player for buffer update
	virtual void registerPlayer(IAudioPlayer *player) = 0;
	/// Remove a stream player from the array of active players
	virtual void unregisterPlayer(IAudioPlayer *player) = 0;
	/// Updates players state (and buffer queue in the case of stream players)
	virtual void updatePlayers() = 0;
};

inline IAudioDevice::~IAudioDevice() {}

/// A fake audio device which doesn't play anything
class DLL_PUBLIC NullAudioDevice : public IAudioDevice
{
  public:
	const Attributes &attributes() const override { return properties_; }
	const char *name() const override { return "NullAudioDevice"; }
	bool hasExtension(ALExtensions::Enum extensionName) const override { return false; }

	float gain() const override { return 1.0f; }
	void setGain(float gain) override {}

	Vector3f position() const override { return Vector3f::Zero; }
	void setPosition(const Vector3f &position) override {}
	void setPosition(float x, float y, float z) override {}

	Vector3f velocity() const override { return Vector3f::Zero; }
	void setVelocity(const Vector3f &velocity) override {}
	void setVelocity(float x, float y, float z) override {}

	unsigned int maxNumSources() const override { return 0; }
	unsigned int numAvailableSources() const override { return 0; }

	unsigned int numPlayers() const override { return 0; }
	const IAudioPlayer *player(unsigned int index) const override { return nullptr; }
	IAudioPlayer *player(unsigned int index) override { return nullptr; }

	void pausePlayers() override {}
	void stopPlayers() override {}
	void pausePlayers(PlayerType playerType) override {}
	void stopPlayers(PlayerType playerType) override {}
	void resumePlayers() override {}

	void pauseDevice() override {}
	void resumeDevice() override {}

	void registerPlayer(IAudioPlayer *player) override {}
	void unregisterPlayer(IAudioPlayer *player) override {}
	void updatePlayers() override {}

  private:
	Attributes properties_;
};

}

#endif
