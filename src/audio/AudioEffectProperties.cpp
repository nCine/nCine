#define NCINE_INCLUDE_OPENAL
#define NCINE_INCLUDE_OPENAL_EFX_PRESETS
#include "common_headers.h"

#include <nctl/algorithms.h>
#include "common_macros.h"
#include "AudioEffectProperties.h"

namespace ncine {

namespace {

	const char *typeTostring(AudioEffect::Type type)
	{
		switch (type)
		{
			case AudioEffect::Type::NULL_EFFECT: return "null";
			case AudioEffect::Type::REVERB: return "reverb";
			case AudioEffect::Type::EAXREVERB: return "eaxReverb";
			case AudioEffect::Type::CHORUS: return "chorus";
			case AudioEffect::Type::DISTORTION: return "distortion";
			case AudioEffect::Type::ECHO: return "echo";
			case AudioEffect::Type::FLANGER: return "flanger";
			case AudioEffect::Type::FREQUENCY_SHIFTER: return "frequencyShifter";
			case AudioEffect::Type::VOCAL_MORPHER: return "vocalMorpher";
			case AudioEffect::Type::PITCH_SHIFTER: return "pitchShifter";
			case AudioEffect::Type::RING_MODULATOR: return "ringModulator";
			case AudioEffect::Type::AUTOWAH: return "autowah";
			case AudioEffect::Type::COMPRESSOR: return "compressor";
			case AudioEffect::Type::EQUALIZER: return "equalizer";
		}
		return "null";
	}

