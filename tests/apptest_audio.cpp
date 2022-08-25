#include <ncine/config.h>

#include "apptest_audio.h"
#include <ncine/Application.h>
#include <ncine/AudioBuffer.h>
#include <ncine/AudioBufferPlayer.h>
#include <ncine/AudioStream.h>
#include <ncine/AudioStreamPlayer.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

#if NCINE_WITH_IMGUI
	#include <ncine/imgui.h>
#endif

namespace {

const float DefaultGain = 1.0f;
const float DefaultPitch = 1.0f;
const float DefaultXPos = 0.0f;
const float MaxPitch = 3.0f;
const float MaxXPos = 10.0f;
const float VerticalTextPos = 0.45f;

const unsigned int NumMusicFiles = 2;
const char *MusicFiles[NumMusicFiles] = { "c64.ogg", "chiptune_loop.ogg" };
const unsigned int NumSoundFiles = 3;
const char *SoundFiles[NumSoundFiles] = { "coins.wav", "explode.wav", "waterdrop.wav" };
int selectedMusicFile = 0;
int selectedSoundFile = 0;

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

#if NCINE_WITH_IMGUI
bool showImGui = true;
#endif

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
	soundIsLooping_ = false;
	musicIsLooping_ = false;

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());
	musicPlayer_ = nctl::makeUnique<nc::AudioStreamPlayer>(prefixDataPath("sounds", MusicFiles[0]).data());
	audioBuffer_ = nctl::makeUnique<nc::AudioBuffer>(prefixDataPath("sounds", SoundFiles[0]).data());
	soundPlayer_ = nctl::makeUnique<nc::AudioBufferPlayer>(audioBuffer_.get());
	soundPlayer_->play();

	nc::SceneNode &rootNode = nc::theApplication().rootNode();
	dummy_ = nctl::makeUnique<nc::SceneNode>(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	textNode_ = nctl::makeUnique<nc::TextNode>(dummy_.get(), font_.get(), MaxStringLength);
	textNode_->setAlignment(nc::TextNode::Alignment::LEFT);
	textString_ = nctl::makeUnique<nctl::String>(MaxStringLength);
}

