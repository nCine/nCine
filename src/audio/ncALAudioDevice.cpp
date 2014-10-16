#include <cstdlib> // for exit()
#include "ncALAudioDevice.h"
#include "ncAudioBufferPlayer.h"
#include "ncAudioStreamPlayer.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncALAudioDevice::ncALAudioDevice()
	: device_(NULL), context_(NULL), gain_(1.0f)
{
	device_ = alcOpenDevice(NULL);
	if (device_ == NULL)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcOpenDevice failed: %x", alGetError());
		exit(EXIT_FAILURE);
	}

	context_ = alcCreateContext(device_, NULL);
	if (context_ == NULL)
	{
		alcCloseDevice(device_);
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcCreateContext failed: %x", alGetError());
		exit(EXIT_FAILURE);
	}

	if (!alcMakeContextCurrent(context_))
	{
		alcDestroyContext(context_);
		alcCloseDevice(device_);
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcMakeContextCurrent failed: %x", alGetError());
		exit(EXIT_FAILURE);
	}

	alGenSources(MaxSources, sources_);
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerf(AL_GAIN, gain_);
}

ncALAudioDevice::~ncALAudioDevice()
{
	for (unsigned int i = 0; i < MaxSources; i++)
	{
		alSourcei(sources_[i], AL_BUFFER, AL_NONE);
	}
	alDeleteSources(MaxSources, sources_);

	alcDestroyContext(context_);

	ALCboolean result = alcCloseDevice(device_);
	if (!result)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::~ncALAudioDevice - alcCloseDevice failed: %d", alGetError());
		exit(EXIT_FAILURE);
	}
}


///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncALAudioDevice::setGain(ALfloat gain)
{
	gain_ = gain;
	alListenerf(AL_GAIN, gain_);
}

void ncALAudioDevice::stopPlayers()
{
	for (ncList<ncIAudioPlayer *>::Const_Iterator i = players_.begin(); i != players_.end(); ++i)
	{
		(*i)->stop();
	}

	players_.clear();
}

void ncALAudioDevice::pausePlayers()
{
	for (ncList<ncIAudioPlayer *>::Const_Iterator i = players_.begin(); i != players_.end(); ++i)
	{
		(*i)->pause();
	}

	players_.clear();
}

int ncALAudioDevice::nextAvailableSource()
{
	ALint sourceState;

	for (unsigned int i = 0; i < MaxSources; i++)
	{
		alGetSourcei(sources_[i], AL_SOURCE_STATE, &sourceState);
		if (sourceState != AL_PLAYING && sourceState != AL_PAUSED)
		{
			return sources_[i];
		}
	}

	return -1;
}

void ncALAudioDevice::registerPlayer(ncIAudioPlayer *player)
{
	players_.insertBack(player);
}

void ncALAudioDevice::updatePlayers()
{
	ncList<ncIAudioPlayer *>::Const_Iterator i = players_.begin();
	while (i != players_.end())
	{
		if ((*i)->isPlaying())
		{
			(*i)->updateState();
			++i;
		}
		else
		{
			players_.remove(i++);
		}
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Stops or pauses all buffer players
void ncALAudioDevice::stopOrPauseBufferPlayers(bool shouldStop)
{
	ncList<ncIAudioPlayer *>::Const_Iterator i = players_.begin();
	while (i != players_.end())
	{
		if ((*i)->type() == ncAudioBufferPlayer::sType())
		{
			if (shouldStop)
			{
				(*i)->stop();
			}
			else
			{
				(*i)->pause();
			}
			players_.remove(i++);
		}
		else
		{
			++i;
		}
	}
}

/// Stops or pauses all stream players
void ncALAudioDevice::stopOrPauseStreamPlayers(bool shouldStop)
{
	ncList<ncIAudioPlayer *>::Const_Iterator i = players_.begin();
	while (i != players_.end())
	{
		if ((*i)->type() == ncAudioStreamPlayer::sType())
		{
			if (shouldStop)
			{
				(*i)->stop();
			}
			else
			{
				(*i)->pause();
			}
			players_.remove(i++);
		}
		else
		{
			i++;
		}
	}
}
