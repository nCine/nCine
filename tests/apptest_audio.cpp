#include "apptest_audio.h"
#include "Application.h"
#include "AudioBuffer.h"
#include "AudioBufferPlayer.h"
#include "AudioStream.h"
#include "AudioStreamPlayer.h"
#include "TextNode.h"
#include "apptest_datapath.h"

namespace {

const float DefaultGain = 1.0f;
const float DefaultPitch = 1.0f;
const float DefaultXPos = 0.0f;
const float VerticalTextPos = 0.45f;

#ifdef __ANDROID__
	const char *FontTextureFile = "DroidSans32_256_8888.ktx";
#else
	const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

}

nc::IAppEventHandler *createAppEventHandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
}

void MyEventHandler::onInit()
{
	gain_ = DefaultGain;
	pitch_ = DefaultPitch;
	xPos_ = DefaultXPos;
	isLooping_ = true;

	font_ = new nc::Font((nc::IFile::dataPath() + "fonts/" + FontTextureFile).data(),
	                     (nc::IFile::dataPath() + "fonts/" + FontFntFile).data());
	musicPlayer_ = new nc::AudioStreamPlayer((nc::IFile::dataPath() + "sounds/music.ogg").data());
	audioBuffer_ = new nc::AudioBuffer((nc::IFile::dataPath() + "sounds/bomb.wav").data());
	soundPlayer_ = new nc::AudioBufferPlayer(audioBuffer_);
	soundPlayer_->play();

	nc::SceneNode &rootNode = nc::theApplication().rootNode();
	dummy_ = new nc::SceneNode(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	textNode_ = new nc::TextNode(dummy_, font_);
	textNode_->setAlignment(nc::TextNode::ALIGN_LEFT);
	textString_ = new nc::String(256);
}

void MyEventHandler::onFrameStart()
{
	soundPlayer_->setGain(gain_);
	soundPlayer_->setPitch(pitch_);
	soundPlayer_->setPosition(xPos_, 0.0f, 0.0f);
	soundPlayer_->setLooping(isLooping_);

	textString_->clear();

	textString_->append("Music: ");
	switch (musicPlayer_->state())
	{
		case nc::IAudioPlayer::STATE_PLAYING:
			textString_->append("playing");
			break;
		case nc::IAudioPlayer::STATE_PAUSED:
			textString_->append("paused");
			break;
		case nc::IAudioPlayer::STATE_STOPPED:
			textString_->append("stopped");
			break;
		default:
			break;

	}
	textString_->append(" (press M)\n");

	textString_->append("Sound: ");
	switch (soundPlayer_->state())
	{
		case nc::IAudioPlayer::STATE_PLAYING:
			textString_->append("playing");
			break;
		case nc::IAudioPlayer::STATE_PAUSED:
			textString_->append("paused");
			break;
		case nc::IAudioPlayer::STATE_STOPPED:
			textString_->append("stopped");
			break;
		default:
			break;

	}
	textString_->append(" (press A/S/D)\n");

	if (soundPlayer_->isLooping())
	{
		textString_->append("Sound is looping");
	}
	else
	{
		textString_->append("Sound is not looping");
	}
	textString_->append(" (press L)\n");

	textString_->formatAppend("Gain: %.2f (press KP 7/8/9)\n", gain_);
	textString_->formatAppend("Pitch: %.2f (press KP 4/5/6)\n", pitch_);
	textString_->formatAppend("Position: %.2f (press KP 1/2/3)\n", xPos_);

	textNode_->setString(*textString_);
	textNode_->setPosition(0.0f, nc::theApplication().height() * VerticalTextPos - textNode_->height() * 0.5f);
}

void MyEventHandler::onShutdown()
{
	delete textString_;
	delete textNode_;
	delete dummy_;
	delete musicPlayer_;
	delete soundPlayer_;
	delete audioBuffer_;
}


#ifdef __ANDROID__
void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	if (event.pointers[0].x < nc::theApplication().width() * 0.5f)
	{
		toggleMusic();
	}
	else
	{
		toggleSound();
	}
}
#else
void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KEY_ESCAPE || event.sym == nc::KEY_Q)
	{
		nc::theApplication().quit();
	}
	else if (event.sym == nc::KEY_M)
	{
		toggleMusic();
	}
	else if (event.sym == nc::KEY_SPACE)
	{
		toggleSound();
	}
	else if (event.sym == nc::KEY_A)
	{
		soundPlayer_->play();
	}
	else if (event.sym == nc::KEY_S)
	{
		soundPlayer_->stop();
	}
	else if (event.sym == nc::KEY_D)
	{
		soundPlayer_->pause();
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

void MyEventHandler::toggleMusic()
{
	if (musicPlayer_->isPaused() || musicPlayer_->isStopped())
	{
		musicPlayer_->play();
	}
	else if (musicPlayer_->isPlaying())
	{
		musicPlayer_->pause();
	}
}

void MyEventHandler::toggleSound()
{
	if (soundPlayer_->isPaused() || soundPlayer_->isStopped())
	{
		soundPlayer_->play();
	}
	else if (soundPlayer_->isPlaying())
	{
		soundPlayer_->pause();
	}
}
