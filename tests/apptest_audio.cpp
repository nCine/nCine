#include <ncine/config.h>

#include "apptest_audio.h"
#include <ncine/Application.h>
#include <ncine/IAudioDevice.h>
#include <ncine/AudioBuffer.h>
#include <ncine/AudioBufferPlayer.h>
#include <ncine/AudioStream.h>
#include <ncine/AudioStreamPlayer.h>
#if NCINE_WITH_OPENAL_EXT
	#include <ncine/AudioEffectProperties.h>
	#include <ncine/AudioEffectSlot.h>
	#include <ncine/AudioFilter.h>
#endif
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
	#if NCINE_WITH_OPENAL_EXT
const char *EffectTypeLabels[14] = { "Null", "Reverb", "EAX Reverb", "Chorus", "Distortion", "Echo",
                                     "Flanger", "Frequency Shifter", "Vocal Morpher", "Pitch Shifter",
                                     "Ring Modulator", "Autowah", "Compressor", "Equalizer" };
const char *EfxPresetLabels[nc::AudioEffectProperties::EfxReverbPresets::COUNT + 1] = {
    "Custom", "Generic", "Padded Cell", "Room", "Bathroom", "Living Room", "Stone Room", "Auditorium", "Concert Hall", "Cave", "Arena", "Hangar", "Carpeted Hallway",
    "Hallway", "Stone Corridor", "Alley", "Forest", "City", "Mountains", "Quarry", "Plain", "Parking Lot", "Sewer Pipe", "Underwater", "Drugged", "Dizzy", "Psychotic",
    "Castle Small Room", "Castle Short Passage", "Castle Medium Room", "Castle Large Room", "Castle Long Passage", "Castle Hall", "Castle Cupboard", "Castle Courtyard", "Castle Alcove",
    "Factory Small Room", "Factory Short Passage", "Factory Medium Room", "Factory Large Room", "Factory Long Passage", "Factory Hall", "Factory Cupboard", "Factory Courtyard", "Factory Alcove",
    "Ice Palace Small Room", "Ice Palace Short Passage", "Ice Palace Medium Room", "Ice Palace Large Room", "Ice Palace Long Passage", "Ice Palace Hall", "Ice Palace Cupboard", "Ice Palace Courtyard", "Ice Palace Alcove",
    "Space Station Small Room", "Space Station Short Passage", "Space Station Medium Room", "Space Station Large Room", "Space Station Long Passage", "Space Station Hall", "Space Station Cupboard", "Space Station Alcove",
    "Wooden Galeon Small Room", "Wooden Galeon Short Passage", "Wooden Galeon Medium Room", "Wooden Galeon Large Room", "Wooden Galeon Long Passage", "Wooden Galeon Hall", "Wooden Galeon Cupboard", "Wooden Galeon Courtyard", "Wooden Galeon Alcove",
    "Sport Empty Stadium", "Sport Squash Court", "Sport Small Swimming Pool", "Sport Large Swimming Pool", "Sport Gymnasium", "Sport Full Stadium", "Sport Stadium Tannoy",
    "Prefab Workshop", "Prefab School Room", "Prefab Practise Room", "Prefab Outhouse", "Prefab Caravan",
    "Dome Tomb", "Pipe Small", "Dome Saints Paul", "Pipe Longthin", "Pipe Large", "Pipe Resonant",
    "Outdoors Backyard", "Outdoors Rolling Plains", "Outdoors Deep Canyon", "Outdoors Crrek", "Outdoors Valley",
    "Mood Heaven", "Mood Hell", "Mod Memories",
    "Driving Commentator", "Driving Pit Garage", "Driving In-car Racer", "Driving In-car Sports", "Driving In-car Luxury", "Driving Full Grandstand", "Driving Empty Grandstand", "Driving Tunnel",
    "City Streets", "City Subway", "City Museum", "City Library", "City Underpass", "City Abandoned",
    "Dusty Room", "Chapel", "Small Water Room"
};
static_assert(nc::AudioEffectProperties::EfxReverbPresets::COUNT + 1 == sizeof(EfxPresetLabels) / sizeof(EfxPresetLabels[0]), "EFX reverb presets and their name strings should match");
const char *WaveformLabels[3] = { "Sinusoid", "Triangle", "Sawtooth" };
const char *DirectionLabels[3] = { "Down", "Up", "Off" };
const char *PhonemeLabels[30] = { "A", "E", "I", "O", "U", "AA", "AE", "AH", "AO", "EH", "ER", "IH", "IY", "UH",
                                  "UW", "B", "D", "F", "G", "J", "K", "L", "M", "N", "P", "R", "S", "T", "V", "Z" };
