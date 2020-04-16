#include "apptest_audio.h"
#include <ncine/Application.h>
#include <ncine/AudioBuffer.h>
#include <ncine/AudioBufferPlayer.h>
#include <ncine/AudioStream.h>
#include <ncine/AudioStreamPlayer.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

namespace {

const float DefaultGain = 1.0f;
const float DefaultPitch = 1.0f;
const float DefaultXPos = 0.0f;
const float VerticalTextPos = 0.45f;

#ifdef __ANDROID__
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const char *audioPlayerStateToString(nc::IAudioPlayer::PlayerState state)
{
	switch (state)
	{
		case nc::IAudioPlayer::PlayerState::INITIAL: return "Initial";
		case nc::IAudioPlayer::PlayerState::PLAYING: return "Playing";
		case nc::IAudioPlayer::PlayerState::PAUSED: return "Paused";
		case nc::IAudioPlayer::PlayerState::STOPPED: return "Stopped";
	}

	return "Unknown";
}

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
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
	isLooping_ = false;

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());
	musicPlayer_ = nctl::makeUnique<nc::AudioStreamPlayer>(prefixDataPath("sounds", "music.ogg").data());
	audioBuffer_ = nctl::makeUnique<nc::AudioBuffer>(prefixDataPath("sounds", "bomb.wav").data());
	soundPlayer_ = nctl::makeUnique<nc::AudioBufferPlayer>(audioBuffer_.get());
	soundPlayer_->play();

	nc::SceneNode &rootNode = nc::theApplication().rootNode();
	dummy_ = nctl::makeUnique<nc::SceneNode>(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	textNode_ = nctl::makeUnique<nc::TextNode>(dummy_.get(), font_.get());
	textNode_->setAlignment(nc::TextNode::Alignment::LEFT);
	textString_ = nctl::makeUnique<nctl::String>(256);
}

void MyEventHandler::onFrameStart()
{
	soundPlayer_->setGain(gain_);
	soundPlayer_->setPitch(pitch_);
	soundPlayer_->setPosition(xPos_, 0.0f, 0.0f);
	soundPlayer_->setLooping(isLooping_);

	textString_->clear();

	textString_->formatAppend("Music: %s (press M)\n", audioPlayerStateToString(musicPlayer_->state()));
	textString_->formatAppend("Sound: %s (press A/S/D)\n", audioPlayerStateToString(soundPlayer_->state()));

	if (soundPlayer_->isLooping())
		textString_->append("Sound is looping");
	else
		textString_->append("Sound is not looping");
	textString_->append(" (press L)\n");

	textString_->formatAppend("Gain: %.2f (press KP 7/8/9)\n", gain_);
	textString_->formatAppend("Pitch: %.2f (press KP 4/5/6)\n", pitch_);
	textString_->formatAppend("Position: %.2f (press KP 1/2/3)\n", xPos_);

	textNode_->setString(*textString_);
	textNode_->setPosition(0.0f, nc::theApplication().height() * VerticalTextPos - textNode_->height() * 0.5f);
}

#ifdef __ANDROID__
void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	if (event.pointers[0].x < nc::theApplication().width() * 0.5f)
		toggleMusic();
	else
		toggleSound();
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::M)
		toggleMusic();
	else if (event.sym == nc::KeySym::SPACE)
		toggleSound();
	else if (event.sym == nc::KeySym::A)
		soundPlayer_->play();
	else if (event.sym == nc::KeySym::S)
		soundPlayer_->stop();
	else if (event.sym == nc::KeySym::D)
		soundPlayer_->pause();
	else if (event.sym == nc::KeySym::L)
		isLooping_ = !isLooping_;
	else if (event.sym == nc::KeySym::KP0)
	{
		gain_ = DefaultGain;
		pitch_ = DefaultPitch;
		xPos_ = DefaultXPos;
	}
	else if (event.sym == nc::KeySym::KP7)
	{
		gain_ -= 0.1f;
		if (gain_ < 0.0f)
			gain_ = 0.0f;
	}
	else if (event.sym == nc::KeySym::KP8)
		gain_ = DefaultGain;
	else if (event.sym == nc::KeySym::KP9)
	{
		gain_ += 0.1f;
		if (gain_ > 1.0f)
			gain_ = 1.0f;
	}
	else if (event.sym == nc::KeySym::KP4)
		pitch_ -= 0.1f;
	else if (event.sym == nc::KeySym::KP5)
		pitch_ = DefaultPitch;
	else if (event.sym == nc::KeySym::KP6)
		pitch_ += 0.1f;
	else if (event.sym == nc::KeySym::KP1)
		xPos_ -= 0.1f;
	else if (event.sym == nc::KeySym::KP2)
		xPos_ = DefaultXPos;
	else if (event.sym == nc::KeySym::KP3)
		xPos_ += 0.1f;
	else if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
}

void MyEventHandler::toggleMusic()
{
	if (musicPlayer_->isPaused() || musicPlayer_->isStopped())
		musicPlayer_->play();
	else if (musicPlayer_->isPlaying())
		musicPlayer_->pause();
}

void MyEventHandler::toggleSound()
{
	if (soundPlayer_->isPaused() || soundPlayer_->isStopped())
		soundPlayer_->play();
	else if (soundPlayer_->isPlaying())
		soundPlayer_->pause();
}