	void loadEfxReverbPreset(const EFXEAXREVERBPROPERTIES efxReverbPreset, AudioEffectProperties::EaxReverbProperties &eaxReverbProperties)
	{
		eaxReverbProperties.density = efxReverbPreset.flDensity;
		eaxReverbProperties.diffusion = efxReverbPreset.flDiffusion;
		eaxReverbProperties.gain = efxReverbPreset.flGain;
		eaxReverbProperties.gainHF = efxReverbPreset.flGainHF;
		eaxReverbProperties.gainLF = efxReverbPreset.flGainLF;
		eaxReverbProperties.decayTime = efxReverbPreset.flDecayTime;
		eaxReverbProperties.decayHFRatio = efxReverbPreset.flDecayHFRatio;
		eaxReverbProperties.decayLFRatio = efxReverbPreset.flDecayLFRatio;
		eaxReverbProperties.reflectionsGain = efxReverbPreset.flReflectionsGain;
		eaxReverbProperties.reflectionsDelay = efxReverbPreset.flReflectionsDelay;
		eaxReverbProperties.reflectionsPan[0] = efxReverbPreset.flReflectionsPan[0];
		eaxReverbProperties.reflectionsPan[1] = efxReverbPreset.flReflectionsPan[1];
		eaxReverbProperties.reflectionsPan[2] = efxReverbPreset.flReflectionsPan[2];
		eaxReverbProperties.lateReverbGain = efxReverbPreset.flLateReverbGain;
		eaxReverbProperties.lateReverbDelay = efxReverbPreset.flLateReverbDelay;
		eaxReverbProperties.lateReverbPan[0] = efxReverbPreset.flLateReverbPan[0];
		eaxReverbProperties.lateReverbPan[1] = efxReverbPreset.flLateReverbPan[1];
		eaxReverbProperties.lateReverbPan[2] = efxReverbPreset.flLateReverbPan[2];
		eaxReverbProperties.echoTime = efxReverbPreset.flEchoTime;
		eaxReverbProperties.echoDepth = efxReverbPreset.flEchoDepth;
		eaxReverbProperties.modulationTime = efxReverbPreset.flModulationTime;
		eaxReverbProperties.modulationDepth = efxReverbPreset.flModulationDepth;
		eaxReverbProperties.airAbsorptionGainHF = efxReverbPreset.flAirAbsorptionGainHF;
		eaxReverbProperties.hfReference = efxReverbPreset.flHFReference;
		eaxReverbProperties.lfReference = efxReverbPreset.flLFReference;
		eaxReverbProperties.roomRolloffFactor = efxReverbPreset.flRoomRolloffFactor;
		eaxReverbProperties.decayHFLimit = efxReverbPreset.iDecayHFLimit;
	}

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////
const float AudioEffectProperties::EaxReverbProperties::MinPan = -1024.0f;
const float AudioEffectProperties::EaxReverbProperties::MaxPan = 1024.0f;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioEffectProperties::AudioEffectProperties()
    : type_(AudioEffect::Type::NULL_EFFECT)
{
	memset(propertiesData_, 0, sizeof(propertiesData_));
}

AudioEffectProperties::AudioEffectProperties(AudioEffect::Type type)
    : AudioEffectProperties()
{
	setType(type);
}

AudioEffectProperties::ReverbProperties::ReverbProperties()
    : density(AL_REVERB_DEFAULT_DENSITY),
      diffusion(AL_REVERB_DEFAULT_DIFFUSION),
      gain(AL_REVERB_DEFAULT_GAIN),
      gainHF(AL_REVERB_DEFAULT_GAINHF),
      decayTime(AL_REVERB_DEFAULT_DECAY_TIME),
      decayHFRatio(AL_REVERB_DEFAULT_DECAY_HFRATIO),
      reflectionsGain(AL_REVERB_DEFAULT_REFLECTIONS_GAIN),
      reflectionsDelay(AL_REVERB_DEFAULT_REFLECTIONS_DELAY),
      lateReverbGain(AL_REVERB_DEFAULT_LATE_REVERB_GAIN),
      lateReverbDelay(AL_REVERB_DEFAULT_LATE_REVERB_DELAY),
      airAbsorptionGainHF(AL_REVERB_DEFAULT_AIR_ABSORPTION_GAINHF),
      roomRolloffFactor(AL_REVERB_DEFAULT_ROOM_ROLLOFF_FACTOR),
      decayHFLimit(AL_REVERB_DEFAULT_DECAY_HFLIMIT)
{
}

AudioEffectProperties::EaxReverbProperties::EaxReverbProperties()
    : density(AL_EAXREVERB_DEFAULT_DENSITY),
      diffusion(AL_EAXREVERB_DEFAULT_DIFFUSION),
      gain(AL_EAXREVERB_DEFAULT_GAIN),
      gainHF(AL_EAXREVERB_DEFAULT_GAINHF),
      gainLF(AL_EAXREVERB_DEFAULT_GAINLF),
      decayTime(AL_EAXREVERB_DEFAULT_DECAY_TIME),
      decayHFRatio(AL_EAXREVERB_DEFAULT_DECAY_HFRATIO),
      decayLFRatio(AL_EAXREVERB_DEFAULT_DECAY_LFRATIO),
      reflectionsGain(AL_EAXREVERB_DEFAULT_REFLECTIONS_GAIN),
      reflectionsDelay(AL_EAXREVERB_DEFAULT_REFLECTIONS_DELAY),
      reflectionsPan{ AL_EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ,
	                  AL_EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ,
	                  AL_EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ },
      lateReverbGain(AL_EAXREVERB_DEFAULT_LATE_REVERB_GAIN),
      lateReverbDelay(AL_EAXREVERB_DEFAULT_LATE_REVERB_DELAY),
      lateReverbPan{ AL_EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ,
	                 AL_EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ,
	                 AL_EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ },
      echoTime(AL_EAXREVERB_DEFAULT_ECHO_TIME),
      echoDepth(AL_EAXREVERB_DEFAULT_ECHO_DEPTH),
      modulationTime(AL_EAXREVERB_DEFAULT_MODULATION_TIME),
      modulationDepth(AL_EAXREVERB_DEFAULT_MODULATION_DEPTH),
      airAbsorptionGainHF(AL_EAXREVERB_DEFAULT_AIR_ABSORPTION_GAINHF),
      hfReference(AL_EAXREVERB_DEFAULT_HFREFERENCE),
      lfReference(AL_EAXREVERB_DEFAULT_LFREFERENCE),
      roomRolloffFactor(AL_EAXREVERB_DEFAULT_ROOM_ROLLOFF_FACTOR),
      decayHFLimit(AL_EAXREVERB_DEFAULT_DECAY_HFLIMIT)
{
}

AudioEffectProperties::ChorusProperties::ChorusProperties()
    : waveform(AL_CHORUS_DEFAULT_WAVEFORM),
      phase(AL_CHORUS_DEFAULT_PHASE),
      rate(AL_CHORUS_DEFAULT_RATE),
      depth(AL_CHORUS_DEFAULT_DEPTH),
      feedback(AL_CHORUS_DEFAULT_FEEDBACK),
      delay(AL_CHORUS_DEFAULT_DELAY)
{
}

AudioEffectProperties::DistortionProperties::DistortionProperties()
    : edge(AL_DISTORTION_DEFAULT_EDGE),
      gain(AL_DISTORTION_DEFAULT_GAIN),
      lowpassCutoff(AL_DISTORTION_DEFAULT_LOWPASS_CUTOFF),
      eqCenter(AL_DISTORTION_DEFAULT_EQCENTER),
      eqBandwidth(AL_DISTORTION_DEFAULT_EQBANDWIDTH)
{
}

AudioEffectProperties::EchoProperties::EchoProperties()
    : delay(AL_ECHO_DEFAULT_DELAY),
      lrDelay(AL_ECHO_DEFAULT_LRDELAY),
      damping(AL_ECHO_DEFAULT_DAMPING),
      feedback(AL_ECHO_DEFAULT_FEEDBACK),
      spread(AL_ECHO_DEFAULT_SPREAD)
{
}

AudioEffectProperties::FlangerProperties::FlangerProperties()
    : waveform(AL_FLANGER_DEFAULT_WAVEFORM),
      phase(AL_FLANGER_DEFAULT_PHASE),
      rate(AL_FLANGER_DEFAULT_RATE),
      depth(AL_FLANGER_DEFAULT_DEPTH),
      feedback(AL_FLANGER_DEFAULT_FEEDBACK),
      delay(AL_FLANGER_DEFAULT_DELAY)
{
}

AudioEffectProperties::FrequencyShifterProperties::FrequencyShifterProperties()
    : frequency(AL_FREQUENCY_SHIFTER_DEFAULT_FREQUENCY),
      leftDirection(AL_FREQUENCY_SHIFTER_DEFAULT_LEFT_DIRECTION),
      rightDirection(AL_FREQUENCY_SHIFTER_DEFAULT_RIGHT_DIRECTION)
{
}

AudioEffectProperties::VocalMorpherProperties::VocalMorpherProperties()
    : phonemeA(AL_VOCAL_MORPHER_DEFAULT_PHONEMEA),
      phonemeACoarseTuning(AL_VOCAL_MORPHER_DEFAULT_PHONEMEA_COARSE_TUNING),
      phonemeB(AL_VOCAL_MORPHER_DEFAULT_PHONEMEB),
      phonemeBCoarseTuning(AL_VOCAL_MORPHER_DEFAULT_PHONEMEB_COARSE_TUNING),
      waveform(AL_VOCAL_MORPHER_DEFAULT_WAVEFORM),
      rate(AL_VOCAL_MORPHER_DEFAULT_RATE)
{
}

AudioEffectProperties::PitchShifterProperties::PitchShifterProperties()
    : coarseTune(AL_PITCH_SHIFTER_DEFAULT_COARSE_TUNE),
      fineTune(AL_PITCH_SHIFTER_DEFAULT_FINE_TUNE)
{
}

AudioEffectProperties::RingModulatorProperties::RingModulatorProperties()
    : frequency(AL_RING_MODULATOR_DEFAULT_FREQUENCY),
      highpassCutoff(AL_RING_MODULATOR_DEFAULT_HIGHPASS_CUTOFF),
      waveform(AL_RING_MODULATOR_DEFAULT_WAVEFORM)
{
}

AudioEffectProperties::AutoWahProperties::AutoWahProperties()
    : attackTime(AL_AUTOWAH_DEFAULT_ATTACK_TIME),
      releaseTime(AL_AUTOWAH_DEFAULT_RELEASE_TIME),
      resonance(AL_AUTOWAH_DEFAULT_RESONANCE),
      peakGain(AL_AUTOWAH_DEFAULT_PEAK_GAIN)
{
}

AudioEffectProperties::CompressorProperties::CompressorProperties()
    : onOff(AL_COMPRESSOR_DEFAULT_ONOFF)
{
}

AudioEffectProperties::EqualizerProperties::EqualizerProperties()
    : lowGain(AL_EQUALIZER_DEFAULT_LOW_GAIN),
      lowCutoff(AL_EQUALIZER_DEFAULT_LOW_CUTOFF),
      mid1Gain(AL_EQUALIZER_DEFAULT_MID1_GAIN),
      mid1Center(AL_EQUALIZER_DEFAULT_MID1_CENTER),
      mid1Width(AL_EQUALIZER_DEFAULT_MID1_WIDTH),
      mid2Gain(AL_EQUALIZER_DEFAULT_MID2_GAIN),
      mid2Center(AL_EQUALIZER_DEFAULT_MID2_CENTER),
      mid2Width(AL_EQUALIZER_DEFAULT_MID2_WIDTH),
      highGain(AL_EQUALIZER_DEFAULT_HIGH_GAIN),
      highCutoff(AL_EQUALIZER_DEFAULT_HIGH_CUTOFF)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AudioEffectProperties::setType(AudioEffect::Type type)
{
	if (type_ != type)
	{
		type_ = type;
		switch (type)
		{
			case AudioEffect::Type::NULL_EFFECT:
				memset(&propertiesData_, 0, sizeof(propertiesData_));
				break;
			case AudioEffect::Type::EAXREVERB:
				resetEaxReverbProperties();
				break;
			case AudioEffect::Type::REVERB:
				resetReverbProperties();
				break;
			case AudioEffect::Type::CHORUS:
				resetChorusProperties();
				break;
			case AudioEffect::Type::DISTORTION:
				resetDistortionProperties();
				break;
			case AudioEffect::Type::ECHO:
				resetEchoProperties();
				break;
			case AudioEffect::Type::FLANGER:
				resetFlangerProperties();
				break;
			case AudioEffect::Type::FREQUENCY_SHIFTER:
				resetFrequencyShifterProperties();
				break;
			case AudioEffect::Type::VOCAL_MORPHER:
				resetVocalMorpherProperties();
				break;
			case AudioEffect::Type::PITCH_SHIFTER:
				resetPitchShifterProperties();
				break;
			case AudioEffect::Type::RING_MODULATOR:
				resetRingModulatorProperties();
				break;
			case AudioEffect::Type::AUTOWAH:
				resetAutoWahProperties();
				break;
			case AudioEffect::Type::COMPRESSOR:
				resetCompressorProperties();
				break;
			case AudioEffect::Type::EQUALIZER:
				resetEqualizerProperties();
				break;
		}
	}
}

void AudioEffectProperties::EaxReverbProperties::loadPreset(const EfxReverbPresets preset)
{
	switch (preset)
	{
		// Default Presets
		case AudioEffectProperties::EfxReverbPresets::GENERIC: loadEfxReverbPreset(EFX_REVERB_PRESET_GENERIC, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PADDED_CELL: loadEfxReverbPreset(EFX_REVERB_PRESET_PADDEDCELL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_ROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::BATHROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_BATHROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::LIVING_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_LIVINGROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::STONE_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_STONEROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::AUDITORIUM: loadEfxReverbPreset(EFX_REVERB_PRESET_AUDITORIUM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CONCERT_HALL: loadEfxReverbPreset(EFX_REVERB_PRESET_CONCERTHALL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CAVE: loadEfxReverbPreset(EFX_REVERB_PRESET_CAVE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ARENA: loadEfxReverbPreset(EFX_REVERB_PRESET_ARENA, *this); break;
		case AudioEffectProperties::EfxReverbPresets::HANGAR: loadEfxReverbPreset(EFX_REVERB_PRESET_HANGAR, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CARPETED_HALLWAY: loadEfxReverbPreset(EFX_REVERB_PRESET_CARPETEDHALLWAY, *this); break;
		case AudioEffectProperties::EfxReverbPresets::HALLWAY: loadEfxReverbPreset(EFX_REVERB_PRESET_HALLWAY, *this); break;
		case AudioEffectProperties::EfxReverbPresets::STONE_CORRIDOR: loadEfxReverbPreset(EFX_REVERB_PRESET_STONECORRIDOR, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ALLEY: loadEfxReverbPreset(EFX_REVERB_PRESET_ALLEY, *this); break;
		case AudioEffectProperties::EfxReverbPresets::FOREST: loadEfxReverbPreset(EFX_REVERB_PRESET_FOREST, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CITY: loadEfxReverbPreset(EFX_REVERB_PRESET_CITY, *this); break;
		case AudioEffectProperties::EfxReverbPresets::MOUNTAINS: loadEfxReverbPreset(EFX_REVERB_PRESET_MOUNTAINS, *this); break;
		case AudioEffectProperties::EfxReverbPresets::QUARRY: loadEfxReverbPreset(EFX_REVERB_PRESET_QUARRY, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PLAIN: loadEfxReverbPreset(EFX_REVERB_PRESET_PLAIN, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PARKING_LOT: loadEfxReverbPreset(EFX_REVERB_PRESET_PARKINGLOT, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SEWER_PIPE: loadEfxReverbPreset(EFX_REVERB_PRESET_SEWERPIPE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::UNDERWATER: loadEfxReverbPreset(EFX_REVERB_PRESET_UNDERWATER, *this); break;
		case AudioEffectProperties::EfxReverbPresets::DRUGGED: loadEfxReverbPreset(EFX_REVERB_PRESET_DRUGGED, *this); break;
		case AudioEffectProperties::EfxReverbPresets::DIZZY: loadEfxReverbPreset(EFX_REVERB_PRESET_DIZZY, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PSYCHOTIC: loadEfxReverbPreset(EFX_REVERB_PRESET_PSYCHOTIC, *this); break;
		// Castle Presets
		case AudioEffectProperties::EfxReverbPresets::CASTLE_SMALL_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_CASTLE_SMALLROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CASTLE_SHORT_PASSAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_CASTLE_SHORTPASSAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CASTLE_MEDIUM_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_CASTLE_MEDIUMROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CASTLE_LARGE_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_CASTLE_LARGEROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CASTLE_LONG_PASSAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_CASTLE_LONGPASSAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CASTLE_HALL: loadEfxReverbPreset(EFX_REVERB_PRESET_CASTLE_HALL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CASTLE_CUPBOARD: loadEfxReverbPreset(EFX_REVERB_PRESET_CASTLE_CUPBOARD, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CASTLE_COURTYARD: loadEfxReverbPreset(EFX_REVERB_PRESET_CASTLE_COURTYARD, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CASTLE_ALCOVE: loadEfxReverbPreset(EFX_REVERB_PRESET_CASTLE_ALCOVE, *this); break;
		// Factory Presets
		case AudioEffectProperties::EfxReverbPresets::FACTORY_SMALL_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_FACTORY_SMALLROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::FACTORY_SHORT_PASSAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_FACTORY_SHORTPASSAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::FACTORY_MEDIUM_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_FACTORY_MEDIUMROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::FACTORY_LARGE_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_FACTORY_LARGEROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::FACTORY_LONG_PASSAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_FACTORY_LONGPASSAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::FACTORY_HALL: loadEfxReverbPreset(EFX_REVERB_PRESET_FACTORY_HALL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::FACTORY_CUPBOARD: loadEfxReverbPreset(EFX_REVERB_PRESET_FACTORY_CUPBOARD, *this); break;
		case AudioEffectProperties::EfxReverbPresets::FACTORY_COURTYARD: loadEfxReverbPreset(EFX_REVERB_PRESET_FACTORY_COURTYARD, *this); break;
		case AudioEffectProperties::EfxReverbPresets::FACTORY_ALCOVE: loadEfxReverbPreset(EFX_REVERB_PRESET_FACTORY_ALCOVE, *this); break;
		// Ice Palace Presets
		case AudioEffectProperties::EfxReverbPresets::ICEPALACE_SMALL_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_ICEPALACE_SMALLROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ICEPALACE_SHORT_PASSAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_ICEPALACE_SHORTPASSAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ICEPALACE_MEDIUM_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_ICEPALACE_MEDIUMROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ICEPALACE_LARGE_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_ICEPALACE_LARGEROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ICEPALACE_LONG_PASSAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_ICEPALACE_LONGPASSAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ICEPALACE_HALL: loadEfxReverbPreset(EFX_REVERB_PRESET_ICEPALACE_HALL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ICEPALACE_CUPBOARD: loadEfxReverbPreset(EFX_REVERB_PRESET_ICEPALACE_CUPBOARD, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ICEPALACE_COURTYARD: loadEfxReverbPreset(EFX_REVERB_PRESET_ICEPALACE_COURTYARD, *this); break;
		case AudioEffectProperties::EfxReverbPresets::ICEPALACE_ALCOVE: loadEfxReverbPreset(EFX_REVERB_PRESET_ICEPALACE_ALCOVE, *this); break;
		// Space Station Presets
		case AudioEffectProperties::EfxReverbPresets::SPACE_STATION_SMALL_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_SPACESTATION_SMALLROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPACE_STATION_SHORT_PASSAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_SPACESTATION_SHORTPASSAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPACE_STATION_MEDIUM_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPACE_STATION_LARGE_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_SPACESTATION_LARGEROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPACE_STATION_LONG_PASSAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_SPACESTATION_LONGPASSAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPACE_STATION_HALL: loadEfxReverbPreset(EFX_REVERB_PRESET_SPACESTATION_HALL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPACE_STATION_CUPBOARD: loadEfxReverbPreset(EFX_REVERB_PRESET_SPACESTATION_CUPBOARD, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPACE_STATION_ALCOVE: loadEfxReverbPreset(EFX_REVERB_PRESET_SPACESTATION_ALCOVE, *this); break;
		// Wooden Galleon Presets
		case AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_SMALL_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_WOODEN_SMALLROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_SHORT_PASSAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_WOODEN_SHORTPASSAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_MEDIUM_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_WOODEN_MEDIUMROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_LARGE_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_WOODEN_LARGEROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_LONG_PASSAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_WOODEN_LONGPASSAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_HALL: loadEfxReverbPreset(EFX_REVERB_PRESET_WOODEN_HALL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_CUPBOARD: loadEfxReverbPreset(EFX_REVERB_PRESET_WOODEN_CUPBOARD, *this); break;
		case AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_COURTYARD: loadEfxReverbPreset(EFX_REVERB_PRESET_WOODEN_COURTYARD, *this); break;
		case AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_ALCOVE: loadEfxReverbPreset(EFX_REVERB_PRESET_WOODEN_ALCOVE, *this); break;
		// Sports Presets
		case AudioEffectProperties::EfxReverbPresets::SPORT_EMPTY_STADIUM: loadEfxReverbPreset(EFX_REVERB_PRESET_SPORT_EMPTYSTADIUM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPORT_SQUASH_COURT: loadEfxReverbPreset(EFX_REVERB_PRESET_SPORT_SQUASHCOURT, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPORT_SMALL_SWIMMING_POOL: loadEfxReverbPreset(EFX_REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPORT_LARGE_SWIMMING_POOL: loadEfxReverbPreset(EFX_REVERB_PRESET_SPORT_LARGESWIMMINGPOOL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPORT_GYMNASIUM: loadEfxReverbPreset(EFX_REVERB_PRESET_SPORT_GYMNASIUM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPORT_FULL_STADIUM: loadEfxReverbPreset(EFX_REVERB_PRESET_SPORT_FULLSTADIUM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SPORT_STADIUM_TANNOY: loadEfxReverbPreset(EFX_REVERB_PRESET_SPORT_STADIUMTANNOY, *this); break;
		// Prefab Presets
		case AudioEffectProperties::EfxReverbPresets::PREFAB_WORKSHOP: loadEfxReverbPreset(EFX_REVERB_PRESET_PREFAB_WORKSHOP, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PREFAB_SCHOOL_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_PREFAB_SCHOOLROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PREFAB_PRACTISE_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_PREFAB_PRACTISEROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PREFAB_OUTHOUSE: loadEfxReverbPreset(EFX_REVERB_PRESET_PREFAB_OUTHOUSE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PREFAB_CARAVAN: loadEfxReverbPreset(EFX_REVERB_PRESET_PREFAB_CARAVAN, *this); break;
		// Dome and Pipe Presets
		case AudioEffectProperties::EfxReverbPresets::DOME_TOMB: loadEfxReverbPreset(EFX_REVERB_PRESET_DOME_TOMB, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PIPE_SMALL: loadEfxReverbPreset(EFX_REVERB_PRESET_PIPE_SMALL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::DOME_SAINT_PAULS: loadEfxReverbPreset(EFX_REVERB_PRESET_DOME_SAINTPAULS, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PIPE_LONGTHIN: loadEfxReverbPreset(EFX_REVERB_PRESET_PIPE_LONGTHIN, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PIPE_LARGE: loadEfxReverbPreset(EFX_REVERB_PRESET_PIPE_LARGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::PIPE_RESONANT: loadEfxReverbPreset(EFX_REVERB_PRESET_PIPE_RESONANT, *this); break;
		// Outdoors Presets
		case AudioEffectProperties::EfxReverbPresets::OUTDOORS_BACKYARD: loadEfxReverbPreset(EFX_REVERB_PRESET_OUTDOORS_BACKYARD, *this); break;
		case AudioEffectProperties::EfxReverbPresets::OUTDOORS_ROLLING_PLAINS: loadEfxReverbPreset(EFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS, *this); break;
		case AudioEffectProperties::EfxReverbPresets::OUTDOORS_DEEP_CANYON: loadEfxReverbPreset(EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON, *this); break;
		case AudioEffectProperties::EfxReverbPresets::OUTDOORS_CREEK: loadEfxReverbPreset(EFX_REVERB_PRESET_OUTDOORS_CREEK, *this); break;
		case AudioEffectProperties::EfxReverbPresets::OUTDOORS_VALLEY: loadEfxReverbPreset(EFX_REVERB_PRESET_OUTDOORS_VALLEY, *this); break;
		// Mood Preset
		case AudioEffectProperties::EfxReverbPresets::MOOD_HEAVEN: loadEfxReverbPreset(EFX_REVERB_PRESET_MOOD_HEAVEN, *this); break;
		case AudioEffectProperties::EfxReverbPresets::MOOD_HELL: loadEfxReverbPreset(EFX_REVERB_PRESET_MOOD_HELL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::MOOD_MEMORY: loadEfxReverbPreset(EFX_REVERB_PRESET_MOOD_MEMORY, *this); break;
		// Driving Presets
		case AudioEffectProperties::EfxReverbPresets::DRIVING_COMMENTATOR: loadEfxReverbPreset(EFX_REVERB_PRESET_DRIVING_COMMENTATOR, *this); break;
		case AudioEffectProperties::EfxReverbPresets::DRIVING_PIT_GARAGE: loadEfxReverbPreset(EFX_REVERB_PRESET_DRIVING_PITGARAGE, *this); break;
		case AudioEffectProperties::EfxReverbPresets::DRIVING_INCAR_RACER: loadEfxReverbPreset(EFX_REVERB_PRESET_DRIVING_INCAR_RACER, *this); break;
		case AudioEffectProperties::EfxReverbPresets::DRIVING_INCAR_SPORTS: loadEfxReverbPreset(EFX_REVERB_PRESET_DRIVING_INCAR_SPORTS, *this); break;
		case AudioEffectProperties::EfxReverbPresets::DRIVING_INCAR_LUXURY: loadEfxReverbPreset(EFX_REVERB_PRESET_DRIVING_INCAR_LUXURY, *this); break;
		case AudioEffectProperties::EfxReverbPresets::DRIVING_FULL_GRANDSTAND: loadEfxReverbPreset(EFX_REVERB_PRESET_DRIVING_FULLGRANDSTAND, *this); break;
		case AudioEffectProperties::EfxReverbPresets::DRIVING_EMPTY_GRANDSTAND: loadEfxReverbPreset(EFX_REVERB_PRESET_DRIVING_EMPTYGRANDSTAND, *this); break;
		case AudioEffectProperties::EfxReverbPresets::DRIVING_TUNNEL: loadEfxReverbPreset(EFX_REVERB_PRESET_DRIVING_TUNNEL, *this); break;
		// City Presets
		case AudioEffectProperties::EfxReverbPresets::CITY_STREETS: loadEfxReverbPreset(EFX_REVERB_PRESET_CITY_STREETS, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CITY_SUBWAY: loadEfxReverbPreset(EFX_REVERB_PRESET_CITY_SUBWAY, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CITY_MUSEUM: loadEfxReverbPreset(EFX_REVERB_PRESET_CITY_MUSEUM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CITY_LIBRARY: loadEfxReverbPreset(EFX_REVERB_PRESET_CITY_LIBRARY, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CITY_UNDERPASS: loadEfxReverbPreset(EFX_REVERB_PRESET_CITY_UNDERPASS, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CITY_ABANDONED: loadEfxReverbPreset(EFX_REVERB_PRESET_CITY_ABANDONED, *this); break;
		// Misc. Presets
		case AudioEffectProperties::EfxReverbPresets::DUSTY_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_DUSTYROOM, *this); break;
		case AudioEffectProperties::EfxReverbPresets::CHAPEL: loadEfxReverbPreset(EFX_REVERB_PRESET_CHAPEL, *this); break;
		case AudioEffectProperties::EfxReverbPresets::SMALL_WATER_ROOM: loadEfxReverbPreset(EFX_REVERB_PRESET_SMALLWATERROOM, *this); break;
		case COUNT: break;
	}
}

//////////////////////////////
// Reverb
//////////////////////////////

AudioEffectProperties::ReverbProperties AudioEffectProperties::minReverbProperties()
{
	ReverbProperties reverbProperties;

	reverbProperties.density = AL_REVERB_MIN_DENSITY;
	reverbProperties.diffusion = AL_REVERB_MIN_DIFFUSION;
	reverbProperties.gain = AL_REVERB_MIN_GAIN;
	reverbProperties.gainHF = AL_REVERB_MIN_GAINHF;
	reverbProperties.decayTime = AL_REVERB_MIN_DECAY_TIME;
	reverbProperties.decayHFRatio = AL_REVERB_MIN_DECAY_HFRATIO;
	reverbProperties.reflectionsGain = AL_REVERB_MIN_REFLECTIONS_GAIN;
	reverbProperties.reflectionsDelay = AL_REVERB_MIN_REFLECTIONS_DELAY;
	reverbProperties.lateReverbGain = AL_REVERB_MIN_LATE_REVERB_GAIN;
	reverbProperties.lateReverbDelay = AL_REVERB_MIN_LATE_REVERB_DELAY;
	reverbProperties.airAbsorptionGainHF = AL_REVERB_MIN_AIR_ABSORPTION_GAINHF;
	reverbProperties.roomRolloffFactor = AL_REVERB_MIN_ROOM_ROLLOFF_FACTOR;
	reverbProperties.decayHFLimit = AL_REVERB_MIN_DECAY_HFLIMIT;

	return reverbProperties;
}

AudioEffectProperties::ReverbProperties AudioEffectProperties::maxReverbProperties()
{
	ReverbProperties reverbProperties;

	reverbProperties.density = AL_REVERB_MAX_DENSITY;
	reverbProperties.diffusion = AL_REVERB_MAX_DIFFUSION;
	reverbProperties.gain = AL_REVERB_MAX_GAIN;
	reverbProperties.gainHF = AL_REVERB_MAX_GAINHF;
	reverbProperties.decayTime = AL_REVERB_MAX_DECAY_TIME;
	reverbProperties.decayHFRatio = AL_REVERB_MAX_DECAY_HFRATIO;
	reverbProperties.reflectionsGain = AL_REVERB_MAX_REFLECTIONS_GAIN;
	reverbProperties.reflectionsDelay = AL_REVERB_MAX_REFLECTIONS_DELAY;
	reverbProperties.lateReverbGain = AL_REVERB_MAX_LATE_REVERB_GAIN;
	reverbProperties.lateReverbDelay = AL_REVERB_MAX_LATE_REVERB_DELAY;
	reverbProperties.airAbsorptionGainHF = AL_REVERB_MAX_AIR_ABSORPTION_GAINHF;
	reverbProperties.roomRolloffFactor = AL_REVERB_MAX_ROOM_ROLLOFF_FACTOR;
	reverbProperties.decayHFLimit = AL_REVERB_MAX_DECAY_HFLIMIT;

	return reverbProperties;
}

const AudioEffectProperties::ReverbProperties &AudioEffectProperties::reverbProperties() const
{
	checkType(AudioEffect::Type::REVERB);
	const ReverbProperties &reverbProperties = reinterpret_cast<const ReverbProperties &>(propertiesData_);
	return reverbProperties;
}

AudioEffectProperties::ReverbProperties &AudioEffectProperties::reverbProperties()
{
	checkType(AudioEffect::Type::REVERB);
	ReverbProperties &reverbProperties = reinterpret_cast<ReverbProperties &>(propertiesData_);
	return reverbProperties;
}

void AudioEffectProperties::setReverbProperties(const ReverbProperties &newReverbProperties)
{
	if (checkType(AudioEffect::Type::REVERB))
	{
		ReverbProperties &reverbProperties = reinterpret_cast<ReverbProperties &>(propertiesData_);
		reverbProperties = newReverbProperties;

		reverbProperties.density = nctl::clamp(reverbProperties.density, AL_REVERB_MIN_DENSITY, AL_REVERB_MAX_DENSITY);
		reverbProperties.diffusion = nctl::clamp(reverbProperties.diffusion, AL_REVERB_MIN_DIFFUSION, AL_REVERB_MAX_DIFFUSION);
		reverbProperties.gain = nctl::clamp(reverbProperties.gain, AL_REVERB_MIN_GAIN, AL_REVERB_MAX_GAIN);
		reverbProperties.gainHF = nctl::clamp(reverbProperties.gainHF, AL_REVERB_MIN_GAINHF, AL_REVERB_MAX_GAINHF);
		reverbProperties.decayTime = nctl::clamp(reverbProperties.decayTime, AL_REVERB_MIN_DECAY_TIME, AL_REVERB_MAX_DECAY_TIME);
		reverbProperties.decayHFRatio = nctl::clamp(reverbProperties.decayHFRatio, AL_REVERB_MIN_DECAY_HFRATIO, AL_REVERB_MAX_DECAY_HFRATIO);
		reverbProperties.reflectionsGain = nctl::clamp(reverbProperties.reflectionsGain, AL_REVERB_MIN_REFLECTIONS_GAIN, AL_REVERB_MAX_REFLECTIONS_GAIN);
		reverbProperties.reflectionsDelay = nctl::clamp(reverbProperties.reflectionsDelay, AL_REVERB_MIN_REFLECTIONS_DELAY, AL_REVERB_MAX_REFLECTIONS_DELAY);
		reverbProperties.lateReverbGain = nctl::clamp(reverbProperties.lateReverbGain, AL_REVERB_MIN_LATE_REVERB_GAIN, AL_REVERB_MAX_LATE_REVERB_GAIN);
		reverbProperties.lateReverbDelay = nctl::clamp(reverbProperties.lateReverbDelay, AL_REVERB_MIN_LATE_REVERB_DELAY, AL_REVERB_MAX_LATE_REVERB_DELAY);
		reverbProperties.airAbsorptionGainHF = nctl::clamp(reverbProperties.airAbsorptionGainHF, AL_REVERB_MIN_AIR_ABSORPTION_GAINHF, AL_REVERB_MAX_AIR_ABSORPTION_GAINHF);
		reverbProperties.roomRolloffFactor = nctl::clamp(reverbProperties.roomRolloffFactor, AL_REVERB_MIN_ROOM_ROLLOFF_FACTOR, AL_REVERB_MAX_ROOM_ROLLOFF_FACTOR);
		reverbProperties.decayHFLimit = nctl::clamp(reverbProperties.decayHFLimit, AL_REVERB_MIN_DECAY_HFLIMIT, AL_REVERB_MAX_DECAY_HFLIMIT);
	}
}

void AudioEffectProperties::resetReverbProperties()
{
	if (checkType(AudioEffect::Type::REVERB))
	{
		ReverbProperties &reverbProperties = reinterpret_cast<ReverbProperties &>(propertiesData_);
		reverbProperties = ReverbProperties();
	}
}

//////////////////////////////
// EAX Reverb
//////////////////////////////

AudioEffectProperties::EaxReverbProperties AudioEffectProperties::minEaxReverbProperties()
{
	EaxReverbProperties eaxReverbProperties;

	eaxReverbProperties.density = AL_EAXREVERB_MIN_DENSITY;
	eaxReverbProperties.diffusion = AL_EAXREVERB_MIN_DIFFUSION;
	eaxReverbProperties.gain = AL_EAXREVERB_MIN_GAIN;
	eaxReverbProperties.gainHF = AL_EAXREVERB_MIN_GAINHF;
	eaxReverbProperties.gainLF = AL_EAXREVERB_MIN_GAINLF;
	eaxReverbProperties.decayTime = AL_EAXREVERB_MIN_DECAY_TIME;
	eaxReverbProperties.decayHFRatio = AL_EAXREVERB_MIN_DECAY_HFRATIO;
	eaxReverbProperties.decayLFRatio = AL_EAXREVERB_MIN_DECAY_LFRATIO;
	eaxReverbProperties.reflectionsGain = AL_EAXREVERB_MIN_REFLECTIONS_GAIN;
	eaxReverbProperties.reflectionsDelay = AL_EAXREVERB_MIN_REFLECTIONS_DELAY;
	eaxReverbProperties.reflectionsPan[0] = EaxReverbProperties::MinPan;
	eaxReverbProperties.reflectionsPan[1] = EaxReverbProperties::MinPan;
	eaxReverbProperties.reflectionsPan[2] = EaxReverbProperties::MinPan;
	eaxReverbProperties.lateReverbGain = AL_EAXREVERB_MIN_LATE_REVERB_GAIN;
	eaxReverbProperties.lateReverbDelay = AL_EAXREVERB_MIN_LATE_REVERB_DELAY;
	eaxReverbProperties.lateReverbPan[0] = EaxReverbProperties::MinPan;
	eaxReverbProperties.lateReverbPan[1] = EaxReverbProperties::MinPan;
	eaxReverbProperties.lateReverbPan[2] = EaxReverbProperties::MinPan;
	eaxReverbProperties.echoTime = AL_EAXREVERB_MIN_ECHO_TIME;
	eaxReverbProperties.echoDepth = AL_EAXREVERB_MIN_ECHO_DEPTH;
	eaxReverbProperties.modulationTime = AL_EAXREVERB_MIN_MODULATION_TIME;
	eaxReverbProperties.modulationDepth = AL_EAXREVERB_MIN_MODULATION_DEPTH;
	eaxReverbProperties.airAbsorptionGainHF = AL_EAXREVERB_MIN_AIR_ABSORPTION_GAINHF;
	eaxReverbProperties.hfReference = AL_EAXREVERB_MIN_HFREFERENCE;
	eaxReverbProperties.lfReference = AL_EAXREVERB_MIN_LFREFERENCE;
	eaxReverbProperties.roomRolloffFactor = AL_EAXREVERB_MIN_ROOM_ROLLOFF_FACTOR;
	eaxReverbProperties.decayHFLimit = AL_EAXREVERB_MIN_DECAY_HFLIMIT;

	return eaxReverbProperties;
}

AudioEffectProperties::EaxReverbProperties AudioEffectProperties::maxEaxReverbProperties()
{
	EaxReverbProperties eaxReverbProperties;

	eaxReverbProperties.density = AL_EAXREVERB_MAX_DENSITY;
	eaxReverbProperties.diffusion = AL_EAXREVERB_MAX_DIFFUSION;
	eaxReverbProperties.gain = AL_EAXREVERB_MAX_GAIN;
	eaxReverbProperties.gainHF = AL_EAXREVERB_MAX_GAINHF;
	eaxReverbProperties.gainLF = AL_EAXREVERB_MAX_GAINLF;
	eaxReverbProperties.decayTime = AL_EAXREVERB_MAX_DECAY_TIME;
	eaxReverbProperties.decayHFRatio = AL_EAXREVERB_MAX_DECAY_HFRATIO;
	eaxReverbProperties.decayLFRatio = AL_EAXREVERB_MAX_DECAY_LFRATIO;
	eaxReverbProperties.reflectionsGain = AL_EAXREVERB_MAX_REFLECTIONS_GAIN;
	eaxReverbProperties.reflectionsDelay = AL_EAXREVERB_MAX_REFLECTIONS_DELAY;
	eaxReverbProperties.reflectionsPan[0] = EaxReverbProperties::MaxPan;
	eaxReverbProperties.reflectionsPan[1] = EaxReverbProperties::MaxPan;
	eaxReverbProperties.reflectionsPan[2] = EaxReverbProperties::MaxPan;
	eaxReverbProperties.lateReverbGain = AL_EAXREVERB_MAX_LATE_REVERB_GAIN;
	eaxReverbProperties.lateReverbDelay = AL_EAXREVERB_MAX_LATE_REVERB_DELAY;
	eaxReverbProperties.lateReverbPan[0] = EaxReverbProperties::MaxPan;
	eaxReverbProperties.lateReverbPan[1] = EaxReverbProperties::MaxPan;
	eaxReverbProperties.lateReverbPan[2] = EaxReverbProperties::MaxPan;
	eaxReverbProperties.echoTime = AL_EAXREVERB_MAX_ECHO_TIME;
	eaxReverbProperties.echoDepth = AL_EAXREVERB_MAX_ECHO_DEPTH;
	eaxReverbProperties.modulationTime = AL_EAXREVERB_MAX_MODULATION_TIME;
	eaxReverbProperties.modulationDepth = AL_EAXREVERB_MAX_MODULATION_DEPTH;
	eaxReverbProperties.airAbsorptionGainHF = AL_EAXREVERB_MAX_AIR_ABSORPTION_GAINHF;
	eaxReverbProperties.hfReference = AL_EAXREVERB_MAX_HFREFERENCE;
	eaxReverbProperties.lfReference = AL_EAXREVERB_MAX_LFREFERENCE;
	eaxReverbProperties.roomRolloffFactor = AL_EAXREVERB_MAX_ROOM_ROLLOFF_FACTOR;
	eaxReverbProperties.decayHFLimit = AL_EAXREVERB_MAX_DECAY_HFLIMIT;

	return eaxReverbProperties;
}

const AudioEffectProperties::EaxReverbProperties &AudioEffectProperties::eaxReverbProperties() const
{
	checkType(AudioEffect::Type::EAXREVERB);
	const EaxReverbProperties &eaxReverbProperties = reinterpret_cast<const EaxReverbProperties &>(propertiesData_);
	return eaxReverbProperties;
}

AudioEffectProperties::EaxReverbProperties &AudioEffectProperties::eaxReverbProperties()
{
	checkType(AudioEffect::Type::EAXREVERB);
	EaxReverbProperties &eaxReverbProperties = reinterpret_cast<EaxReverbProperties &>(propertiesData_);
	return eaxReverbProperties;
}

void AudioEffectProperties::setEaxReverbProperties(const EaxReverbProperties &newEaxReverbProperties)
{
	if (checkType(AudioEffect::Type::EAXREVERB))
	{
		EaxReverbProperties &eaxReverbProperties = reinterpret_cast<EaxReverbProperties &>(propertiesData_);
		eaxReverbProperties = newEaxReverbProperties;

		eaxReverbProperties.density = nctl::clamp(eaxReverbProperties.density, AL_EAXREVERB_MIN_DENSITY, AL_EAXREVERB_MAX_DENSITY);
		eaxReverbProperties.diffusion = nctl::clamp(eaxReverbProperties.diffusion, AL_EAXREVERB_MIN_DIFFUSION, AL_EAXREVERB_MAX_DIFFUSION);
		eaxReverbProperties.gain = nctl::clamp(eaxReverbProperties.gain, AL_EAXREVERB_MIN_GAIN, AL_EAXREVERB_MAX_GAIN);
		eaxReverbProperties.gainHF = nctl::clamp(eaxReverbProperties.gainHF, AL_EAXREVERB_MIN_GAINHF, AL_EAXREVERB_MAX_GAINHF);
		eaxReverbProperties.gainLF = nctl::clamp(eaxReverbProperties.gainLF, AL_EAXREVERB_MIN_GAINLF, AL_EAXREVERB_MAX_GAINLF);
		eaxReverbProperties.decayTime = nctl::clamp(eaxReverbProperties.decayTime, AL_EAXREVERB_MIN_DECAY_TIME, AL_EAXREVERB_MAX_DECAY_TIME);
		eaxReverbProperties.decayHFRatio = nctl::clamp(eaxReverbProperties.decayHFRatio, AL_EAXREVERB_MIN_DECAY_HFRATIO, AL_EAXREVERB_MAX_DECAY_HFRATIO);
		eaxReverbProperties.decayLFRatio = nctl::clamp(eaxReverbProperties.decayLFRatio, AL_EAXREVERB_MIN_DECAY_LFRATIO, AL_EAXREVERB_MAX_DECAY_LFRATIO);
		eaxReverbProperties.reflectionsGain = nctl::clamp(eaxReverbProperties.reflectionsGain, AL_EAXREVERB_MIN_REFLECTIONS_GAIN, AL_EAXREVERB_MAX_REFLECTIONS_GAIN);
		eaxReverbProperties.reflectionsDelay = nctl::clamp(eaxReverbProperties.reflectionsDelay, AL_EAXREVERB_MIN_REFLECTIONS_DELAY, AL_EAXREVERB_MAX_REFLECTIONS_DELAY);
		eaxReverbProperties.reflectionsPan[0] = nctl::clamp(eaxReverbProperties.reflectionsPan[0], EaxReverbProperties::MinPan, EaxReverbProperties::MaxPan);
		eaxReverbProperties.reflectionsPan[1] = nctl::clamp(eaxReverbProperties.reflectionsPan[1], EaxReverbProperties::MinPan, EaxReverbProperties::MaxPan);
		eaxReverbProperties.reflectionsPan[2] = nctl::clamp(eaxReverbProperties.reflectionsPan[2], EaxReverbProperties::MinPan, EaxReverbProperties::MaxPan);
		eaxReverbProperties.lateReverbGain = nctl::clamp(eaxReverbProperties.lateReverbGain, AL_EAXREVERB_MIN_LATE_REVERB_GAIN, AL_EAXREVERB_MAX_LATE_REVERB_GAIN);
		eaxReverbProperties.lateReverbDelay = nctl::clamp(eaxReverbProperties.lateReverbDelay, AL_EAXREVERB_MIN_LATE_REVERB_DELAY, AL_EAXREVERB_MAX_LATE_REVERB_DELAY);
		eaxReverbProperties.lateReverbPan[0] = nctl::clamp(eaxReverbProperties.lateReverbPan[0], EaxReverbProperties::MinPan, EaxReverbProperties::MaxPan);
		eaxReverbProperties.lateReverbPan[1] = nctl::clamp(eaxReverbProperties.lateReverbPan[1], EaxReverbProperties::MinPan, EaxReverbProperties::MaxPan);
		eaxReverbProperties.lateReverbPan[2] = nctl::clamp(eaxReverbProperties.lateReverbPan[2], EaxReverbProperties::MinPan, EaxReverbProperties::MaxPan);
		eaxReverbProperties.echoTime = nctl::clamp(eaxReverbProperties.echoTime, AL_EAXREVERB_MIN_ECHO_TIME, AL_EAXREVERB_MAX_ECHO_TIME);
		eaxReverbProperties.echoDepth = nctl::clamp(eaxReverbProperties.echoDepth, AL_EAXREVERB_MIN_ECHO_DEPTH, AL_EAXREVERB_MAX_ECHO_DEPTH);
		eaxReverbProperties.modulationTime = nctl::clamp(eaxReverbProperties.modulationTime, AL_EAXREVERB_MIN_MODULATION_TIME, AL_EAXREVERB_MAX_MODULATION_TIME);
		eaxReverbProperties.modulationDepth = nctl::clamp(eaxReverbProperties.modulationDepth, AL_EAXREVERB_MIN_MODULATION_DEPTH, AL_EAXREVERB_MAX_MODULATION_DEPTH);
		eaxReverbProperties.airAbsorptionGainHF = nctl::clamp(eaxReverbProperties.airAbsorptionGainHF, AL_EAXREVERB_MIN_AIR_ABSORPTION_GAINHF, AL_EAXREVERB_MAX_AIR_ABSORPTION_GAINHF);
		eaxReverbProperties.hfReference = nctl::clamp(eaxReverbProperties.hfReference, AL_EAXREVERB_MIN_HFREFERENCE, AL_EAXREVERB_MAX_HFREFERENCE);
		eaxReverbProperties.lfReference = nctl::clamp(eaxReverbProperties.lfReference, AL_EAXREVERB_MIN_LFREFERENCE, AL_EAXREVERB_MAX_LFREFERENCE);
		eaxReverbProperties.roomRolloffFactor = nctl::clamp(eaxReverbProperties.roomRolloffFactor, AL_EAXREVERB_MIN_ROOM_ROLLOFF_FACTOR, AL_EAXREVERB_MAX_ROOM_ROLLOFF_FACTOR);
		eaxReverbProperties.decayHFLimit = nctl::clamp(eaxReverbProperties.decayHFLimit, AL_EAXREVERB_MIN_DECAY_HFLIMIT, AL_EAXREVERB_MAX_DECAY_HFLIMIT);
	}
}

void AudioEffectProperties::resetEaxReverbProperties()
{
	if (checkType(AudioEffect::Type::EAXREVERB))
	{
		EaxReverbProperties &eaxReverbProperties = reinterpret_cast<EaxReverbProperties &>(propertiesData_);
		eaxReverbProperties = EaxReverbProperties();
	}
}

//////////////////////////////
// Chorus
//////////////////////////////

AudioEffectProperties::ChorusProperties AudioEffectProperties::minChorusProperties()
{
	ChorusProperties chorusProperties;

	chorusProperties.waveform = AL_CHORUS_MIN_WAVEFORM;
	chorusProperties.phase = AL_CHORUS_MIN_PHASE;
	chorusProperties.rate = AL_CHORUS_MIN_RATE;
	chorusProperties.depth = AL_CHORUS_MIN_DEPTH;
	chorusProperties.feedback = AL_CHORUS_MIN_FEEDBACK;
	chorusProperties.delay = AL_CHORUS_MIN_DELAY;

	return chorusProperties;
}

AudioEffectProperties::ChorusProperties AudioEffectProperties::maxChorusProperties()
{
	ChorusProperties chorusProperties;

	chorusProperties.waveform = AL_CHORUS_MAX_WAVEFORM;
	chorusProperties.phase = AL_CHORUS_MAX_PHASE;
	chorusProperties.rate = AL_CHORUS_MAX_RATE;
	chorusProperties.depth = AL_CHORUS_MAX_DEPTH;
	chorusProperties.feedback = AL_CHORUS_MAX_FEEDBACK;
	chorusProperties.delay = AL_CHORUS_MAX_DELAY;

	return chorusProperties;
}

const AudioEffectProperties::ChorusProperties &AudioEffectProperties::chorusProperties() const
{
	checkType(AudioEffect::Type::CHORUS);
	const ChorusProperties &chorusProperties = reinterpret_cast<const ChorusProperties &>(propertiesData_);
	return chorusProperties;
}

AudioEffectProperties::ChorusProperties &AudioEffectProperties::chorusProperties()
{
	checkType(AudioEffect::Type::CHORUS);
	ChorusProperties &chorusProperties = reinterpret_cast<ChorusProperties &>(propertiesData_);
	return chorusProperties;
}

void AudioEffectProperties::setChorusProperties(const ChorusProperties &newChorusProperties)
{
	if (checkType(AudioEffect::Type::CHORUS))
	{
		ChorusProperties &chorusProperties = reinterpret_cast<ChorusProperties &>(propertiesData_);
		chorusProperties = newChorusProperties;

		chorusProperties.waveform = nctl::clamp(chorusProperties.waveform, AL_CHORUS_MIN_WAVEFORM, AL_CHORUS_MAX_WAVEFORM);
		chorusProperties.phase = nctl::clamp(chorusProperties.phase, AL_CHORUS_MIN_PHASE, AL_CHORUS_MAX_PHASE);
		chorusProperties.rate = nctl::clamp(chorusProperties.rate, AL_CHORUS_MIN_RATE, AL_CHORUS_MAX_RATE);
		chorusProperties.depth = nctl::clamp(chorusProperties.depth, AL_CHORUS_MIN_DEPTH, AL_CHORUS_MAX_DEPTH);
		chorusProperties.feedback = nctl::clamp(chorusProperties.feedback, AL_CHORUS_MIN_FEEDBACK, AL_CHORUS_MAX_FEEDBACK);
		chorusProperties.delay = nctl::clamp(chorusProperties.delay, AL_CHORUS_MIN_DELAY, AL_CHORUS_MAX_DELAY);
	}
}

void AudioEffectProperties::resetChorusProperties()
{
	if (checkType(AudioEffect::Type::CHORUS))
	{
		ChorusProperties &chorusProperties = reinterpret_cast<ChorusProperties &>(propertiesData_);
		chorusProperties = ChorusProperties();
	}
}

//////////////////////////////
// Distortion
//////////////////////////////

AudioEffectProperties::DistortionProperties AudioEffectProperties::minDistortionProperties()
{
	DistortionProperties distortionProperties;

	distortionProperties.edge = AL_DISTORTION_MIN_EDGE;
	distortionProperties.gain = AL_DISTORTION_MIN_GAIN;
	distortionProperties.lowpassCutoff = AL_DISTORTION_MIN_LOWPASS_CUTOFF;
	distortionProperties.eqCenter = AL_DISTORTION_MIN_EQCENTER;
	distortionProperties.eqBandwidth = AL_DISTORTION_MIN_EQBANDWIDTH;

	return distortionProperties;
}

AudioEffectProperties::DistortionProperties AudioEffectProperties::maxDistortionProperties()
{
	DistortionProperties distortionProperties;

	distortionProperties.edge = AL_DISTORTION_MAX_EDGE;
	distortionProperties.gain = AL_DISTORTION_MAX_GAIN;
	distortionProperties.lowpassCutoff = AL_DISTORTION_MAX_LOWPASS_CUTOFF;
	distortionProperties.eqCenter = AL_DISTORTION_MAX_EQCENTER;
	distortionProperties.eqBandwidth = AL_DISTORTION_MAX_EQBANDWIDTH;

	return distortionProperties;
}

const AudioEffectProperties::DistortionProperties &AudioEffectProperties::distortionProperties() const
{
	checkType(AudioEffect::Type::DISTORTION);
	const DistortionProperties &distortionProperties = reinterpret_cast<const DistortionProperties &>(propertiesData_);
	return distortionProperties;
}

AudioEffectProperties::DistortionProperties &AudioEffectProperties::distortionProperties()
{
	checkType(AudioEffect::Type::DISTORTION);
	DistortionProperties &distortionProperties = reinterpret_cast<DistortionProperties &>(propertiesData_);
	return distortionProperties;
}

void AudioEffectProperties::setDistortionProperties(const DistortionProperties &newDistortionProperties)
{
	if (checkType(AudioEffect::Type::DISTORTION))
	{
		DistortionProperties &distortionProperties = reinterpret_cast<DistortionProperties &>(propertiesData_);
		distortionProperties = newDistortionProperties;

		distortionProperties.edge = nctl::clamp(distortionProperties.edge, AL_DISTORTION_MIN_EDGE, AL_DISTORTION_MAX_EDGE);
		distortionProperties.gain = nctl::clamp(distortionProperties.gain, AL_DISTORTION_MIN_GAIN, AL_DISTORTION_MAX_GAIN);
		distortionProperties.lowpassCutoff = nctl::clamp(distortionProperties.lowpassCutoff, AL_DISTORTION_MIN_LOWPASS_CUTOFF, AL_DISTORTION_MAX_LOWPASS_CUTOFF);
		distortionProperties.eqCenter = nctl::clamp(distortionProperties.eqCenter, AL_DISTORTION_MIN_EQCENTER, AL_DISTORTION_MAX_EQCENTER);
		distortionProperties.eqBandwidth = nctl::clamp(distortionProperties.eqBandwidth, AL_DISTORTION_MIN_EQBANDWIDTH, AL_DISTORTION_MAX_EQBANDWIDTH);
	}
}

void AudioEffectProperties::resetDistortionProperties()
{
	if (checkType(AudioEffect::Type::DISTORTION))
	{
		DistortionProperties &distortionProperties = reinterpret_cast<DistortionProperties &>(propertiesData_);
		distortionProperties = DistortionProperties();
	}
}

//////////////////////////////
// Echo
//////////////////////////////

AudioEffectProperties::EchoProperties AudioEffectProperties::minEchoProperties()
{
	EchoProperties echoProperties;

	echoProperties.delay = AL_ECHO_MIN_DELAY;
	echoProperties.lrDelay = AL_ECHO_MIN_LRDELAY;
	echoProperties.damping = AL_ECHO_MIN_DAMPING;
	echoProperties.feedback = AL_ECHO_MIN_FEEDBACK;
	echoProperties.spread = AL_ECHO_MIN_SPREAD;

	return echoProperties;
}

AudioEffectProperties::EchoProperties AudioEffectProperties::maxEchoProperties()
{
	EchoProperties echoProperties;

	echoProperties.delay = AL_ECHO_MAX_DELAY;
	echoProperties.lrDelay = AL_ECHO_MAX_LRDELAY;
	echoProperties.damping = AL_ECHO_MAX_DAMPING;
	echoProperties.feedback = AL_ECHO_MAX_FEEDBACK;
	echoProperties.spread = AL_ECHO_MAX_SPREAD;

	return echoProperties;
}

const AudioEffectProperties::EchoProperties &AudioEffectProperties::echoProperties() const
{
	checkType(AudioEffect::Type::ECHO);
	const EchoProperties &echoProperties = reinterpret_cast<const EchoProperties &>(propertiesData_);
	return echoProperties;
}

AudioEffectProperties::EchoProperties &AudioEffectProperties::echoProperties()
{
	checkType(AudioEffect::Type::ECHO);
	EchoProperties &echoProperties = reinterpret_cast<EchoProperties &>(propertiesData_);
	return echoProperties;
}

void AudioEffectProperties::setEchoProperties(const EchoProperties &newEchoProperties)
{
	if (checkType(AudioEffect::Type::ECHO))
	{
		EchoProperties &echoProperties = reinterpret_cast<EchoProperties &>(propertiesData_);
		echoProperties = newEchoProperties;

		echoProperties.delay = nctl::clamp(echoProperties.delay, AL_ECHO_MIN_DELAY, AL_ECHO_MAX_DELAY);
		echoProperties.lrDelay = nctl::clamp(echoProperties.lrDelay, AL_ECHO_MIN_LRDELAY, AL_ECHO_MAX_LRDELAY);
		echoProperties.damping = nctl::clamp(echoProperties.damping, AL_ECHO_MIN_DAMPING, AL_ECHO_MAX_DAMPING);
		echoProperties.feedback = nctl::clamp(echoProperties.feedback, AL_ECHO_MIN_FEEDBACK, AL_ECHO_MAX_FEEDBACK);
		echoProperties.spread = nctl::clamp(echoProperties.spread, AL_ECHO_MIN_SPREAD, AL_ECHO_MAX_SPREAD);
	}
}

void AudioEffectProperties::resetEchoProperties()
{
	if (checkType(AudioEffect::Type::ECHO))
	{
		EchoProperties &echoProperties = reinterpret_cast<EchoProperties &>(propertiesData_);
		echoProperties = EchoProperties();
	}
}

//////////////////////////////
// Flanger
//////////////////////////////

AudioEffectProperties::FlangerProperties AudioEffectProperties::minFlangerProperties()
{
	FlangerProperties flangerProperties;

	flangerProperties.waveform = AL_FLANGER_MIN_WAVEFORM;
	flangerProperties.phase = AL_FLANGER_MIN_PHASE;
	flangerProperties.rate = AL_FLANGER_MIN_RATE;
	flangerProperties.depth = AL_FLANGER_MIN_DEPTH;
	flangerProperties.feedback = AL_FLANGER_MIN_FEEDBACK;
	flangerProperties.delay = AL_FLANGER_MIN_DELAY;

	return flangerProperties;
}

AudioEffectProperties::FlangerProperties AudioEffectProperties::maxFlangerProperties()
{
	FlangerProperties flangerProperties;

	flangerProperties.waveform = AL_FLANGER_MAX_WAVEFORM;
	flangerProperties.phase = AL_FLANGER_MAX_PHASE;
	flangerProperties.rate = AL_FLANGER_MAX_RATE;
	flangerProperties.depth = AL_FLANGER_MAX_DEPTH;
	flangerProperties.feedback = AL_FLANGER_MAX_FEEDBACK;
	flangerProperties.delay = AL_FLANGER_MAX_DELAY;

	return flangerProperties;
}

const AudioEffectProperties::FlangerProperties &AudioEffectProperties::flangerProperties() const
{
	checkType(AudioEffect::Type::FLANGER);
	const FlangerProperties &flangerProperties = reinterpret_cast<const FlangerProperties &>(propertiesData_);
	return flangerProperties;
}

AudioEffectProperties::FlangerProperties &AudioEffectProperties::flangerProperties()
{
	checkType(AudioEffect::Type::FLANGER);
	FlangerProperties &flangerProperties = reinterpret_cast<FlangerProperties &>(propertiesData_);
	return flangerProperties;
}

void AudioEffectProperties::setFlangerProperties(const FlangerProperties &newFlangerProperties)
{
	if (checkType(AudioEffect::Type::FLANGER))
	{
		FlangerProperties &flangerProperties = reinterpret_cast<FlangerProperties &>(propertiesData_);
		flangerProperties = newFlangerProperties;

		flangerProperties.waveform = nctl::clamp(flangerProperties.waveform, AL_FLANGER_MIN_WAVEFORM, AL_FLANGER_MAX_WAVEFORM);
		flangerProperties.phase = nctl::clamp(flangerProperties.phase, AL_FLANGER_MIN_PHASE, AL_FLANGER_MAX_PHASE);
		flangerProperties.rate = nctl::clamp(flangerProperties.rate, AL_FLANGER_MIN_RATE, AL_FLANGER_MAX_RATE);
		flangerProperties.depth = nctl::clamp(flangerProperties.depth, AL_FLANGER_MIN_DEPTH, AL_FLANGER_MAX_DEPTH);
		flangerProperties.feedback = nctl::clamp(flangerProperties.feedback, AL_FLANGER_MIN_FEEDBACK, AL_FLANGER_MAX_FEEDBACK);
		flangerProperties.delay = nctl::clamp(flangerProperties.delay, AL_FLANGER_MIN_DELAY, AL_FLANGER_MAX_DELAY);
	}
}

void AudioEffectProperties::resetFlangerProperties()
{
	if (checkType(AudioEffect::Type::FLANGER))
	{
		FlangerProperties &flangerProperties = reinterpret_cast<FlangerProperties &>(propertiesData_);
		flangerProperties = FlangerProperties();
	}
}

//////////////////////////////
// Frequency Shifter
//////////////////////////////

AudioEffectProperties::FrequencyShifterProperties AudioEffectProperties::minFrequencyShifterProperties()
{
	FrequencyShifterProperties frequencyShifterProperties;

	frequencyShifterProperties.frequency = AL_FREQUENCY_SHIFTER_MIN_FREQUENCY;
	frequencyShifterProperties.leftDirection = AL_FREQUENCY_SHIFTER_MIN_LEFT_DIRECTION;
	frequencyShifterProperties.rightDirection = AL_FREQUENCY_SHIFTER_MIN_RIGHT_DIRECTION;

	return frequencyShifterProperties;
}

AudioEffectProperties::FrequencyShifterProperties AudioEffectProperties::maxFrequencyShifterProperties()
{
	FrequencyShifterProperties frequencyShifterProperties;

	frequencyShifterProperties.frequency = AL_FREQUENCY_SHIFTER_MAX_FREQUENCY;
	frequencyShifterProperties.leftDirection = AL_FREQUENCY_SHIFTER_MAX_LEFT_DIRECTION;
	frequencyShifterProperties.rightDirection = AL_FREQUENCY_SHIFTER_MIN_RIGHT_DIRECTION;

	return frequencyShifterProperties;
}

const AudioEffectProperties::FrequencyShifterProperties &AudioEffectProperties::frequencyShifterProperties() const
{
	checkType(AudioEffect::Type::FREQUENCY_SHIFTER);
	const FrequencyShifterProperties &frequencyShifterProperties = reinterpret_cast<const FrequencyShifterProperties &>(propertiesData_);
	return frequencyShifterProperties;
}

AudioEffectProperties::FrequencyShifterProperties &AudioEffectProperties::frequencyShifterProperties()
{
	checkType(AudioEffect::Type::FREQUENCY_SHIFTER);
	FrequencyShifterProperties &frequencyShifterProperties = reinterpret_cast<FrequencyShifterProperties &>(propertiesData_);
	return frequencyShifterProperties;
}

void AudioEffectProperties::setFrequencyShifterProperties(const FrequencyShifterProperties &newFrequencyShifterProperties)
{
	if (checkType(AudioEffect::Type::FREQUENCY_SHIFTER))
	{
		FrequencyShifterProperties &frequencyShifterProperties = reinterpret_cast<FrequencyShifterProperties &>(propertiesData_);
		frequencyShifterProperties = newFrequencyShifterProperties;

		frequencyShifterProperties.frequency = nctl::clamp(frequencyShifterProperties.frequency, AL_FREQUENCY_SHIFTER_MIN_FREQUENCY, AL_FREQUENCY_SHIFTER_MAX_FREQUENCY);
		frequencyShifterProperties.leftDirection = nctl::clamp(frequencyShifterProperties.leftDirection, AL_FREQUENCY_SHIFTER_MIN_LEFT_DIRECTION, AL_FREQUENCY_SHIFTER_MAX_LEFT_DIRECTION);
		frequencyShifterProperties.rightDirection = nctl::clamp(frequencyShifterProperties.rightDirection, AL_FREQUENCY_SHIFTER_MIN_RIGHT_DIRECTION, AL_FREQUENCY_SHIFTER_MAX_RIGHT_DIRECTION);
	}
}

void AudioEffectProperties::resetFrequencyShifterProperties()
{
	if (checkType(AudioEffect::Type::FREQUENCY_SHIFTER))
	{
		FrequencyShifterProperties &frequencyShifterProperties = reinterpret_cast<FrequencyShifterProperties &>(propertiesData_);
		frequencyShifterProperties = FrequencyShifterProperties();
	}
}

//////////////////////////////
// Vocal Morpher
//////////////////////////////

AudioEffectProperties::VocalMorpherProperties AudioEffectProperties::minVocalMorpherProperties()
{
	VocalMorpherProperties vocalMorpherProperties;

	vocalMorpherProperties.phonemeA = AL_VOCAL_MORPHER_MIN_PHONEMEA;
	vocalMorpherProperties.phonemeACoarseTuning = AL_VOCAL_MORPHER_MIN_PHONEMEA_COARSE_TUNING;
	vocalMorpherProperties.phonemeB = AL_VOCAL_MORPHER_MIN_PHONEMEB;
	vocalMorpherProperties.phonemeBCoarseTuning = AL_VOCAL_MORPHER_MIN_PHONEMEB_COARSE_TUNING;
	vocalMorpherProperties.waveform = AL_VOCAL_MORPHER_MIN_WAVEFORM;
	vocalMorpherProperties.rate = AL_VOCAL_MORPHER_MIN_RATE;

	return vocalMorpherProperties;
}

AudioEffectProperties::VocalMorpherProperties AudioEffectProperties::maxVocalMorpherProperties()
{
	VocalMorpherProperties vocalMorpherProperties;

	vocalMorpherProperties.phonemeA = AL_VOCAL_MORPHER_MAX_PHONEMEA;
	vocalMorpherProperties.phonemeACoarseTuning = AL_VOCAL_MORPHER_MAX_PHONEMEA_COARSE_TUNING;
	vocalMorpherProperties.phonemeB = AL_VOCAL_MORPHER_MAX_PHONEMEB;
	vocalMorpherProperties.phonemeBCoarseTuning = AL_VOCAL_MORPHER_MAX_PHONEMEB_COARSE_TUNING;
	vocalMorpherProperties.waveform = AL_VOCAL_MORPHER_MAX_WAVEFORM;
	vocalMorpherProperties.rate = AL_VOCAL_MORPHER_MAX_RATE;

	return vocalMorpherProperties;
}

const AudioEffectProperties::VocalMorpherProperties &AudioEffectProperties::vocalMorpherProperties() const
{
	checkType(AudioEffect::Type::VOCAL_MORPHER);
	const VocalMorpherProperties &vocalMorpherProperties = reinterpret_cast<const VocalMorpherProperties &>(propertiesData_);
	return vocalMorpherProperties;
}

AudioEffectProperties::VocalMorpherProperties &AudioEffectProperties::vocalMorpherProperties()
{
	checkType(AudioEffect::Type::VOCAL_MORPHER);
	VocalMorpherProperties &vocalMorpherProperties = reinterpret_cast<VocalMorpherProperties &>(propertiesData_);
	return vocalMorpherProperties;
}

void AudioEffectProperties::setVocalMorpherProperties(const VocalMorpherProperties &newVocalMorpherProperties)
{
	if (checkType(AudioEffect::Type::VOCAL_MORPHER))
	{
		VocalMorpherProperties &vocalMorpherProperties = reinterpret_cast<VocalMorpherProperties &>(propertiesData_);
		vocalMorpherProperties = newVocalMorpherProperties;

		vocalMorpherProperties.phonemeA = nctl::clamp(vocalMorpherProperties.phonemeA, AL_VOCAL_MORPHER_MIN_PHONEMEA, AL_VOCAL_MORPHER_MAX_PHONEMEA);
		vocalMorpherProperties.phonemeACoarseTuning = nctl::clamp(vocalMorpherProperties.phonemeACoarseTuning, AL_VOCAL_MORPHER_MIN_PHONEMEA_COARSE_TUNING, AL_VOCAL_MORPHER_MAX_PHONEMEA_COARSE_TUNING);
		vocalMorpherProperties.phonemeB = nctl::clamp(vocalMorpherProperties.phonemeB, AL_VOCAL_MORPHER_MIN_PHONEMEB, AL_VOCAL_MORPHER_MAX_PHONEMEB);
		vocalMorpherProperties.phonemeBCoarseTuning = nctl::clamp(vocalMorpherProperties.phonemeBCoarseTuning, AL_VOCAL_MORPHER_MIN_PHONEMEB_COARSE_TUNING, AL_VOCAL_MORPHER_MAX_PHONEMEB_COARSE_TUNING);
		vocalMorpherProperties.waveform = nctl::clamp(vocalMorpherProperties.waveform, AL_VOCAL_MORPHER_MIN_WAVEFORM, AL_VOCAL_MORPHER_MAX_WAVEFORM);
		vocalMorpherProperties.rate = nctl::clamp(vocalMorpherProperties.rate, AL_VOCAL_MORPHER_MIN_RATE, AL_VOCAL_MORPHER_MAX_RATE);
	}
}

void AudioEffectProperties::resetVocalMorpherProperties()
{
	if (checkType(AudioEffect::Type::VOCAL_MORPHER))
	{
		VocalMorpherProperties &vocalMorpherProperties = reinterpret_cast<VocalMorpherProperties &>(propertiesData_);
		vocalMorpherProperties = VocalMorpherProperties();
	}
}

//////////////////////////////
// Pitch Shifter
//////////////////////////////

AudioEffectProperties::PitchShifterProperties AudioEffectProperties::minPitchShifterProperties()
{
	PitchShifterProperties pitchShifterProperties;

	pitchShifterProperties.coarseTune = AL_PITCH_SHIFTER_MIN_COARSE_TUNE;
	pitchShifterProperties.fineTune = AL_PITCH_SHIFTER_MIN_FINE_TUNE;

	return pitchShifterProperties;
}

AudioEffectProperties::PitchShifterProperties AudioEffectProperties::maxPitchShifterProperties()
{
	PitchShifterProperties pitchShifterProperties;

	pitchShifterProperties.coarseTune = AL_PITCH_SHIFTER_MAX_COARSE_TUNE;
	pitchShifterProperties.fineTune = AL_PITCH_SHIFTER_MAX_FINE_TUNE;

	return pitchShifterProperties;
}

const AudioEffectProperties::PitchShifterProperties &AudioEffectProperties::pitchShifterProperties() const
{
	checkType(AudioEffect::Type::PITCH_SHIFTER);
	const PitchShifterProperties &pitchShifterProperties = reinterpret_cast<const PitchShifterProperties &>(propertiesData_);
	return pitchShifterProperties;
}

AudioEffectProperties::PitchShifterProperties &AudioEffectProperties::pitchShifterProperties()
{
	checkType(AudioEffect::Type::PITCH_SHIFTER);
	PitchShifterProperties &pitchShifterProperties = reinterpret_cast<PitchShifterProperties &>(propertiesData_);
	return pitchShifterProperties;
}

void AudioEffectProperties::setPitchShifterProperties(const PitchShifterProperties &newPitchShifterProperties)
{
	if (checkType(AudioEffect::Type::PITCH_SHIFTER))
	{
		PitchShifterProperties &pitchShifterProperties = reinterpret_cast<PitchShifterProperties &>(propertiesData_);
		pitchShifterProperties = newPitchShifterProperties;

		pitchShifterProperties.coarseTune = nctl::clamp(pitchShifterProperties.coarseTune, AL_PITCH_SHIFTER_MIN_COARSE_TUNE, AL_PITCH_SHIFTER_MAX_COARSE_TUNE);
		pitchShifterProperties.fineTune = nctl::clamp(pitchShifterProperties.fineTune, AL_PITCH_SHIFTER_MIN_FINE_TUNE, AL_PITCH_SHIFTER_MAX_FINE_TUNE);
	}
}

void AudioEffectProperties::resetPitchShifterProperties()
{
	if (checkType(AudioEffect::Type::PITCH_SHIFTER))
	{
		PitchShifterProperties &pitchShifterProperties = reinterpret_cast<PitchShifterProperties &>(propertiesData_);
		pitchShifterProperties = PitchShifterProperties();
	}
}

//////////////////////////////
// Ring Modulator
//////////////////////////////

AudioEffectProperties::RingModulatorProperties AudioEffectProperties::minRingModulatorProperties()
{
	RingModulatorProperties ringModulatorProperties;

	ringModulatorProperties.frequency = AL_RING_MODULATOR_MIN_FREQUENCY;
	ringModulatorProperties.highpassCutoff = AL_RING_MODULATOR_MIN_HIGHPASS_CUTOFF;
	ringModulatorProperties.waveform = AL_RING_MODULATOR_MIN_WAVEFORM;

	return ringModulatorProperties;
}

AudioEffectProperties::RingModulatorProperties AudioEffectProperties::maxRingModulatorProperties()
{
	RingModulatorProperties ringModulatorProperties;

	ringModulatorProperties.frequency = AL_RING_MODULATOR_MAX_FREQUENCY;
	ringModulatorProperties.highpassCutoff = AL_RING_MODULATOR_MAX_HIGHPASS_CUTOFF;
	ringModulatorProperties.waveform = AL_RING_MODULATOR_MAX_WAVEFORM;

	return ringModulatorProperties;
}

const AudioEffectProperties::RingModulatorProperties &AudioEffectProperties::ringModulatorProperties() const
{
	checkType(AudioEffect::Type::RING_MODULATOR);
	const RingModulatorProperties &ringModulatorProperties = reinterpret_cast<const RingModulatorProperties &>(propertiesData_);
	return ringModulatorProperties;
}

AudioEffectProperties::RingModulatorProperties &AudioEffectProperties::ringModulatorProperties()
{
	checkType(AudioEffect::Type::RING_MODULATOR);
	RingModulatorProperties &ringModulatorProperties = reinterpret_cast<RingModulatorProperties &>(propertiesData_);
	return ringModulatorProperties;
}

void AudioEffectProperties::setRingModulatorProperties(const RingModulatorProperties &newRingModulatorProperties)
{
	if (checkType(AudioEffect::Type::RING_MODULATOR))
	{
		RingModulatorProperties &ringModulatorProperties = reinterpret_cast<RingModulatorProperties &>(propertiesData_);
		ringModulatorProperties = newRingModulatorProperties;

		ringModulatorProperties.frequency = nctl::clamp(ringModulatorProperties.frequency, AL_RING_MODULATOR_MIN_FREQUENCY, AL_RING_MODULATOR_MAX_FREQUENCY);
		ringModulatorProperties.highpassCutoff = nctl::clamp(ringModulatorProperties.highpassCutoff, AL_RING_MODULATOR_MIN_HIGHPASS_CUTOFF, AL_RING_MODULATOR_MAX_HIGHPASS_CUTOFF);
		ringModulatorProperties.waveform = nctl::clamp(ringModulatorProperties.waveform, AL_RING_MODULATOR_MIN_WAVEFORM, AL_RING_MODULATOR_MAX_WAVEFORM);
	}
}

void AudioEffectProperties::resetRingModulatorProperties()
{
	if (checkType(AudioEffect::Type::RING_MODULATOR))
	{
		RingModulatorProperties &ringModulatorProperties = reinterpret_cast<RingModulatorProperties &>(propertiesData_);
		ringModulatorProperties = RingModulatorProperties();
	}
}

//////////////////////////////
// Auto-wah
//////////////////////////////

AudioEffectProperties::AutoWahProperties AudioEffectProperties::minAutoWahProperties()
{
	AutoWahProperties autoWahProperties;

	autoWahProperties.attackTime = AL_AUTOWAH_MIN_ATTACK_TIME;
	autoWahProperties.releaseTime = AL_AUTOWAH_MIN_RELEASE_TIME;
	autoWahProperties.resonance = AL_AUTOWAH_MIN_RESONANCE;
	autoWahProperties.peakGain = AL_AUTOWAH_MIN_PEAK_GAIN;

	return autoWahProperties;
}

AudioEffectProperties::AutoWahProperties AudioEffectProperties::maxAutoWahProperties()
{
	AutoWahProperties autoWahProperties;

	autoWahProperties.attackTime = AL_AUTOWAH_MAX_ATTACK_TIME;
	autoWahProperties.releaseTime = AL_AUTOWAH_MAX_RELEASE_TIME;
	autoWahProperties.resonance = AL_AUTOWAH_MAX_RESONANCE;
	autoWahProperties.peakGain = AL_AUTOWAH_MAX_PEAK_GAIN;

	return autoWahProperties;
}

const AudioEffectProperties::AutoWahProperties &AudioEffectProperties::autoWahProperties() const
{
	checkType(AudioEffect::Type::AUTOWAH);
	const AutoWahProperties &autoWahProperties = reinterpret_cast<const AutoWahProperties &>(propertiesData_);
	return autoWahProperties;
}

AudioEffectProperties::AutoWahProperties &AudioEffectProperties::autoWahProperties()
{
	checkType(AudioEffect::Type::AUTOWAH);
	AutoWahProperties &autoWahProperties = reinterpret_cast<AutoWahProperties &>(propertiesData_);
	return autoWahProperties;
}

void AudioEffectProperties::setAutoWahProperties(const AutoWahProperties &newAutoWahProperties)
{
	if (checkType(AudioEffect::Type::AUTOWAH))
	{
		AutoWahProperties &autoWahProperties = reinterpret_cast<AutoWahProperties &>(propertiesData_);
		autoWahProperties = newAutoWahProperties;

		autoWahProperties.attackTime = nctl::clamp(autoWahProperties.attackTime, AL_AUTOWAH_MIN_ATTACK_TIME, AL_AUTOWAH_MAX_ATTACK_TIME);
		autoWahProperties.releaseTime = nctl::clamp(autoWahProperties.releaseTime, AL_AUTOWAH_MIN_RELEASE_TIME, AL_AUTOWAH_MAX_RELEASE_TIME);
		autoWahProperties.resonance = nctl::clamp(autoWahProperties.resonance, AL_AUTOWAH_MIN_RESONANCE, AL_AUTOWAH_MAX_RESONANCE);
		autoWahProperties.peakGain = nctl::clamp(autoWahProperties.peakGain, AL_AUTOWAH_MIN_PEAK_GAIN, AL_AUTOWAH_MAX_PEAK_GAIN);
	}
}

void AudioEffectProperties::resetAutoWahProperties()
{
	if (checkType(AudioEffect::Type::AUTOWAH))
	{
		AutoWahProperties &echoProperties = reinterpret_cast<AutoWahProperties &>(propertiesData_);
		echoProperties = AutoWahProperties();
	}
}

//////////////////////////////
// Compressor
//////////////////////////////

AudioEffectProperties::CompressorProperties AudioEffectProperties::minCompressorProperties()
{
	CompressorProperties compressorProperties;

	compressorProperties.onOff = AL_COMPRESSOR_MIN_ONOFF;

	return compressorProperties;
}

AudioEffectProperties::CompressorProperties AudioEffectProperties::maxCompressorProperties()
{
	CompressorProperties compressorProperties;

	compressorProperties.onOff = AL_COMPRESSOR_MAX_ONOFF;

	return compressorProperties;
}

const AudioEffectProperties::CompressorProperties &AudioEffectProperties::compressorProperties() const
{
	checkType(AudioEffect::Type::COMPRESSOR);
	const CompressorProperties &compressorProperties = reinterpret_cast<const CompressorProperties &>(propertiesData_);
	return compressorProperties;
}

AudioEffectProperties::CompressorProperties &AudioEffectProperties::compressorProperties()
{
	checkType(AudioEffect::Type::COMPRESSOR);
	CompressorProperties &compressorProperties = reinterpret_cast<CompressorProperties &>(propertiesData_);
	return compressorProperties;
}

void AudioEffectProperties::setCompressorProperties(const CompressorProperties &newCompressorProperties)
{
	if (checkType(AudioEffect::Type::COMPRESSOR))
	{
		CompressorProperties &compressorProperties = reinterpret_cast<CompressorProperties &>(propertiesData_);
		compressorProperties = newCompressorProperties;

		compressorProperties.onOff = nctl::clamp(compressorProperties.onOff, AL_COMPRESSOR_MIN_ONOFF, AL_COMPRESSOR_MAX_ONOFF);
	}
}

void AudioEffectProperties::resetCompressorProperties()
{
	if (checkType(AudioEffect::Type::COMPRESSOR))
	{
		CompressorProperties &compressorProperties = reinterpret_cast<CompressorProperties &>(propertiesData_);
		compressorProperties = CompressorProperties();
	}
}

//////////////////////////////
// Equalizer
//////////////////////////////

AudioEffectProperties::EqualizerProperties AudioEffectProperties::minEqualizerProperties()
{
	EqualizerProperties equalizerProperties;

	equalizerProperties.lowGain = AL_EQUALIZER_MIN_LOW_GAIN;
	equalizerProperties.lowCutoff = AL_EQUALIZER_MIN_LOW_CUTOFF;
	equalizerProperties.mid1Gain = AL_EQUALIZER_MIN_MID1_GAIN;
	equalizerProperties.mid1Center = AL_EQUALIZER_MIN_MID1_CENTER;
	equalizerProperties.mid1Width = AL_EQUALIZER_MIN_MID1_WIDTH;
	equalizerProperties.mid2Gain = AL_EQUALIZER_MIN_MID2_GAIN;
	equalizerProperties.mid2Center = AL_EQUALIZER_MIN_MID2_CENTER;
	equalizerProperties.mid2Width = AL_EQUALIZER_MIN_MID2_WIDTH;
	equalizerProperties.highGain = AL_EQUALIZER_MIN_HIGH_GAIN;
	equalizerProperties.highCutoff = AL_EQUALIZER_MIN_HIGH_CUTOFF;

	return equalizerProperties;
}

AudioEffectProperties::EqualizerProperties AudioEffectProperties::maxEqualizerProperties()
{
	EqualizerProperties equalizerProperties;

	equalizerProperties.lowGain = AL_EQUALIZER_MAX_LOW_GAIN;
	equalizerProperties.lowCutoff = AL_EQUALIZER_MAX_LOW_CUTOFF;
	equalizerProperties.mid1Gain = AL_EQUALIZER_MAX_MID1_GAIN;
	equalizerProperties.mid1Center = AL_EQUALIZER_MAX_MID1_CENTER;
	equalizerProperties.mid1Width = AL_EQUALIZER_MAX_MID1_WIDTH;
	equalizerProperties.mid2Gain = AL_EQUALIZER_MAX_MID2_GAIN;
	equalizerProperties.mid2Center = AL_EQUALIZER_MAX_MID2_CENTER;
	equalizerProperties.mid2Width = AL_EQUALIZER_MAX_MID2_WIDTH;
	equalizerProperties.highGain = AL_EQUALIZER_MAX_HIGH_GAIN;
	equalizerProperties.highCutoff = AL_EQUALIZER_MAX_HIGH_CUTOFF;

	return equalizerProperties;
}

const AudioEffectProperties::EqualizerProperties &AudioEffectProperties::equalizerProperties() const
{
	checkType(AudioEffect::Type::EQUALIZER);
	const EqualizerProperties &equalizerProperties = reinterpret_cast<const EqualizerProperties &>(propertiesData_);
	return equalizerProperties;
}

AudioEffectProperties::EqualizerProperties &AudioEffectProperties::equalizerProperties()
{
	checkType(AudioEffect::Type::EQUALIZER);
	EqualizerProperties &equalizerProperties = reinterpret_cast<EqualizerProperties &>(propertiesData_);
	return equalizerProperties;
}

void AudioEffectProperties::setEqualizerProperties(const EqualizerProperties &newEqualizerProperties)
{
	if (checkType(AudioEffect::Type::EQUALIZER))
	{
		EqualizerProperties &equalizerProperties = reinterpret_cast<EqualizerProperties &>(propertiesData_);
		equalizerProperties = newEqualizerProperties;

		equalizerProperties.lowGain = nctl::clamp(equalizerProperties.lowGain, AL_EQUALIZER_MIN_LOW_GAIN, AL_EQUALIZER_MAX_LOW_GAIN);
		equalizerProperties.lowCutoff = nctl::clamp(equalizerProperties.lowCutoff, AL_EQUALIZER_MIN_LOW_CUTOFF, AL_EQUALIZER_MAX_LOW_CUTOFF);
		equalizerProperties.mid1Gain = nctl::clamp(equalizerProperties.mid1Gain, AL_EQUALIZER_MIN_MID1_GAIN, AL_EQUALIZER_MAX_MID1_GAIN);
		equalizerProperties.mid1Center = nctl::clamp(equalizerProperties.mid1Center, AL_EQUALIZER_MIN_MID1_CENTER, AL_EQUALIZER_MAX_MID1_CENTER);
		equalizerProperties.mid1Width = nctl::clamp(equalizerProperties.mid1Width, AL_EQUALIZER_MIN_MID1_WIDTH, AL_EQUALIZER_MAX_MID1_WIDTH);
		equalizerProperties.mid2Gain = nctl::clamp(equalizerProperties.mid2Gain, AL_EQUALIZER_MIN_MID2_GAIN, AL_EQUALIZER_MAX_MID2_GAIN);
		equalizerProperties.mid2Center = nctl::clamp(equalizerProperties.mid2Center, AL_EQUALIZER_MIN_MID2_CENTER, AL_EQUALIZER_MAX_MID2_CENTER);
		equalizerProperties.mid2Width = nctl::clamp(equalizerProperties.mid2Width, AL_EQUALIZER_MIN_MID2_WIDTH, AL_EQUALIZER_MAX_MID2_WIDTH);
		equalizerProperties.highGain = nctl::clamp(equalizerProperties.highGain, AL_EQUALIZER_MIN_HIGH_GAIN, AL_EQUALIZER_MAX_HIGH_GAIN);
		equalizerProperties.highCutoff = nctl::clamp(equalizerProperties.highCutoff, AL_EQUALIZER_MIN_HIGH_CUTOFF, AL_EQUALIZER_MAX_HIGH_CUTOFF);
	}
}

void AudioEffectProperties::resetEqualizerProperties()
{
	if (checkType(AudioEffect::Type::EQUALIZER))
	{
		EqualizerProperties &equalizerProperties = reinterpret_cast<EqualizerProperties &>(propertiesData_);
		equalizerProperties = EqualizerProperties();
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool AudioEffectProperties::checkType(AudioEffect::Type type) const
{
	ASSERT(type_ == type);
	if (type_ != type)
		LOGW_X("Accessing %s properties when type is %s", typeTostring(type), typeTostring(type_));
	return (type_ == type);
}

}
