#include <ncine/config.h>

#include "apptest_audio.h"
#include <ncine/Application.h>
#include <ncine/IAudioDevice.h>
#include <ncine/AudioBuffer.h>
#include <ncine/AudioBufferPlayer.h>
#include <ncine/AudioStream.h>
#include <ncine/AudioStreamPlayer.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

#if NCINE_WITH_IMGUI
	#include <ncine/imgui.h>
#endif

#define WITH_STRESS_TEST (1)
#if WITH_STRESS_TEST
	#include <nctl/Array.h>
	#include <ncine/TimeStamp.h>
	#include <ncine/Random.h>
	#include <ncine/IFile.h>
#endif

namespace {

const int MaxStringLength = 384;
const float DefaultXPos = 0.0f;
const float MaxXPos = 10.0f;
const float VerticalTextPos = 0.45f;

const unsigned int NumMusicFiles = 2;
const char *MusicFiles[NumMusicFiles] = { "c64.ogg", "chiptune_loop.ogg" };
const unsigned int NumSoundFiles = 3;
const char *SoundFiles[NumSoundFiles] = { "coins.wav", "explode.wav", "waterdrop.wav" };
int selectedMusicFile = 0;
int selectedSoundFile = 0;
int selectedEffectType = 0;
int selectedFilterType = 0;

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
const ImVec4 Green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
const ImVec4 Red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
bool showImGui = true;

void sourcePropertiesGui(nc::IAudioPlayer &player)
{
	nc::IAudioDevice &device = nc::theServiceLocator().audioDevice();

	const nc::IAudioPlayer::PlayerState state = player.state();
	const bool isPlaying = (state == nc::IAudioPlayer::PlayerState::PLAYING);
	const bool isStopped = (state == nc::IAudioPlayer::PlayerState::STOPPED);

	ImGui::Text("State: %s", audioPlayerStateToString(state));

	ImGui::BeginDisabled(isPlaying);
	if (ImGui::Button("Play"))
		player.play();
	ImGui::EndDisabled();

	ImGui::SameLine();
	ImGui::BeginDisabled(isPlaying == false);
	if (ImGui::Button("Pause"))
		player.pause();
	ImGui::EndDisabled();

	ImGui::SameLine();
	ImGui::BeginDisabled(isStopped);
	if (ImGui::Button("Stop"))
		player.stop();
	ImGui::EndDisabled();

	ImGui::SameLine();
	bool isLooping = player.isLooping();
	ImGui::Checkbox("Looping", &isLooping);
	player.setLooping(isLooping);

	ImGui::SameLine();
	bool sourceLocked = player.isSourceLocked();
	ImGui::Checkbox("Source locked", &sourceLocked);
	player.setSourceLocked(sourceLocked);

	float gain = player.gain();
	ImGui::SliderFloat("Gain", &gain, nc::IAudioPlayer::MinGain, nc::IAudioPlayer::MaxGain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	player.setGain(gain);

	float pitch = player.pitch();
	ImGui::SliderFloat("Pitch", &pitch, nc::IAudioPlayer::MinPitch, nc::IAudioPlayer::MaxPitch, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	player.setPitch(pitch);

	nc::Vector3f position = player.position();
	ImGui::SliderFloat3("Position", position.data(), -MaxXPos, MaxXPos, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	player.setPosition(position);

	nc::Vector3f velocity = player.velocity();
	ImGui::SliderFloat3("Velocity", velocity.data(), -MaxXPos, MaxXPos, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	player.setVelocity(velocity);

	nc::Vector3f direction = player.direction();
	ImGui::SliderFloat3("Direction", direction.data(), -MaxXPos, MaxXPos, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	player.setDirection(direction);

	float coneInnerAngle = player.coneInnerAngle();
	ImGui::SliderFloat("Cone inner angle", &coneInnerAngle, nc::IAudioPlayer::MinConeAngle, nc::IAudioPlayer::MaxConeAngle, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	player.setConeInnerAngle(coneInnerAngle);

	float coneOuterAngle = player.coneOuterAngle();
	ImGui::SliderFloat("Cone outer angle", &coneOuterAngle, nc::IAudioPlayer::MinConeAngle, nc::IAudioPlayer::MaxConeAngle, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	player.setConeOuterAngle(coneOuterAngle);

	float coneOuterGain = player.coneOuterGain();
	ImGui::SliderFloat("Cone outer gain", &coneOuterGain, nc::IAudioPlayer::MinConeOuterGain, nc::IAudioPlayer::MaxConeOuterGain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	player.setConeOuterGain(coneOuterGain);

	if (ImGui::Button("Reset"))
	{
		player.setGain(nc::IAudioPlayer::DefaultGain);
		player.setPitch(nc::IAudioPlayer::DefaultPitch);
		player.setPosition(0.0f, 0.0f, 0.0f);
		player.setVelocity(0.0f, 0.0f, 0.0f);
		player.setDirection(0.0f, 0.0f, 0.0f);
		player.setConeInnerAngle(nc::IAudioPlayer::DefaultConeAngle);
		player.setConeOuterAngle(nc::IAudioPlayer::DefaultConeAngle);
		player.setConeOuterGain(nc::IAudioPlayer::DefaultConeOuterGain);
	}
}
#endif

#if WITH_STRESS_TEST
enum StressModePhases
{
	START,
	SATURATE_MIXED,
	STOP_ALL_1,
	SATURATE_BUFFERS,
	STOP_ALL_2,
	SATURATE_STREAMS,
	STOP_ALL_3,
	RANDOM_ACTION,
	STOP_ALL_4,
	PAUSE_RESUME_STOP_ALL,
	STOP_ALL_5,
	CHANGE_PROPERTIES,
	RESET_PROPERTIES,
	LOCK_UNLOCK,
	END
};

bool stressModeEnabled = false;
StressModePhases stressModePhase = START;
nc::TimeStamp stressModeTimestamp;
const float StressModePhaseDuration = 5.0f;
nc::TimeStamp stressModeDelayTimestamp;
float stressModeActionDelay = 0.001f; // Avoid NaN in ImGui progress bar
const float StressModeActionMinDelay = 0.025f;
const float StressModeActionMaxDelay = 0.05f;
float stressModeTargetPlayPressure = 0.25f;
nctl::Array<nctl::UniquePtr<nc::AudioBuffer>> stressAudioBuffers;
nctl::Array<nctl::UniquePtr<nc::AudioBufferPlayer>> stressAudioBufferPlayers;
nctl::Array<nctl::UniquePtr<nc::AudioStreamPlayer>> stressAudioStreamPlayers;

void toggleStressTest()
{
	stressModePhase = (stressModeEnabled ? StressModePhases::END : StressModePhases::START);
	stressModeTimestamp = nc::TimeStamp::now();
	stressModeActionDelay = nc::random().fastReal(StressModeActionMinDelay, StressModeActionMaxDelay);
	stressModeDelayTimestamp = nc::TimeStamp::now();
	stressModeEnabled = true;
}

void nextStressTestPhase()
{
	stressModePhase = StressModePhases(stressModePhase + 1);
	stressModeTimestamp = nc::TimeStamp::now();
	stressModeActionDelay = nc::random().fastReal(StressModeActionMinDelay, StressModeActionMaxDelay);
	stressModeDelayTimestamp = nc::TimeStamp::now();
}

void stressTestStopAll()
{
	nc::IAudioDevice &device = nc::theServiceLocator().audioDevice();
	for (unsigned int i = 0; i < stressAudioBufferPlayers.size(); i++)
	{
		stressAudioBufferPlayers[i]->stop();
		stressAudioBufferPlayers[i]->setSourceLocked(false);
	}
	for (unsigned int i = 0; i < stressAudioStreamPlayers.size(); i++)
	{
		stressAudioStreamPlayers[i]->stop();
		stressAudioStreamPlayers[i]->setSourceLocked(false);
	}
}

void handleStressTestPhases()
{
	nc::IAudioDevice &device = nc::theServiceLocator().audioDevice();

	if (stressModeTimestamp.secondsSince() >= StressModePhaseDuration)
		nextStressTestPhase();

	if (stressModeDelayTimestamp.secondsSince() < stressModeActionDelay)
		return;
	stressModeDelayTimestamp = nc::TimeStamp::now();
	stressModeActionDelay = nc::random().fastReal(StressModeActionMinDelay, StressModeActionMaxDelay);

	// Search for a non playing buffer player
	nc::IAudioPlayer *randomBufferPlayer = stressAudioBufferPlayers[0].get();
	for (unsigned int i = 0; i < stressAudioBufferPlayers.size(); i++)
	{
		if (stressAudioBufferPlayers[i]->isPlaying() == false)
		{
			randomBufferPlayer = stressAudioBufferPlayers[i].get();
			break;
		}
	}

	// Search for a non playing stream player
	nc::IAudioPlayer *randomStreamPlayer = stressAudioStreamPlayers[0].get();
	for (unsigned int i = 0; i < stressAudioStreamPlayers.size(); i++)
	{
		if (stressAudioStreamPlayers[i]->isPlaying() == false)
		{
			randomStreamPlayer = stressAudioStreamPlayers[i].get();
			break;
		}
	}

	nc::IAudioPlayer *randomPlayer = nc::random().fastInteger(0, 2) ? randomBufferPlayer : randomStreamPlayer;
	nc::IAudioPlayer *randomActivePlayer = device.player(nc::random().fastInteger(0, device.numPlayers()));

	switch (stressModePhase)
	{
		case StressModePhases::START:
		case StressModePhases::STOP_ALL_1:
		case StressModePhases::STOP_ALL_2:
		case StressModePhases::STOP_ALL_3:
		case StressModePhases::STOP_ALL_4:
		case StressModePhases::STOP_ALL_5:
		{
			stressTestStopAll();
			stressModePhase = StressModePhases(stressModePhase + 1);
			break;
		}
		case StressModePhases::SATURATE_MIXED:
		{
			randomPlayer->play();
			break;
		}
		case StressModePhases::SATURATE_BUFFERS:
		{
			randomBufferPlayer->play();
			break;
		}
		case StressModePhases::SATURATE_STREAMS:
		{
			randomStreamPlayer->play();
			break;
		}
		case StressModePhases::RANDOM_ACTION:
		{
			// 0: Play, 1: Pause, 2: Stop
			unsigned int action = 0; // play as the default action
			if (device.numPlayers() >= device.maxNumSources() * stressModeTargetPlayPressure && randomActivePlayer)
				action = nc::random().fastInteger(1, 3); // Don't play if the target play pressure has been hit

			switch (action)
			{
				case 0:
					randomPlayer->play();
					break;
				case 1:
					randomActivePlayer->pause();
					break;
				case 2:
					randomActivePlayer->stop();
					break;
			}
			break;
		}
		case StressModePhases::PAUSE_RESUME_STOP_ALL:
		{
			// 0: Play, 1-3: Pause, 4-6: Stop, 7: Resume
			unsigned int action = 0; // play as the default action
			if (device.numPlayers() >= device.maxNumSources() * stressModeTargetPlayPressure)
				action = nc::random().fastInteger(1, 8); // Don't play if the target play pressure has been hit

			switch (action)
			{
				case 0:
					randomPlayer->play();
					break;
				case 1:
					device.pausePlayers();
					break;
				case 2:
					device.pausePlayers(nc::IAudioDevice::PlayerType::BUFFER);
					break;
				case 3:
					device.pausePlayers(nc::IAudioDevice::PlayerType::STREAM);
					break;
				case 4:
					device.stopPlayers();
					break;
				case 5:
					device.stopPlayers(nc::IAudioDevice::PlayerType::BUFFER);
					break;
				case 6:
					device.stopPlayers(nc::IAudioDevice::PlayerType::STREAM);
					break;
				case 7:
					device.resumePlayers();
					break;
			}
			break;
		}
		case StressModePhases::CHANGE_PROPERTIES:
		{
			// 0: Play, 1: Gain, 2: Pitch, 3: Position, 4: Velocity
			unsigned int action = 0; // play as the default action
			if (device.numPlayers() >= device.maxNumSources() * stressModeTargetPlayPressure && randomActivePlayer)
				action = nc::random().fastInteger(1, 5); // Don't play if the target play pressure has been hit

			switch (action)
			{
				case 0:
					randomPlayer->play();
					break;
				case 1:
				{
					const float gainRange = nc::IAudioPlayer::MaxGain - nc::IAudioPlayer::MinGain;
					const float gain = nc::random().fastReal(gainRange * 0.75f, nc::IAudioPlayer::MaxGain);
					randomActivePlayer->setGain(gain);
					break;
				}
				case 2:
				{
					const float pitch = nc::random().fastReal(nc::IAudioPlayer::MinPitch, nc::IAudioPlayer::MaxPitch);
					randomActivePlayer->setPitch(pitch);
					break;
				}
				case 3:
				{
					const nc::Vector3f position(nc::random().fastReal(), nc::random().fastReal(), nc::random().fastReal());
					randomActivePlayer->setPosition(position);
					break;
				}
				case 4:
				{
					const nc::Vector3f velocity(nc::random().fastReal(), nc::random().fastReal(), nc::random().fastReal());
					randomActivePlayer->setVelocity(velocity);
					break;
				}
			}
			break;
		}
		case StressModePhases::RESET_PROPERTIES:
		{
			for (unsigned int i = 0; i < stressAudioBufferPlayers.size(); i++)
			{
				stressAudioBufferPlayers[i]->setGain(nc::IAudioPlayer::DefaultGain);
				stressAudioBufferPlayers[i]->setPitch(nc::IAudioPlayer::DefaultPitch);
				stressAudioBufferPlayers[i]->setPosition(0.0f, 0.0f, 0.0f);
				stressAudioBufferPlayers[i]->setVelocity(0.0f, 0.0f, 0.0f);
			}
			for (unsigned int i = 0; i < stressAudioStreamPlayers.size(); i++)
			{
				stressAudioStreamPlayers[i]->setGain(nc::IAudioPlayer::DefaultGain);
				stressAudioStreamPlayers[i]->setPitch(nc::IAudioPlayer::DefaultPitch);
				stressAudioStreamPlayers[i]->setPosition(0.0f, 0.0f, 0.0f);
				stressAudioStreamPlayers[i]->setVelocity(0.0f, 0.0f, 0.0f);
			}
			break;
		}
		case StressModePhases::LOCK_UNLOCK:
		{
			// 0: Play, 1:Stop, 2: Lock, 3: Unlock
			unsigned int action = 0; // play as the default action
			if (device.numPlayers() >= device.maxNumSources() * stressModeTargetPlayPressure && randomActivePlayer)
				action = nc::random().fastInteger(1, 4); // Don't play if the target play pressure has been hit

			switch (action)
			{
				case 0:
					randomPlayer->play();
					break;
				case 1:
					randomActivePlayer->stop();
					break;
				case 2:
					randomActivePlayer->setSourceLocked(true);
					break;
				case 3:
					randomPlayer->setSourceLocked(false);
					break;
			}
			break;
		}
		case StressModePhases::END:
		{
			stressTestStopAll();
			stressModePhase = StressModePhases::START;
			stressModeEnabled = false;
			break;
		}
	}
}

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
	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());
	musicPlayer_ = nctl::makeUnique<nc::AudioStreamPlayer>(prefixDataPath("sounds", MusicFiles[0]).data());
	audioBuffer_ = nctl::makeUnique<nc::AudioBuffer>(prefixDataPath("sounds", SoundFiles[0]).data());
	soundPlayer_ = nctl::makeUnique<nc::AudioBufferPlayer>(audioBuffer_.get());

	nc::SceneNode &rootNode = nc::theApplication().rootNode();
	dummy_ = nctl::makeUnique<nc::SceneNode>(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	textNode_ = nctl::makeUnique<nc::TextNode>(dummy_.get(), font_.get(), MaxStringLength);
	textNode_->setAlignment(nc::TextNode::Alignment::LEFT);
	textString_ = nctl::makeUnique<nctl::String>(MaxStringLength);

#if WITH_STRESS_TEST
	nc::IAudioDevice &device = nc::theServiceLocator().audioDevice();

	// Load music files from disk once, then pass the memory buffers to all sources
	unsigned long int musicFileLengths[NumMusicFiles];
	nctl::UniquePtr<unsigned char[]> musicFileBuffers[NumMusicFiles];
	for (unsigned int i = 0; i < NumMusicFiles; i++)
	{
		nctl::UniquePtr<nc::IFile> musicFile = nc::IFile::createFileHandle(prefixDataPath("sounds", MusicFiles[i]).data());
		musicFile->open(nc::IFile::OpenMode::READ | nc::IFile::OpenMode::BINARY);
		musicFileLengths[i] = musicFile->size();
		musicFileBuffers[i] = nctl::makeUnique<unsigned char[]>(musicFileLengths[i]);
		musicFile->read(musicFileBuffers[i].get(), musicFileLengths[i]);
		musicFile->close();
	}

	// Load sound files from disk once, then pass the memory buffers to all sources
	unsigned long int soundFileLengths[NumSoundFiles];
	nctl::UniquePtr<unsigned char[]> soundFileBuffers[NumSoundFiles];
	for (unsigned int i = 0; i < NumSoundFiles; i++)
	{
		nctl::UniquePtr<nc::IFile> soundFile = nc::IFile::createFileHandle(prefixDataPath("sounds", SoundFiles[i]).data());
		soundFile->open(nc::IFile::OpenMode::READ | nc::IFile::OpenMode::BINARY);
		soundFileLengths[i] = soundFile->size();
		soundFileBuffers[i] = nctl::makeUnique<unsigned char[]>(soundFileLengths[i]);
		soundFile->read(soundFileBuffers[i].get(), soundFileLengths[i]);
		soundFile->close();
	}

	for (unsigned int i = 0; i < device.maxNumSources(); i++)
	{
		stressAudioStreamPlayers.pushBack(nctl::makeUnique<nc::AudioStreamPlayer>(MusicFiles[i % NumMusicFiles], musicFileBuffers[i % NumMusicFiles].get(), musicFileLengths[i % NumMusicFiles]));
		stressAudioBuffers.pushBack(nctl::makeUnique<nc::AudioBuffer>(SoundFiles[i % NumSoundFiles], soundFileBuffers[i % NumSoundFiles].get(), soundFileLengths[i % NumSoundFiles]));
		stressAudioBufferPlayers.pushBack(nctl::makeUnique<nc::AudioBufferPlayer>(stressAudioBuffers.back().get()));
	}
#endif

	soundPlayer_->play();
}

void MyEventHandler::onFrameStart()
{
	nc::IAudioDevice &device = nc::theServiceLocator().audioDevice();

#if NCINE_WITH_IMGUI
	if (showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(580.0f, 580.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(35.0f, 35.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("apptest_audio", &showImGui))
		{
			if (ImGui::TreeNode("Device Properties"))
			{
				const nc::IAudioDevice::Attributes &properties = device.attributes();

				ImGui::Text("Device name: %s", properties.deviceName);
				ImGui::Text("Version: %d.%d", properties.majorVersion, properties.minorVersion);
				ImGui::Text("Output frequency: %d", properties.outputFrequency);
				ImGui::Text("Mono sources: %d", properties.numMonoSources);
				ImGui::Text("Stereo sources: %d", properties.numStereoSources);
				ImGui::Text("Refresh rate: %d", properties.refreshRate);
				ImGui::Text("Synchronous context: %d", properties.synchronous);

				ImGui::NewLine();
				float gain = device.gain();
				ImGui::SliderFloat("Gain", &gain, nc::IAudioPlayer::MinGain, nc::IAudioPlayer::MaxGain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				device.setGain(gain);

				nc::Vector3f position = device.position();
				ImGui::SliderFloat3("Position", position.data(), -MaxXPos, MaxXPos, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				device.setPosition(position);

				nc::Vector3f velocity = device.velocity();
				ImGui::SliderFloat3("Velocity", velocity.data(), -MaxXPos, MaxXPos, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				device.setVelocity(velocity);

				if (ImGui::Button("Reset"))
				{
					device.setGain(nc::IAudioPlayer::DefaultGain);
					device.setPosition(0.0f, 0.0f, 0.0f);
					device.setVelocity(0.0f, 0.0f, 0.0f);
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Music player", ImGuiTreeNodeFlags_DefaultOpen))
			{
				const nc::AudioStreamPlayer *audioStreamPlayer = static_cast<const nc::AudioStreamPlayer *>(musicPlayer_.get());
				if (ImGui::TreeNode("Audio stream information"))
				{
					ImGui::Text("Bytes per sample: %d", audioStreamPlayer->bytesPerSample());
					ImGui::Text("Number of channels: %d", audioStreamPlayer->numChannels());
					ImGui::Text("Frequency: %d Hz", audioStreamPlayer->frequency());
					ImGui::Text("Number of samples: %ld", audioStreamPlayer->numSamples());
					ImGui::Text("Duration: %.3f s", audioStreamPlayer->duration());
					ImGui::Text("Buffer size: %ld", audioStreamPlayer->bufferSize());
					ImGui::Text("Number of stream samples: %ld", audioStreamPlayer->numSamplesInStreamBuffer());
					ImGui::Text("Stream buffer size: %d", audioStreamPlayer->streamBufferSize());

					ImGui::TreePop();
				}

				if (ImGui::Combo("Music file", &selectedMusicFile, MusicFiles, NumMusicFiles))
					static_cast<nc::AudioStreamPlayer *>(musicPlayer_.get())->loadFromFile(prefixDataPath("sounds", MusicFiles[selectedMusicFile]).data());

				const int sampleOffset = musicPlayer_->sampleOffset();
				const unsigned long numSamplesInStreamBuffer = audioStreamPlayer->numSamplesInStreamBuffer();
				const float fraction = sampleOffset / static_cast<float>(numSamplesInStreamBuffer);
				ImGui::ProgressBar(fraction, ImVec2(0.0f, 0.0f));
				ImGui::Text("Samples: %d / %ld", sampleOffset, numSamplesInStreamBuffer);

				const unsigned long sampleOffsetInStream = audioStreamPlayer->sampleOffsetInStream();
				const unsigned long numSamples = musicPlayer_->numSamples();
				const float streamFraction = sampleOffsetInStream / static_cast<float>(numSamples);
				ImGui::ProgressBar(streamFraction, ImVec2(0.0f, 0.0f));
				ImGui::Text("Stream samples: %ld / %ld", sampleOffsetInStream, numSamples);

				sourcePropertiesGui(*musicPlayer_);

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
						ImGui::Text("Number of samples: %lu", audioBuffer->numSamples());
						ImGui::Text("Duration: %.3f s", audioBuffer->duration());
						ImGui::Text("Buffer size: %lu", audioBuffer->bufferSize());

						ImGui::TreePop();
					}
				}

				if (ImGui::Combo("Sound file", &selectedSoundFile, SoundFiles, NumSoundFiles))
				{
					soundPlayer_->stop();
					audioBuffer_->loadFromFile(prefixDataPath("sounds", SoundFiles[selectedSoundFile]).data());
				}

				const int sampleOffset = soundPlayer_->sampleOffset();
				const unsigned long numSamples = soundPlayer_->numSamples();
				const float fraction = sampleOffset / static_cast<float>(numSamples);
				ImGui::ProgressBar(fraction, ImVec2(0.0f, 0.0f));
				ImGui::Text("Samples: %d / %lu", sampleOffset, numSamples);

				sourcePropertiesGui(*soundPlayer_);

				ImGui::TreePop();
			}

	#if WITH_STRESS_TEST
			if (ImGui::TreeNodeEx("Stress mode"))
			{
				const unsigned int numAvailableSources = device.numAvailableSources();
				const unsigned int maxNumSources = device.maxNumSources();
				const float availableSourcesFraction = numAvailableSources / float(maxNumSources);
				ImGui::ProgressBar(availableSourcesFraction, ImVec2(0.0f, 0.0f));
				ImGui::Text("Available sources: %u / %u", numAvailableSources, maxNumSources);

				if (ImGui::Button(stressModeEnabled ? "Disable" : "Enable"))
					toggleStressTest();

				ImGui::SliderFloat("Target play pressure", &stressModeTargetPlayPressure, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

				ImGui::BeginDisabled(stressModeEnabled == false);
				const float phaseNumberFraction = stressModePhase / static_cast<float>(StressModePhases::END);
				ImGui::ProgressBar(phaseNumberFraction, ImVec2(0.0f, 0.0f));
				ImGui::Text("Phase: %d of %d", stressModePhase, StressModePhases::END);
				ImGui::SameLine();
				if (ImGui::Button("Next"))
					nextStressTestPhase();

				const float stressPhaseStartTime = stressModeEnabled ? stressModeTimestamp.secondsSince() : 0.0f;
				const float phaseTimeFraction = stressPhaseStartTime / StressModePhaseDuration;
				ImGui::ProgressBar(phaseTimeFraction, ImVec2(0.0f, 0.0f));
				ImGui::Text("Next phase in: %.2f s", StressModePhaseDuration - stressPhaseStartTime);

				const float stressDelayStartTime = stressModeEnabled ? stressModeDelayTimestamp.secondsSince() : 0.0f;
				const float delayTimeFraction = stressDelayStartTime / stressModeActionDelay;
				ImGui::ProgressBar(delayTimeFraction, ImVec2(0.0f, 0.0f));
				ImGui::Text("Next action in: %.2f s", stressModeActionDelay - stressDelayStartTime);

				const unsigned int numPlayers = device.numPlayers();
				unsigned int numPausedPlayers = 0;
				unsigned int numLockedPlayers = 0;
				for (unsigned int i = 0; i < numPlayers; i++)
				{
					if (device.player(i)->isPaused())
						numPausedPlayers++;
					if (device.player(i)->isSourceLocked())
						numLockedPlayers++;
				}

				const ImGuiTreeNodeFlags flags = (numPlayers > 0) ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
				if (ImGui::TreeNodeEx(&device, flags, "Active players: %u (%u paused, %u locked)", numPlayers, numPausedPlayers, numLockedPlayers))
				{
					for (unsigned int i = 0; i < numPlayers; i++)
					{
						const nc::IAudioPlayer *player = device.player(i);
						ImGui::Text("Player %u - state: %s - source: %u", i, audioPlayerStateToString(player->state()), player->sourceId());
					}

					ImGui::TreePop();
				}

				ImGui::EndDisabled();

				ImGui::TreePop();
			}
	#endif
		}
		ImGui::End();
	}
#endif

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

	textString_->formatAppend("Gain: %.2f (press KP 7/8/9)\n", soundPlayer_->gain());
	textString_->formatAppend("Pitch: %.2f (press KP 4/5/6)\n", soundPlayer_->pitch());
	textString_->formatAppend("Position: %.2f (press KP 1/2/3)\n", soundPlayer_->position().x);

	textNode_->setString(*textString_);
	textNode_->setPosition(0.0f, nc::theApplication().height() * VerticalTextPos - textNode_->height() * 0.5f);

#if WITH_STRESS_TEST
	if (stressModeEnabled)
	{
		soundPlayer_->stop();
		musicPlayer_->stop();
		soundPlayer_->setSourceLocked(false);
		musicPlayer_->setSourceLocked(false);

		textString_->clear();
		textString_->formatAppend("Stress Mode, phase %d of %d\n", stressModePhase, StressModePhases::END);
		textString_->formatAppend("Next phase in %.2f s\n", StressModePhaseDuration - stressModeTimestamp.secondsSince());
		textString_->formatAppend("Next action in %.2f s\n", stressModeActionDelay - stressModeDelayTimestamp.secondsSince());

		const unsigned int numPlayers = device.numPlayers();
		unsigned int numPausedPlayers = 0;
		unsigned int numLockedPlayers = 0;
		for (unsigned int i = 0; i < numPlayers; i++)
		{
			if (device.player(i)->isPaused())
				numPausedPlayers++;
			if (device.player(i)->isSourceLocked())
				numLockedPlayers++;
		}
		textString_->formatAppend("Active players: %u (%u paused, %u locked)\n", numPlayers, numPausedPlayers, numLockedPlayers);

		textNode_->setString(*textString_);
		textNode_->setPosition(0.0f, nc::theApplication().height() * VerticalTextPos - textNode_->height() * 0.5f);

		handleStressTestPhases();
	}
#endif
}

void MyEventHandler::onShutdown()
{
#if WITH_STRESS_TEST
	stressAudioBufferPlayers.clear();
	stressAudioStreamPlayers.clear();
	stressAudioBuffers.clear();
#endif
}

#ifdef __ANDROID__
void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	#if NCINE_WITH_IMGUI
	// Ignore the touch event if ImGui interface is visible
	if (showImGui)
		return;
	#endif

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
		soundPlayer_->setLooping(!soundPlayer_->isLooping());
#if WITH_STRESS_TEST
	else if (event.sym == nc::KeySym::T)
		toggleStressTest();
#endif
	else if (event.sym == nc::KeySym::KP0)
		resetValues();
	else if (event.sym == nc::KeySym::KP7)
	{
		const float gain = soundPlayer_->gain() - 0.1f * (nc::IAudioPlayer::MaxGain - nc::IAudioPlayer::MinGain);
		soundPlayer_->setGain(gain);
	}
	else if (event.sym == nc::KeySym::KP8)
		soundPlayer_->setGain(nc::IAudioPlayer::DefaultGain);
	else if (event.sym == nc::KeySym::KP9)
	{
		const float gain = soundPlayer_->gain() + 0.1f * (nc::IAudioPlayer::MaxGain - nc::IAudioPlayer::MinGain);
		soundPlayer_->setGain(gain);
	}
	else if (event.sym == nc::KeySym::KP4)
	{
		const float pitch = soundPlayer_->pitch() - 0.1f * (nc::IAudioPlayer::MaxPitch - nc::IAudioPlayer::MinPitch);
		soundPlayer_->setPitch(pitch);
	}
	else if (event.sym == nc::KeySym::KP5)
		soundPlayer_->setPitch(nc::IAudioPlayer::DefaultPitch);
	else if (event.sym == nc::KeySym::KP6)
	{
		const float pitch = soundPlayer_->pitch() + 0.1f * (nc::IAudioPlayer::MaxPitch - nc::IAudioPlayer::MinPitch);
		soundPlayer_->setPitch(pitch);
	}
	else if (event.sym == nc::KeySym::KP1)
	{
		const nc::Vector3f position = soundPlayer_->position() - nc::Vector3f(MaxXPos * 0.1f, 0.0f, 0.0f);
		soundPlayer_->setPosition(position);
	}
	else if (event.sym == nc::KeySym::KP2)
		soundPlayer_->setPosition(DefaultXPos, soundPlayer_->position().y, soundPlayer_->position().z);
	else if (event.sym == nc::KeySym::KP3)
	{
		const nc::Vector3f position = soundPlayer_->position() + nc::Vector3f(MaxXPos * 0.1f, 0.0f, 0.0f);
		soundPlayer_->setPosition(position);
	}
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
	{
		const nc::Vector3f position(MaxXPos * event.value, soundPlayer_->position().y, soundPlayer_->position().z);
		soundPlayer_->setPosition(position);
	}
	else if (event.axisName == nc::AxisName::LTRIGGER)
		soundPlayer_->setGain(1.0f - event.value);
	else if (event.axisName == nc::AxisName::RTRIGGER)
		soundPlayer_->setPitch(nc::IAudioPlayer::DefaultPitch + (nc::IAudioPlayer::MaxPitch - nc::IAudioPlayer::DefaultPitch) * event.value);
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
#if WITH_STRESS_TEST
	else if (event.buttonName == nc::ButtonName::START)
		toggleStressTest();
#endif
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
	soundPlayer_->setGain(nc::IAudioPlayer::DefaultGain);
	soundPlayer_->setPitch(nc::IAudioPlayer::DefaultPitch);
	soundPlayer_->setPosition(DefaultXPos, soundPlayer_->position().y, soundPlayer_->position().z);
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