const char *OnOffLabels[2] = { "Off", "On" };
const char *FilterTypeLabels[4] = { "Null", "Low-pass", "High-pass", "Band-pass" };
nc::AudioEffectProperties effectProperties;
nc::AudioFilter::Properties filterProperties;
	#endif
bool showImGui = true;

void sourcePropertiesGui(nc::IAudioPlayer &player, nc::AudioEffectSlot &audioEffectSlot, nc::AudioFilter &directAudioFilter)
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

	#if NCINE_WITH_OPENAL_EXT
	bool effectSlot = player.hasEffectSlot();
	ImGui::Checkbox("Effect slot", &effectSlot);
	player.setEffectSlot(effectSlot ? &audioEffectSlot : nullptr);

	ImGui::SameLine();
	bool directFilter = player.hasDirectFilter();
	ImGui::Checkbox("Direct filter", &directFilter);
	player.setDirectFilter(directFilter ? &directAudioFilter : nullptr);
	#endif

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

	#if NCINE_WITH_OPENAL_EXT
	if (device.hasExtension(nc::IAudioDevice::ALExtensions::EXT_EFX))
	{
		ImGui::NewLine();
		float airAbsorptionFactor = player.airAbsorptionFactor();
		ImGui::SliderFloat("Air absorption factor", &airAbsorptionFactor, nc::IAudioPlayer::MinAirAbsorptionFactor, nc::IAudioPlayer::MaxAirAbsorptionFactor, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		player.setAirAbsorptionFactor(airAbsorptionFactor);

		float roomRolloffFactor = player.roomRolloffFactor();
		ImGui::SliderFloat("Room rolloff factor", &roomRolloffFactor, nc::IAudioPlayer::MinRoomRolloffFactor, nc::IAudioPlayer::MaxRoomRolloffFactor, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		player.setRoomRolloffFactor(roomRolloffFactor);

		float coneOuterGainHF = player.coneOuterGainHF();
		ImGui::SliderFloat("Cone outer gain HF", &coneOuterGainHF, nc::IAudioPlayer::MinConeOuterGainHF, nc::IAudioPlayer::MaxConeOuterGainHF, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		player.setConeOuterGainHF(coneOuterGainHF);
	}
	#endif

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

	#if NCINE_WITH_OPENAL_EXT
		if (device.hasExtension(nc::IAudioDevice::ALExtensions::EXT_EFX))
		{
			player.setAirAbsorptionFactor(nc::IAudioPlayer::DefaultAirAbsorptionFactor);
			player.setRoomRolloffFactor(nc::IAudioPlayer::DefaultRoomRolloffFactor);
			player.setConeOuterGainHF(nc::IAudioPlayer::DefaultConeOuterGainHF);
		}
	#endif
	}
}

	#if NCINE_WITH_OPENAL_EXT
bool nullEffectPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::NULL_EFFECT);
	return false;
}

