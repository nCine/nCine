#include "apptest_audio.h"
#include "Application.h"
#include "AudioBuffer.h"
#include "AudioBufferPlayer.h"
#include "AudioStream.h"
#include "AudioStreamPlayer.h"

#define WITH_STREAMING (1)

const float MyEventHandler::DefaultGain = 1.0f;
const float MyEventHandler::DefaultPitch = 1.0f;
const float MyEventHandler::DefaultXPos = 0.0f;

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	gain_ = DefaultGain;
	pitch_ = DefaultPitch;
	xPos_ = DefaultXPos;
	isLooping_ = true;

#if WITH_STREAMING
	#ifdef __ANDROID__
	player_ = new nc::AudioStreamPlayer((nc::IFile::dataPath() + "bomb.ogg").data());
	#else
	player_ = new nc::AudioStreamPlayer((nc::IFile::dataPath() + "sounds/bomb.ogg").data());
	#endif
#else
	#ifdef __ANDROID__
	audioBuffer_ = new nc::AudioBuffer((nc::IFile::dataPath() + "bomb.wav").data());
	#else
	audioBuffer_ = new nc::AudioBuffer((nc::IFile::dataPath() + "sounds/bomb.wav").data());
	#endif
	player_ = new nc::AudioBufferPlayer(audioBuffer_);
#endif
	player_->play();
}

void MyEventHandler::onFrameStart()
{
	player_->setGain(gain_);
	player_->setPitch(pitch_);
	player_->setPosition(xPos_, 0.0f, 0.0f);
	player_->setLooping(isLooping_);
}

void MyEventHandler::onShutdown()
{
	delete player_;
#if !(WITH_STREAMING)
	delete audioBuffer_;
#endif
}

#ifndef __ANDROID__
void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KEY_ESCAPE || event.sym == nc::KEY_Q)
	{
		nc::theApplication().quit();
	}
	else if (event.sym == nc::KEY_SPACE)
	{
		if (player_->isPaused() || player_->isStopped())
		{
			player_->play();
		}
		else if (player_->isPlaying())
		{
			player_->pause();
		}
	}
	else if (event.sym == nc::KEY_A)
	{
		player_->play();
	}
	else if (event.sym == nc::KEY_S)
	{
		player_->stop();
	}
	else if (event.sym == nc::KEY_D)
	{
		player_->pause();
	}
	else if (event.sym == nc::KEY_L)
	{
		isLooping_ = !isLooping_;
	}
	else if (event.sym == nc::KEY_KP0)
	{
		gain_ = DefaultGain;
		pitch_ = DefaultPitch;
		xPos_ = DefaultXPos;
	}
	else if (event.sym == nc::KEY_KP0)
	{
	}
	else if (event.sym == nc::KEY_KP7)
	{
		gain_ -= 0.1f;
		if (gain_ < 0.0f)
		{
			gain_ = 0.0f;
		}
	}
	else if (event.sym == nc::KEY_KP8)
	{
		gain_ = DefaultGain;
	}
	else if (event.sym == nc::KEY_KP9)
	{
		gain_ += 0.1f;
		if (gain_ > 1.0f)
		{
			gain_ = 1.0f;
		}
	}
	else if (event.sym == nc::KEY_KP4)
	{
		pitch_ -= 0.1f;
	}
	else if (event.sym == nc::KEY_KP5)
	{
		pitch_ = DefaultPitch;
	}
	else if (event.sym == nc::KEY_KP6)
	{
		pitch_ += 0.1f;
	}
	else if (event.sym == nc::KEY_KP1)
	{
		xPos_ -= 0.1f;
	}
	else if (event.sym == nc::KEY_KP2)
	{
		xPos_ = DefaultXPos;
	}
	else if (event.sym == nc::KEY_KP3)
	{
		xPos_ += 0.1f;
	}
}
#endif