void MyEventHandler::onFrameStart()
{
#if NCINE_WITH_IMGUI
	if (showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(370.0f, 440.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20.0f, 30.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("apptest_audio", &showImGui))
		{
			if (ImGui::TreeNodeEx("Music player", ImGuiTreeNodeFlags_DefaultOpen))
			{
				const nc::AudioStreamPlayer *audioStreamPlayer = static_cast<const nc::AudioStreamPlayer *>(musicPlayer_.get());
				if (ImGui::TreeNode("Audio stream information"))
				{
					ImGui::Text("Bytes per sample: %d", audioStreamPlayer->bytesPerSample());
					ImGui::Text("Number of channels: %d", audioStreamPlayer->numChannels());
					ImGui::Text("Frequency: %d Hz", audioStreamPlayer->frequency());
					ImGui::Text("Number of samples: %d", audioStreamPlayer->numSamples());
					ImGui::Text("Duration: %.3f s", audioStreamPlayer->duration());
					ImGui::Text("Buffer size: %d", audioStreamPlayer->bufferSize());
					ImGui::Text("Number of stream samples: %d", audioStreamPlayer->numSamplesInStreamBuffer());
					ImGui::Text("Stream buffer size: %d", audioStreamPlayer->streamBufferSize());

					ImGui::TreePop();
				}

				if (ImGui::Combo("Music file", &selectedMusicFile, MusicFiles, NumMusicFiles))
					static_cast<nc::AudioStreamPlayer *>(musicPlayer_.get())->loadFromFile(prefixDataPath("sounds", MusicFiles[selectedMusicFile]).data());

				const nc::IAudioPlayer::PlayerState state = musicPlayer_->state();
				const bool isPlaying = (state == nc::IAudioPlayer::PlayerState::PLAYING);
				const bool isStopped = (state == nc::IAudioPlayer::PlayerState::STOPPED);

				const int sampleOffset = musicPlayer_->sampleOffset();
				const int numSamplesInStreamBuffer = audioStreamPlayer->numSamplesInStreamBuffer();
				const float fraction = sampleOffset / static_cast<float>(numSamplesInStreamBuffer);
				ImGui::ProgressBar(fraction, ImVec2(0.0f, 0.0f));
				ImGui::Text("Samples: %d / %d", sampleOffset, numSamplesInStreamBuffer);

				const int sampleOffsetInStream = audioStreamPlayer->sampleOffsetInStream();
				const int numSamples = musicPlayer_->numSamples();
				const float streamFraction = sampleOffsetInStream / static_cast<float>(numSamples);
				ImGui::ProgressBar(streamFraction, ImVec2(0.0f, 0.0f));
				ImGui::Text("Stream samples: %d / %d", sampleOffsetInStream, numSamples);

				ImGui::Text("State: %s", audioPlayerStateToString(state));

				ImGui::BeginDisabled(isPlaying);
				if (ImGui::Button("Play"))
					musicPlayer_->play();
				ImGui::EndDisabled();

				ImGui::SameLine();
				ImGui::BeginDisabled(isPlaying == false);
				if (ImGui::Button("Pause"))
					musicPlayer_->pause();
				ImGui::EndDisabled();

				ImGui::SameLine();
				ImGui::BeginDisabled(isStopped);
				if (ImGui::Button("Stop"))
					musicPlayer_->stop();
				ImGui::EndDisabled();

				ImGui::SameLine();
				ImGui::Checkbox("Looping", &musicIsLooping_);

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Sound player", ImGuiTreeNodeFlags_DefaultOpen))
			{
				const nc::AudioBuffer *audioBuffer = static_cast<const nc::AudioBufferPlayer *>(soundPlayer_.get())->audioBuffer();
				if (audioBuffer)
				{
					if (ImGui::TreeNode("Audio buffer information"))
					{
						ImGui::Text("Bytes per sample: %d", audioBuffer->bytesPerSample());
						ImGui::Text("Number of channels: %d", audioBuffer->numChannels());
						ImGui::Text("Frequency: %d Hz", audioBuffer->frequency());
						ImGui::Text("Number of samples: %d", audioBuffer->numSamples());
						ImGui::Text("Duration: %.3f s", audioBuffer->duration());
						ImGui::Text("Buffer size: %d", audioBuffer->bufferSize());

						ImGui::TreePop();
					}
				}

				if (ImGui::Combo("Sound file", &selectedSoundFile, SoundFiles, NumSoundFiles))
				{
					soundPlayer_->stop();
					audioBuffer_->loadFromFile(prefixDataPath("sounds", SoundFiles[selectedSoundFile]).data());
				}

				const nc::IAudioPlayer::PlayerState state = soundPlayer_->state();
				const bool isPlaying = (state == nc::IAudioPlayer::PlayerState::PLAYING);
				const bool isStopped = (state == nc::IAudioPlayer::PlayerState::STOPPED);

				const int sampleOffset = soundPlayer_->sampleOffset();
				const int numSamples = soundPlayer_->numSamples();
				const float fraction = sampleOffset / static_cast<float>(numSamples);
				ImGui::ProgressBar(fraction, ImVec2(0.0f, 0.0f));
				ImGui::Text("Samples: %d / %d", sampleOffset, numSamples);

				ImGui::Text("State: %s", audioPlayerStateToString(state));

				ImGui::BeginDisabled(isPlaying);
				if (ImGui::Button("Play"))
					soundPlayer_->play();
				ImGui::EndDisabled();

				ImGui::SameLine();
				ImGui::BeginDisabled(isPlaying == false);
				if (ImGui::Button("Pause"))
					soundPlayer_->pause();
				ImGui::EndDisabled();

				ImGui::SameLine();
				ImGui::BeginDisabled(isStopped);
				if (ImGui::Button("Stop"))
					soundPlayer_->stop();
				ImGui::EndDisabled();

				ImGui::SameLine();
				ImGui::Checkbox("Looping", &soundIsLooping_);

				ImGui::SliderFloat("Gain", &gain_, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::SliderFloat("Pitch", &pitch_, 0.0f, MaxPitch, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::SliderFloat("X Position", &xPos_, -MaxXPos, MaxXPos, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				if (ImGui::Button("Reset"))
					resetValues();

				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
#endif

	soundPlayer_->setGain(gain_);
	soundPlayer_->setPitch(pitch_);
	soundPlayer_->setPosition(xPos_, 0.0f, 0.0f);
	soundPlayer_->setLooping(soundIsLooping_);
	musicPlayer_->setLooping(musicIsLooping_);

	textString_->clear();

	const float musicSeekPosition = (100.0f * static_cast<nc::AudioStreamPlayer *>(musicPlayer_.get())->sampleOffsetInStream()) / musicPlayer_->numSamples();
	const float soundSeekPosition = (100.0f * soundPlayer_->sampleOffset()) / soundPlayer_->numSamples();
	textString_->formatAppend("Music: \"%s\" (%d of %u)\nSample seek position: %.0f%%\nState: %s (press M)\n\n",
	                          MusicFiles[selectedMusicFile], selectedMusicFile + 1, NumMusicFiles, musicSeekPosition, audioPlayerStateToString(musicPlayer_->state()));
	textString_->formatAppend("Sound: \"%s\" (%d of %u)\nSample seek position: %.0f%%\nState: %s (press A/S/D)\n\n",
	                          SoundFiles[selectedSoundFile], selectedSoundFile + 1, NumSoundFiles, soundSeekPosition, audioPlayerStateToString(soundPlayer_->state()));

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
		soundIsLooping_ = !soundIsLooping_;
	else if (event.sym == nc::KeySym::KP0)
		resetValues();
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
		pitch_ -= MaxPitch * 0.1f;
	else if (event.sym == nc::KeySym::KP5)
		pitch_ = DefaultPitch;
	else if (event.sym == nc::KeySym::KP6)
		pitch_ += MaxPitch * 0.1f;
	else if (event.sym == nc::KeySym::KP1)
		xPos_ -= MaxXPos * 0.1f;
	else if (event.sym == nc::KeySym::KP2)
		xPos_ = DefaultXPos;
	else if (event.sym == nc::KeySym::KP3)
		xPos_ += MaxXPos * 0.1f;
	else if (event.sym == nc::KeySym::LEFT)
		loadPrevMusic();
	else if (event.sym == nc::KeySym::RIGHT)
		loadNextMusic();
	else if (event.sym == nc::KeySym::UP)
		loadPrevSound();
	else if (event.sym == nc::KeySym::DOWN)
		loadNextSound();
#if NCINE_WITH_IMGUI
	else if (event.mod & nc::KeyMod::CTRL && event.sym == nc::KeySym::H)
		showImGui = !showImGui;
#endif
	else if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}

void MyEventHandler::onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event)
{
	if (event.axisName == nc::AxisName::LX)
		xPos_ = MaxXPos * event.value;
	else if (event.axisName == nc::AxisName::LTRIGGER)
		gain_ = 1.0f - event.value;
	else if (event.axisName == nc::AxisName::RTRIGGER)
		pitch_ = DefaultPitch + (MaxPitch - DefaultPitch) * event.value;
}

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	if (event.buttonName == nc::ButtonName::A)
		toggleMusic();
	else if (event.buttonName == nc::ButtonName::B)
		toggleSound();
	else if (event.buttonName == nc::ButtonName::X)
	{
		musicPlayer_->stop();
		soundPlayer_->stop();
	}
	else if (event.buttonName == nc::ButtonName::Y)
		resetValues();
	else if (event.buttonName == nc::ButtonName::LBUMPER)
		loadNextMusic();
	else if (event.buttonName == nc::ButtonName::RBUMPER)
		loadNextSound();
	else if (event.buttonName == nc::ButtonName::DPAD_LEFT)
		loadPrevMusic();
	else if (event.buttonName == nc::ButtonName::DPAD_RIGHT)
		loadNextMusic();
	else if (event.buttonName == nc::ButtonName::DPAD_UP)
		loadPrevSound();
	else if (event.buttonName == nc::ButtonName::DPAD_DOWN)
		loadNextSound();
	else if (event.buttonName == nc::ButtonName::GUIDE)
		nc::theApplication().quit();
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	resetValues();
}

void MyEventHandler::resetValues()
{
	gain_ = DefaultGain;
	pitch_ = DefaultPitch;
	xPos_ = DefaultXPos;
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

void MyEventHandler::loadNextMusic()
{
	selectedMusicFile++;
	if (selectedMusicFile > NumMusicFiles - 1)
		selectedMusicFile = 0;

	static_cast<nc::AudioStreamPlayer *>(musicPlayer_.get())->loadFromFile(prefixDataPath("sounds", MusicFiles[selectedMusicFile]).data());
}

void MyEventHandler::loadPrevMusic()
{
	selectedMusicFile--;
	if (selectedMusicFile < 0)
		selectedMusicFile = NumMusicFiles - 1;

	static_cast<nc::AudioStreamPlayer *>(musicPlayer_.get())->loadFromFile(prefixDataPath("sounds", MusicFiles[selectedMusicFile]).data());
}

void MyEventHandler::loadNextSound()
{
	selectedSoundFile++;
	if (selectedSoundFile > NumSoundFiles - 1)
		selectedSoundFile = 0;

	soundPlayer_->stop();
	audioBuffer_->loadFromFile(prefixDataPath("sounds", SoundFiles[selectedSoundFile]).data());
}

void MyEventHandler::loadPrevSound()
{
	selectedSoundFile--;
	if (selectedSoundFile < 0)
		selectedSoundFile = NumSoundFiles - 1;

	soundPlayer_->stop();
	audioBuffer_->loadFromFile(prefixDataPath("sounds", SoundFiles[selectedSoundFile]).data());
}