bool reverbPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::REVERB);
	nc::AudioEffectProperties::ReverbProperties &props = effectProperties.reverbProperties();
	const nc::AudioEffectProperties::ReverbProperties &min = nc::AudioEffectProperties::minReverbProperties();
	const nc::AudioEffectProperties::ReverbProperties &max = nc::AudioEffectProperties::maxReverbProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderFloat("Density", &props.density, min.density, max.density, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Diffusion", &props.diffusion, min.diffusion, max.diffusion, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Gain", &props.gain, min.gain, max.gain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Gain HF", &props.gainHF, min.gainHF, max.gainHF, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Decay time", &props.decayTime, min.decayTime, max.decayTime, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Decay HF ratio", &props.decayHFRatio, min.decayHFRatio, max.decayHFRatio, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Reflections gain", &props.reflectionsGain, min.reflectionsGain, max.reflectionsGain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Reflections delay", &props.reflectionsDelay, min.reflectionsDelay, max.reflectionsDelay, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Late reverb gain", &props.lateReverbGain, min.lateReverbGain, max.lateReverbGain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Late reverb delay", &props.lateReverbDelay, min.lateReverbDelay, max.lateReverbDelay, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Air absorption gain HF", &props.airAbsorptionGainHF, min.airAbsorptionGainHF, max.airAbsorptionGainHF, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Room roll-off factor", &props.roomRolloffFactor, min.roomRolloffFactor, max.roomRolloffFactor, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Decay HF limit", &props.decayHFLimit, min.decayHFLimit, max.decayHFLimit, OnOffLabels[props.decayHFLimit], ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::ReverbProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool eaxReverbPropertiesGui()
{
	static int selectedEfxPreset = 0;

	effectProperties.setType(nc::AudioEffect::Type::EAXREVERB);
	nc::AudioEffectProperties::EaxReverbProperties &props = effectProperties.eaxReverbProperties();
	const nc::AudioEffectProperties::EaxReverbProperties &min = nc::AudioEffectProperties::minEaxReverbProperties();
	const nc::AudioEffectProperties::EaxReverbProperties &max = nc::AudioEffectProperties::maxEaxReverbProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderFloat("Density", &props.density, min.density, max.density, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Diffusion", &props.diffusion, min.diffusion, max.diffusion, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Gain", &props.gain, min.gain, max.gain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Gain HF", &props.gainHF, min.gainHF, max.gainHF, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Gain LF", &props.gainLF, min.gainLF, max.gainLF, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Decay time", &props.decayTime, min.decayTime, max.decayTime, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Decay HF ratio", &props.decayHFRatio, min.decayHFRatio, max.decayHFRatio, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Decay LF ratio", &props.decayLFRatio, min.decayLFRatio, max.decayLFRatio, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Reflections gain", &props.reflectionsGain, min.reflectionsGain, max.reflectionsGain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Reflections delay", &props.reflectionsDelay, min.reflectionsDelay, max.reflectionsDelay, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat3("Reflections pan", &props.reflectionsPan[0], min.reflectionsPan[0], max.reflectionsPan[0], "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Late reverb gain", &props.lateReverbGain, min.lateReverbGain, max.lateReverbGain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Late reverb delay", &props.lateReverbDelay, min.lateReverbDelay, max.lateReverbDelay, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat3("Late reverb pan", &props.lateReverbPan[0], min.lateReverbPan[0], max.lateReverbPan[0], "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Echo time", &props.echoTime, min.echoTime, max.echoTime, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Echo depth", &props.echoDepth, min.echoDepth, max.echoDepth, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Modulation time", &props.modulationTime, min.modulationTime, max.modulationTime, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Modulation depth", &props.modulationDepth, min.modulationDepth, max.modulationDepth, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Air absorption gain HF", &props.airAbsorptionGainHF, min.airAbsorptionGainHF, max.airAbsorptionGainHF, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("HF reference", &props.hfReference, min.hfReference, max.hfReference, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("LF reference", &props.lfReference, min.lfReference, max.lfReference, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Room roll-off factor", &props.roomRolloffFactor, min.roomRolloffFactor, max.roomRolloffFactor, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Decay HF limit", &props.decayHFLimit, min.decayHFLimit, max.decayHFLimit, OnOffLabels[props.decayHFLimit], ImGuiSliderFlags_AlwaysClamp);

	// Set the custom preset if any properties has been altered
	if (valueChanged)
		selectedEfxPreset = 0;

	if (ImGui::Combo("EFX Preset", &selectedEfxPreset, EfxPresetLabels, IM_ARRAYSIZE(EfxPresetLabels)))
	{
		if (selectedEfxPreset > 0)
		{
			props.loadPreset(nc::AudioEffectProperties::EfxReverbPresets(selectedEfxPreset - 1));
			valueChanged = true;
		}
	}

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::EaxReverbProperties();
		selectedEfxPreset = 0;
		valueChanged = true;
	}

	return valueChanged;
}

bool chorusPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::CHORUS);
	nc::AudioEffectProperties::ChorusProperties &props = effectProperties.chorusProperties();
	const nc::AudioEffectProperties::ChorusProperties &min = nc::AudioEffectProperties::minChorusProperties();
	const nc::AudioEffectProperties::ChorusProperties &max = nc::AudioEffectProperties::maxChorusProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderInt("Waveform", &props.waveform, min.waveform, max.waveform, WaveformLabels[props.waveform], ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Phase", &props.phase, min.phase, max.phase, "%d", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Rate", &props.rate, min.rate, max.rate, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Depth", &props.depth, min.depth, max.depth, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Feedback", &props.feedback, min.feedback, max.feedback, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Delay", &props.delay, min.delay, max.delay, "%.3f", ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::ChorusProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool distortionPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::DISTORTION);
	nc::AudioEffectProperties::DistortionProperties &props = effectProperties.distortionProperties();
	const nc::AudioEffectProperties::DistortionProperties &min = nc::AudioEffectProperties::minDistortionProperties();
	const nc::AudioEffectProperties::DistortionProperties &max = nc::AudioEffectProperties::maxDistortionProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderFloat("Edge", &props.edge, min.edge, max.edge, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Gain", &props.gain, min.gain, max.gain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Low-pass cutoff", &props.lowpassCutoff, min.lowpassCutoff, max.lowpassCutoff, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Eq center", &props.eqCenter, min.eqCenter, max.eqCenter, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Eq bandwidth", &props.eqBandwidth, min.eqBandwidth, max.eqBandwidth, "%.2f", ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::DistortionProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool echoPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::ECHO);
	nc::AudioEffectProperties::EchoProperties &props = effectProperties.echoProperties();
	const nc::AudioEffectProperties::EchoProperties &min = nc::AudioEffectProperties::minEchoProperties();
	const nc::AudioEffectProperties::EchoProperties &max = nc::AudioEffectProperties::maxEchoProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderFloat("Delay", &props.delay, min.delay, max.delay, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("LR delay", &props.lrDelay, min.lrDelay, max.lrDelay, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Damping", &props.damping, min.damping, max.damping, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Feedback", &props.feedback, min.feedback, max.feedback, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Spread", &props.spread, min.spread, max.spread, "%.2f", ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::EchoProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool flangerPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::FLANGER);
	nc::AudioEffectProperties::FlangerProperties &props = effectProperties.flangerProperties();
	const nc::AudioEffectProperties::FlangerProperties &min = nc::AudioEffectProperties::minFlangerProperties();
	const nc::AudioEffectProperties::FlangerProperties &max = nc::AudioEffectProperties::maxFlangerProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderInt("Waveform", &props.waveform, min.waveform, max.waveform, WaveformLabels[props.waveform], ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Phase", &props.phase, min.phase, max.phase, "%d", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Rate", &props.rate, min.rate, max.rate, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Depth", &props.depth, min.depth, max.depth, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Feedback", &props.feedback, min.feedback, max.feedback, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Delay", &props.delay, min.delay, max.delay, "%.3f", ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::FlangerProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool frequencyShifterPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::FREQUENCY_SHIFTER);
	nc::AudioEffectProperties::FrequencyShifterProperties &props = effectProperties.frequencyShifterProperties();
	const nc::AudioEffectProperties::FrequencyShifterProperties &min = nc::AudioEffectProperties::minFrequencyShifterProperties();
	const nc::AudioEffectProperties::FrequencyShifterProperties &max = nc::AudioEffectProperties::maxFrequencyShifterProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderFloat("Frequency", &props.frequency, min.frequency, max.frequency, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Left direction", &props.leftDirection, min.leftDirection, max.leftDirection, DirectionLabels[props.leftDirection], ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Right direction", &props.rightDirection, min.rightDirection, max.rightDirection, DirectionLabels[props.rightDirection], ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::FrequencyShifterProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool vocalMorpherPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::VOCAL_MORPHER);
	nc::AudioEffectProperties::VocalMorpherProperties &props = effectProperties.vocalMorpherProperties();
	const nc::AudioEffectProperties::VocalMorpherProperties &min = nc::AudioEffectProperties::minVocalMorpherProperties();
	const nc::AudioEffectProperties::VocalMorpherProperties &max = nc::AudioEffectProperties::maxVocalMorpherProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderInt("Phoneme A", &props.phonemeA, min.phonemeA, max.phonemeA, PhonemeLabels[props.phonemeA], ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Phoneme A coarse tuning", &props.phonemeACoarseTuning, min.phonemeACoarseTuning, max.phonemeACoarseTuning, "%d", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Phoneme B", &props.phonemeB, min.phonemeB, max.phonemeB, PhonemeLabels[props.phonemeB], ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Phoneme B coarse tuning", &props.phonemeBCoarseTuning, min.phonemeBCoarseTuning, max.phonemeBCoarseTuning, "%d", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Waveform", &props.waveform, min.waveform, max.waveform, WaveformLabels[props.waveform], ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Rate", &props.rate, min.rate, max.rate, "%.2f", ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::VocalMorpherProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool pitchShifterPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::PITCH_SHIFTER);
	nc::AudioEffectProperties::PitchShifterProperties &props = effectProperties.pitchShifterProperties();
	const nc::AudioEffectProperties::PitchShifterProperties &min = nc::AudioEffectProperties::minPitchShifterProperties();
	const nc::AudioEffectProperties::PitchShifterProperties &max = nc::AudioEffectProperties::maxPitchShifterProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderInt("Coarse tune", &props.coarseTune, min.coarseTune, max.coarseTune, "%d", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Fine tune", &props.fineTune, min.fineTune, max.fineTune, "%d", ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::PitchShifterProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool ringModulatorPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::RING_MODULATOR);
	nc::AudioEffectProperties::RingModulatorProperties &props = effectProperties.ringModulatorProperties();
	const nc::AudioEffectProperties::RingModulatorProperties &min = nc::AudioEffectProperties::minRingModulatorProperties();
	const nc::AudioEffectProperties::RingModulatorProperties &max = nc::AudioEffectProperties::maxRingModulatorProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderFloat("Rate", &props.frequency, min.frequency, max.frequency, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("High-pass cutoff", &props.highpassCutoff, min.highpassCutoff, max.highpassCutoff, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderInt("Waveform", &props.waveform, min.waveform, max.waveform, WaveformLabels[props.waveform], ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::maxRingModulatorProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool autoWahPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::AUTOWAH);
	nc::AudioEffectProperties::AutoWahProperties &props = effectProperties.autoWahProperties();
	const nc::AudioEffectProperties::AutoWahProperties &min = nc::AudioEffectProperties::minAutoWahProperties();
	const nc::AudioEffectProperties::AutoWahProperties &max = nc::AudioEffectProperties::maxAutoWahProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderFloat("Attack time", &props.attackTime, min.attackTime, max.attackTime, "%.4f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Release time", &props.releaseTime, min.releaseTime, max.releaseTime, "%.4f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Resonance", &props.resonance, min.resonance, max.resonance, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Peak gain", &props.peakGain, min.peakGain, max.peakGain, "%.5f", ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::maxAutoWahProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool compressorPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::COMPRESSOR);
	nc::AudioEffectProperties::CompressorProperties &props = effectProperties.compressorProperties();
	const nc::AudioEffectProperties::CompressorProperties &min = nc::AudioEffectProperties::minCompressorProperties();
	const nc::AudioEffectProperties::CompressorProperties &max = nc::AudioEffectProperties::maxCompressorProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderInt("On/Off", &props.onOff, min.onOff, max.onOff, OnOffLabels[props.onOff], ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::maxCompressorProperties();
		valueChanged = true;
	}

	return valueChanged;
}

bool equalizerPropertiesGui()
{
	effectProperties.setType(nc::AudioEffect::Type::EQUALIZER);
	nc::AudioEffectProperties::EqualizerProperties &props = effectProperties.equalizerProperties();
	const nc::AudioEffectProperties::EqualizerProperties &min = nc::AudioEffectProperties::minEqualizerProperties();
	const nc::AudioEffectProperties::EqualizerProperties &max = nc::AudioEffectProperties::maxEqualizerProperties();

	bool valueChanged = false;
	valueChanged |= ImGui::SliderFloat("Low gain", &props.lowGain, min.lowGain, max.lowGain, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Low cutoff", &props.lowCutoff, min.lowCutoff, max.lowCutoff, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Mid1 gain", &props.mid1Gain, min.mid1Gain, max.mid1Gain, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Mid1 center", &props.mid1Center, min.mid1Center, max.mid1Center, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Mid1 width", &props.mid1Width, min.mid1Width, max.mid1Width, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Mid2 gain", &props.mid2Gain, min.mid2Gain, max.mid2Gain, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Mid2 center", &props.mid2Center, min.mid2Center, max.mid2Center, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("Mid2 width", &props.mid2Width, min.mid2Width, max.mid2Width, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("High gain", &props.highGain, min.highGain, max.highGain, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	valueChanged |= ImGui::SliderFloat("High cutoff", &props.highCutoff, min.highCutoff, max.highCutoff, "%.2f", ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Reset"))
	{
		props = nc::AudioEffectProperties::EqualizerProperties();
		valueChanged = true;
	}

	return valueChanged;
}
	#endif

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

	nc::IAudioDevice &device = nc::theServiceLocator().audioDevice();
#if NCINE_WITH_OPENAL_EXT
	if (device.hasExtension(nc::IAudioDevice::ALExtensions::EXT_EFX))
	{
		audioEffect_ = nctl::makeUnique<nc::AudioEffect>();
		audioEffectSlot_ = nctl::makeUnique<nc::AudioEffectSlot>(*audioEffect_);
		audioFilter_ = nctl::makeUnique<nc::AudioFilter>();
		soundPlayer_->setEffectSlot(audioEffectSlot_.get(), audioFilter_.get());
		musicPlayer_->setEffectSlot(audioEffectSlot_.get(), audioFilter_.get());
	}
#endif

#if WITH_STRESS_TEST
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
				ImGui::TextUnformatted("OpenAL EFX extension:");
				ImGui::SameLine();
				if (device.hasExtension(nc::IAudioDevice::ALExtensions::EXT_EFX))
				{
					ImGui::TextColored(Green, "available");
					ImGui::Text("EFX version: %d.%d", properties.efxMajorVersion, properties.efxMinorVersion);
					ImGui::Text("Max auxiliary sends: %d", properties.maxAuxiliarySends);
				}
				else
					ImGui::TextColored(Red, "not available");

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

				sourcePropertiesGui(*musicPlayer_, *audioEffectSlot_, *audioFilter_);

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

				sourcePropertiesGui(*soundPlayer_, *audioEffectSlot_, *audioFilter_);

				ImGui::TreePop();
			}

	#if NCINE_WITH_OPENAL_EXT
			if (device.hasExtension(nc::IAudioDevice::ALExtensions::EXT_EFX))
			{
				if (ImGui::TreeNodeEx("EFX audio effects"))
				{
					bool valueChanged = false;

					if (ImGui::Combo("Effect", &selectedEffectType, EffectTypeLabels, IM_ARRAYSIZE(EffectTypeLabels)))
						valueChanged = true;

					switch (selectedEffectType)
					{
						default:
						case static_cast<int>(nc::AudioEffect::Type::NULL_EFFECT):
							valueChanged |= nullEffectPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::REVERB):
							valueChanged |= reverbPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::EAXREVERB):
							valueChanged |= eaxReverbPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::CHORUS):
							valueChanged |= chorusPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::DISTORTION):
							valueChanged |= distortionPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::ECHO):
							valueChanged |= echoPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::FLANGER):
							valueChanged |= flangerPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::FREQUENCY_SHIFTER):
							valueChanged |= frequencyShifterPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::VOCAL_MORPHER):
							valueChanged |= vocalMorpherPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::PITCH_SHIFTER):
							valueChanged |= pitchShifterPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::RING_MODULATOR):
							valueChanged |= ringModulatorPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::AUTOWAH):
							valueChanged |= autoWahPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::COMPRESSOR):
							valueChanged |= compressorPropertiesGui();
							break;
						case static_cast<int>(nc::AudioEffect::Type::EQUALIZER):
							valueChanged |= equalizerPropertiesGui();
							break;
					}

					if (valueChanged)
					{
						audioEffect_->applyProperties(effectProperties);
						audioEffectSlot_->applyEffect(*audioEffect_);
					}

					ImGui::TreePop();
				}
				if (ImGui::TreeNodeEx("EFX audio filters"))
				{
					bool valueChanged = false;

					if (ImGui::Combo("Filter", &selectedFilterType, FilterTypeLabels, IM_ARRAYSIZE(FilterTypeLabels)))
						valueChanged = true;

					switch (selectedFilterType)
					{
						default:
						case static_cast<int>(nc::AudioFilter::Type::NULL_FILTER):
							filterProperties = nc::AudioFilter::Properties();
							break;
						case static_cast<int>(nc::AudioFilter::Type::LOWPASS):
							filterProperties.type = nc::AudioFilter::Type::LOWPASS;
							valueChanged |= ImGui::SliderFloat("Gain", &filterProperties.gain, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							valueChanged |= ImGui::SliderFloat("Gain HF", &filterProperties.gainHF, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							break;
						case static_cast<int>(nc::AudioFilter::Type::HIGHPASS):
							filterProperties.type = nc::AudioFilter::Type::HIGHPASS;
							valueChanged |= ImGui::SliderFloat("Gain", &filterProperties.gain, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							valueChanged |= ImGui::SliderFloat("Gain LF", &filterProperties.gainLF, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							break;
						case static_cast<int>(nc::AudioFilter::Type::BANDPASS):
							filterProperties.type = nc::AudioFilter::Type::BANDPASS;
							valueChanged |= ImGui::SliderFloat("Gain", &filterProperties.gain, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							valueChanged |= ImGui::SliderFloat("Gain LF", &filterProperties.gainLF, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							valueChanged |= ImGui::SliderFloat("Gain HF", &filterProperties.gainHF, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
							break;
					}

					if (ImGui::Button("Reset"))
					{
						filterProperties.gain = 1.0f;
						filterProperties.gainLF = 1.0f;
						filterProperties.gainHF = 1.0f;
						valueChanged = true;
					}

					if (valueChanged)
						audioFilter_->applyProperties(filterProperties);

					ImGui::TreePop();
				}
			}
	#endif

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
