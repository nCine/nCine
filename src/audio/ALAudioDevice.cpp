#ifdef WITH_OPENAL_EXT
	#include "openal_proto.h"
#endif

#include "common_macros.h"
#include "ALAudioDevice.h"
#include "AudioBufferPlayer.h"
#include "AudioStreamPlayer.h"
#include "AppConfiguration.h"
#include <nctl/HashSetIterator.h>

namespace ncine {

const char *ExtensionNames[IAudioDevice::ALExtensions::COUNT] = {
#ifdef WITH_OPENAL_EXT
	ALC_EXT_EFX_NAME,
#else
	"ALC_EXT_EFX",
#endif
	"ALC_SOFT_pause_device",
	"AL_SOFT_deferred_updates",
	"AL_SOFT_source_spatialize"
};

bool hasEfxExtension_ = false;

bool hasEfxExtension()
{
	return hasEfxExtension_;
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ALAudioDevice::ALAudioDevice(const AppConfiguration &appCfg)
    : device_(nullptr), context_(nullptr),
      gain_(1.0f), position_(Vector3f::Zero), velocity_(Vector3f::Zero),
      pausedPlayers_(1)
{
	device_ = alcOpenDevice(nullptr);
	FATAL_ASSERT_MSG_X(device_ != nullptr, "alcOpenDevice failed: 0x%x", alGetError());

	const ALCint attrList[7] = {
#ifdef __EMSCRIPTEN__
		ALC_FREQUENCY, (appCfg.outputAudioFrequency > 0) ? static_cast<ALCint>(appCfg.outputAudioFrequency) : 44100,
		ALC_MONO_SOURCES, (appCfg.monoAudioSources > 0) ? static_cast<ALCint>(appCfg.monoAudioSources) : 15,
		ALC_STEREO_SOURCES, (appCfg.stereoAudioSources > 0) ? static_cast<ALCint>(appCfg.stereoAudioSources) : 1,
#else
		ALC_FREQUENCY, static_cast<ALCint>(appCfg.outputAudioFrequency),
		ALC_MONO_SOURCES, static_cast<ALCint>(appCfg.monoAudioSources),
		ALC_STEREO_SOURCES, static_cast<ALCint>(appCfg.stereoAudioSources),
#endif

		0
	};

	context_ = alcCreateContext(device_, attrList);
	if (context_ == nullptr)
	{
		alcCloseDevice(device_);
		FATAL_MSG_X("alcCreateContext failed: 0x%x", alGetError());
	}

	if (!alcMakeContextCurrent(context_))
	{
		alcDestroyContext(context_);
		alcCloseDevice(device_);
		FATAL_MSG_X("alcMakeContextCurrent failed: 0x%x", alGetError());
	}

	retrieveAttributes();
	for (unsigned int i = 0; i < ALExtensions::COUNT; i++)
		alExtensions_[i] = false;
	retrieveExtensions();

	const unsigned int MaxAvailableSources = unsigned(attributes_.numMonoSources + attributes_.numStereoSources);
	unsigned int sourcePoolSize = appCfg.monoAudioSources + appCfg.stereoAudioSources;
	if (sourcePoolSize == 0 || sourcePoolSize > MaxAvailableSources)
		sourcePoolSize = MaxAvailableSources;
	sources_.setSize(sourcePoolSize);
	pausedPlayers_ = nctl::move(nctl::HashSet<IAudioPlayer *>(sourcePoolSize));

	alGetError();
	alGenSources(sources_.size(), sources_.data());
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alGenSources failed: 0x%x", error);
	sourcesPool_ = sources_;

	for (unsigned int i = 0; i < sources_.size(); i++)
		ASSERT(alIsSource(sources_[i]) == AL_TRUE);

	alListenerf(AL_GAIN, gain_);
	alListenerfv(AL_POSITION, position_.data());
	alListenerfv(AL_VELOCITY, velocity_.data());

#ifdef WITH_OPENAL_EXT
	// Allow spatialization for stereo sources
	if (hasExtension(ALExtensions::SOFT_SOURCE_SPATIALIZE))
	{
		for (ALuint source : sources_)
			alSourcei(source, AL_SOURCE_SPATIALIZE_SOFT, AL_TRUE);
	}
#endif
}

ALAudioDevice::~ALAudioDevice()
{
	for (ALuint sourceId : sources_)
		alSourcei(sourceId, AL_BUFFER, AL_NONE);
	alDeleteSources(sources_.size(), sources_.data());
	sources_.clear();

	alcDestroyContext(context_);

	ALCboolean result = alcCloseDevice(device_);
	FATAL_ASSERT_MSG_X(result, "alcCloseDevice failed: %d", alGetError());
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool ALAudioDevice::hasExtension(ALExtensions::Enum extensionName) const
{
	bool extensionAvailable = false;

	if (extensionName >= 0 && extensionName < ALExtensions::COUNT)
		extensionAvailable = alExtensions_[extensionName];

	return extensionAvailable;
}

void ALAudioDevice::setGain(ALfloat gain)
{
	gain_ = gain;
	alListenerf(AL_GAIN, gain_);
}

void ALAudioDevice::setPosition(const Vector3f &position)
{
	position_ = position;
	alListenerfv(AL_POSITION, position_.data());
}

void ALAudioDevice::setPosition(float x, float y, float z)
{
	position_.set(x, y, z);
	alListenerfv(AL_POSITION, position_.data());
}

void ALAudioDevice::setVelocity(const Vector3f &velocity)
{
	velocity_ = velocity;
	alListenerfv(AL_VELOCITY, velocity_.data());
}

void ALAudioDevice::setVelocity(float x, float y, float z)
{
	velocity_.set(x, y, z);
	alListenerfv(AL_VELOCITY, velocity_.data());
}

const IAudioPlayer *ALAudioDevice::player(unsigned int index) const
{
	if (index < players_.size())
		return players_[index];

	return nullptr;
}

IAudioPlayer *ALAudioDevice::player(unsigned int index)
{
	if (index < players_.size())
		return players_[index];

	return nullptr;
}

void ALAudioDevice::pausePlayers()
{
	for (unsigned int i = 0; i < players_.size(); i++)
	{
		if (players_[i]->isPlaying())
		{
			players_[i]->pause();
			pausedPlayers_.insert(players_[i]);
		}
	}
}

void ALAudioDevice::stopPlayers()
{
	for (unsigned int i = 0; i < players_.size(); i++)
		players_[i]->stop();
}

void ALAudioDevice::pausePlayers(PlayerType playerType)
{
	const Object::ObjectType objectType = (playerType == PlayerType::BUFFER)
	                                          ? AudioBufferPlayer::sType()
	                                          : AudioStreamPlayer::sType();

	for (int i = players_.size() - 1; i >= 0; i--)
	{
		if (players_[i]->type() == objectType)
		{
			players_[i]->pause();
			pausedPlayers_.insert(players_[i]);
		}
	}
}

void ALAudioDevice::stopPlayers(PlayerType playerType)
{
	const Object::ObjectType objectType = (playerType == PlayerType::BUFFER)
	                                          ? AudioBufferPlayer::sType()
	                                          : AudioStreamPlayer::sType();

	for (int i = players_.size() - 1; i >= 0; i--)
	{
		if (players_[i]->type() == objectType)
			players_[i]->stop();
	}
}

void ALAudioDevice::resumePlayers()
{
	for (IAudioPlayer *player : pausedPlayers_)
	{
		if (player->isPaused())
			player->play();
	}
	pausedPlayers_.clear();
}

void ALAudioDevice::pauseDevice()
{
#ifdef WITH_OPENAL_EXT
	if (hasExtension(ALExtensions::SOFT_PAUSE_DEVICE))
		alcDevicePauseSOFT(device_);
	else
#endif
		pausePlayers();
}

void ALAudioDevice::resumeDevice()
{
#ifdef WITH_OPENAL_EXT
	if (hasExtension(ALExtensions::SOFT_PAUSE_DEVICE))
		alcDeviceResumeSOFT(device_);
	else
#endif
		resumePlayers();
}

void ALAudioDevice::registerPlayer(IAudioPlayer *player)
{
	ASSERT(player);
	ASSERT(players_.size() == sources_.size() - sourcesPool_.size());
	ASSERT(player->sourceId_ == InvalidSource);

	if (player == nullptr || player->sourceId_ != InvalidSource)
		return;

	if (sourcesPool_.isEmpty() || players_.size() == sources_.size())
	{
		player->sourceId_ = InvalidSource;
		return;
	}

	const ALuint sourceId = sourcesPool_.back();
	sourcesPool_.popBack();

	player->sourceId_ = sourceId;
	players_.pushBack(player);

#ifdef WITH_OPENAL_EXT
	// Apply all source properties for a player at the same time
	hasExtension(ALExtensions::SOFT_DEFERRED_UPDATES) ? alDeferUpdatesSOFT() : alcSuspendContext(context_);
#endif
	player->applySourceProperties();
#ifdef WITH_OPENAL_EXT
	hasExtension(ALExtensions::SOFT_DEFERRED_UPDATES) ? alProcessUpdatesSOFT() : alcProcessContext(context_);
#endif
}

void ALAudioDevice::unregisterPlayer(IAudioPlayer *player)
{
	ASSERT(player);
	ASSERT(players_.size() == sources_.size() - sourcesPool_.size());
	ASSERT(player->sourceId_ != InvalidSource);

	if (player == nullptr || player->sourceId_ == InvalidSource)
		return;

	for (unsigned int i = 0; i < players_.size(); i++)
	{
		if (players_[i] == player)
		{
			players_.unorderedRemoveAt(i);
			break;
		}
	}

	sourcesPool_.pushBack(player->sourceId_);
	player->sourceId_ = InvalidSource;
}

void ALAudioDevice::updatePlayers()
{
	for (unsigned int i = 0; i < players_.size(); i++)
	{
		IAudioPlayer *player = players_[i];
		// Stopped players are unregistered and removed from the array
		ASSERT(player->isSourceLocked() || player->isPlaying() || player->isPaused());

		if (player->isPlaying())
			player->updateState();
	}

	// Players that have just stopped are unregistered (in reverse order)
	for (int i = players_.size() - 1; i >= 0; i--)
	{
		IAudioPlayer *player = players_[i];

		if (player->isStopped() && player->isSourceLocked() == false)
			unregisterPlayer(player);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ALAudioDevice::retrieveAttributes()
{
	attributes_.deviceName = alcGetString(device_, ALC_DEVICE_SPECIFIER);

	ALCint attributesSize = 0;
	alcGetIntegerv(device_, ALC_ATTRIBUTES_SIZE, 1, &attributesSize);
	if (attributesSize > 0)
	{
		const unsigned int MaxAttributes = 16;
		ALCint attributes[MaxAttributes * 2];
		const ALCint numAttributes = (attributesSize < MaxAttributes * 2) ? attributesSize : MaxAttributes * 2;

		alcGetIntegerv(device_, ALC_ALL_ATTRIBUTES, numAttributes, attributes);

		for (int i = 0; i + 1 < numAttributes; i += 2)
		{
			switch (attributes[i])
			{
				case ALC_MAJOR_VERSION:
					attributes_.majorVersion = attributes[i + 1];
					break;
				case ALC_MINOR_VERSION:
					attributes_.minorVersion = attributes[i + 1];
					break;
				case ALC_FREQUENCY:
					attributes_.outputFrequency = attributes[i + 1];
					break;
				case ALC_MONO_SOURCES:
					attributes_.numMonoSources = attributes[i + 1];
					break;
				case ALC_STEREO_SOURCES:
					attributes_.numStereoSources = attributes[i + 1];
					break;
				case ALC_REFRESH:
					attributes_.refreshRate = attributes[i + 1];
					break;
				case ALC_SYNC:
					attributes_.synchronous = attributes[i + 1];
					break;
			}
		}
	}
}

void ALAudioDevice::retrieveExtensions()
{
#ifdef WITH_OPENAL_EXT
	for (unsigned int i = 0; i < ALExtensions::COUNT; i++)
	{
		// Determining if an extension is relative to a context (ALC_) or not (AL_)
		if (ExtensionNames[i][2] == 'C')
			alExtensions_[i] = (alcIsExtensionPresent(device_, ExtensionNames[i]) == ALC_TRUE);
		else
			alExtensions_[i] = (alIsExtensionPresent(ExtensionNames[i]) == ALC_TRUE);
	}
	hasEfxExtension_ = alExtensions_[ALExtensions::EXT_EFX]; // cached value

	if (hasExtension(ALExtensions::EXT_EFX))
	{
		retrieveEfxFunctions();
		alcGetIntegerv(device_, ALC_EFX_MAJOR_VERSION, 1, &attributes_.efxMajorVersion);
		alcGetIntegerv(device_, ALC_EFX_MINOR_VERSION, 1, &attributes_.efxMinorVersion);
		alcGetIntegerv(device_, ALC_MAX_AUXILIARY_SENDS, 1, &attributes_.maxAuxiliarySends);
	}

	if (hasExtension(ALExtensions::SOFT_PAUSE_DEVICE))
		retrievePauseDeviceFunctions();

	if (hasExtension(ALExtensions::SOFT_DEFERRED_UPDATES))
		retrieveDeferredUpdatesFunctions();
#endif
}

void ALAudioDevice::logALAttributes()
{
	LOGI("--- OpenAL attributes ---");
	LOGI_X("Device name: %s", attributes_.deviceName);
	LOGI_X("Version: %d.%d", attributes_.majorVersion, attributes_.minorVersion);
	LOGI_X("Frequency: %d", attributes_.outputFrequency);
	LOGI_X("Mono sources: %d", attributes_.numMonoSources);
	LOGI_X("Stereo sources: %d", attributes_.numStereoSources);
	LOGI_X("Refresh rate: %d", attributes_.refreshRate);
	LOGI_X("Synchronous: %s", attributes_.synchronous ? "true" : "false");
	LOGI("--- OpenAL attributes ---");
}

void ALAudioDevice::logALExtensions()
{
	LOGI("--- OpenAL extensions ---");
	for (unsigned int i = 0; i < ALExtensions::COUNT; i++)
		LOGI_X("%s: %d", ExtensionNames[i], alExtensions_[i]);

	if (hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
	{
		LOGI_X("EFX version: %d.%d", attributes_.efxMajorVersion, attributes_.efxMinorVersion);
		LOGI_X("Max auxiliary sends: %d", attributes_.maxAuxiliarySends);
	}
	LOGI("--- OpenAL extensions ---");
}

}
