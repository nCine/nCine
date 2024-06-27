#include "LuaAudioEffectProperties.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaUtils.h"
#include "LuaVector3Utils.h"
#include "AudioEffectProperties.h"

namespace ncine {

namespace LuaNames {
namespace AudioEffectProperties {
	static const char *AudioEffectProperties = "audio_effect_properties";

	static const char *type = "get_type";
	static const char *setType = "set_type";

	static const char *minReverbProperties = "get_min_reverb_properties";
	static const char *maxReverbProperties = "get_max_reverb_properties";
	static const char *reverbProperties = "get_reverb_properties";
	static const char *setReverbProperties = "set_reverb_properties";
	static const char *resetReverbProperties = "reset_reverb_properties";

	static const char *minEaxReverbProperties = "get_min_eax_reverb_properties";
	static const char *maxEaxReverbProperties = "get_max_eax_reverb_properties";
	static const char *eaxReverbProperties = "get_eax_reverb_properties";
	static const char *setEaxReverbProperties = "set_eax_reverb_properties";
	static const char *resetEaxReverbProperties = "reset_eax_reverb_properties";
	static const char *loadEfxReverbPreset = "load_efx_reverb_preset";

	static const char *minChorusProperties = "get_min_chorus_properties";
	static const char *maxChorusProperties = "get_max_chorus_properties";
	static const char *chorusProperties = "get_chorus_properties";
	static const char *setChorusProperties = "set_chorus_properties";
	static const char *resetChorusProperties = "reset_chorus_properties";

	static const char *minDistortionProperties = "get_min_distortion_properties";
	static const char *maxDistortionProperties = "get_max_distortion_properties";
	static const char *distortionProperties = "get_distortion_properties";
	static const char *setDistortionProperties = "set_distortion_properties";
	static const char *resetDistortionProperties = "reset_distortion_properties";

	static const char *minEchoProperties = "get_min_echo_properties";
	static const char *maxEchoProperties = "get_max_echo_properties";
	static const char *echoProperties = "get_echo_properties";
	static const char *setEchoProperties = "set_echo_properties";
	static const char *resetEchoProperties = "reset_echo_properties";

	static const char *minFlangerProperties = "get_min_flanger_properties";
	static const char *maxFlangerProperties = "get_max_flanger_properties";
	static const char *flangerProperties = "get_flanger_properties";
	static const char *setFlangerProperties = "set_flanger_properties";
	static const char *resetFlangerProperties = "reset_flanger_properties";

	static const char *minFrequencyShifterProperties = "get_min_frequency_shifter_properties";
	static const char *maxFrequencyShifterProperties = "get_max_frequency_shifter_properties";
	static const char *frequencyShifterProperties = "get_frequency_shifter_properties";
	static const char *setFrequencyShifterProperties = "set_frequency_shifter_properties";
	static const char *resetFrequencyShifterProperties = "reset_frequency_shifter_properties";

	static const char *minVocalMorpherProperties = "get_min_vocal_morpher_properties";
	static const char *maxVocalMorpherProperties = "get_max_vocal_morpher_properties";
	static const char *vocalMorpherProperties = "get_vocal_morpher_properties";
	static const char *setVocalMorpherProperties = "set_vocal_morpher_properties";
	static const char *resetVocalMorpherProperties = "reset_vocal_morpher_properties";

	static const char *minPitchShifterProperties = "get_min_pitch_shifter_properties";
	static const char *maxPitchShifterProperties = "get_max_pitch_shifter_properties";
	static const char *pitchShifterProperties = "get_pitch_shifter_properties";
	static const char *setPitchShifterProperties = "set_pitch_shifter_properties";
	static const char *resetPitchShifterProperties = "reset_pitch_shifter_properties";

	static const char *minRingModulatorProperties = "get_min_ring_modulator_properties";
	static const char *maxRingModulatorProperties = "get_max_ring_modulator_properties";
	static const char *ringModulatorProperties = "get_ring_modulator_properties";
	static const char *setRingModulatorProperties = "set_ring_modulator_properties";
	static const char *resetRingModulatorProperties = "reset_ring_modulator_properties";

	static const char *minAutoWahProperties = "get_min_auto_wah_properties";
	static const char *maxAutoWahProperties = "get_max_auto_wah_properties";
	static const char *autoWahProperties = "get_auto_wah_properties";
	static const char *setAutoWahProperties = "set_auto_wah_properties";
	static const char *resetAutoWahProperties = "reset_auto_wah_properties";

	static const char *minCompressorProperties = "get_min_compressor_properties";
	static const char *maxCompressorProperties = "get_max_compressor_properties";
	static const char *compressorProperties = "get_compressor_properties";
	static const char *setCompressorProperties = "set_compressor_properties";
	static const char *resetCompressorProperties = "reset_compressor_properties";

	static const char *minEqualizerProperties = "get_min_equalizer_properties";
	static const char *maxEqualizerProperties = "get_max_equalizer_properties";
	static const char *equalizerProperties = "get_equalizer_properties";
	static const char *setEqualizerProperties = "set_equalizer_properties";
	static const char *resetEqualizerProperties = "reset_equalizer_properties";

	namespace ReverbProperties {
		static const char *density = "density";
		static const char *diffusion = "diffusion";
		static const char *gain = "gain";
		static const char *gainHF = "gain_hf";
		static const char *decayTime = "decay_time";
		static const char *decayHFRatio = "decay_hf_ratio";
		static const char *reflectionsGain = "reflections_gain";
		static const char *reflectionsDelay = "reflections_delay";
		static const char *lateReverbGain = "late_reverb_gain";
		static const char *lateReverbDelay = "late_reverb_delay";
		static const char *airAbsorptionGainHF = "air_absorption_gain_hf";
		static const char *roomRolloffFactor = "room_rolloff_factor";
		static const char *decayHFLimit = "decay_hf_limit";
	}

	namespace EfxReverbPresets {
		// Default Presets
		static const char *GENERIC = "GENERIC";
		static const char *PADDED_CELL = "PADDED_CELL";
		static const char *ROOM = "ROOM";
		static const char *BATHROOM = "BATHROOM";
		static const char *LIVING_ROOM = "LIVING_ROOM";
		static const char *STONE_ROOM = "STONE_ROOM";
		static const char *AUDITORIUM = "AUDITORIUM";
		static const char *CONCERT_HALL = "CONCERT_HALL";
		static const char *CAVE = "CAVE";
		static const char *ARENA = "ARENA";
		static const char *HANGAR = "HANGAR";
		static const char *CARPETED_HALLWAY = "CARPETED_HALLWAY";
		static const char *HALLWAY = "HALLWAY";
		static const char *STONE_CORRIDOR = "STONE_CORRIDOR";
		static const char *ALLEY = "ALLEY";
		static const char *FOREST = "FOREST";
		static const char *CITY = "CITY";
		static const char *MOUNTAINS = "MOUNTAINS";
		static const char *QUARRY = "QUARRY";
		static const char *PLAIN = "PLAIN";
		static const char *PARKING_LOT = "PARKING_LOT";
		static const char *SEWER_PIPE = "SEWER_PIPE";
		static const char *UNDERWATER = "UNDERWATER";
		static const char *DRUGGED = "DRUGGED";
		static const char *DIZZY = "DIZZY";
		static const char *PSYCHOTIC = "PSYCHOTIC";
		// Castle Presets
		static const char *CASTLE_SMALL_ROOM = "CASTLE_SMALL_ROOM";
		static const char *CASTLE_SHORT_PASSAGE = "CASTLE_SHORT_PASSAGE";
		static const char *CASTLE_MEDIUM_ROOM = "CASTLE_MEDIUM_ROOM";
		static const char *CASTLE_LARGE_ROOM = "CASTLE_LARGE_ROOM";
		static const char *CASTLE_LONG_PASSAGE = "CASTLE_LONG_PASSAGE";
		static const char *CASTLE_HALL = "CASTLE_HALL";
		static const char *CASTLE_CUPBOARD = "CASTLE_CUPBOARD";
		static const char *CASTLE_COURTYARD = "CASTLE_COURTYARD";
		static const char *CASTLE_ALCOVE = "CASTLE_ALCOVE";
		// Factory Presets
		static const char *FACTORY_SMALL_ROOM = "FACTORY_SMALL_ROOM";
		static const char *FACTORY_SHORT_PASSAGE = "FACTORY_SHORT_PASSAGE";
		static const char *FACTORY_MEDIUM_ROOM = "FACTORY_MEDIUM_ROOM";
		static const char *FACTORY_LARGE_ROOM = "FACTORY_LARGE_ROOM";
		static const char *FACTORY_LONG_PASSAGE = "FACTORY_LONG_PASSAGE";
		static const char *FACTORY_HALL = "FACTORY_HALL";
		static const char *FACTORY_CUPBOARD = "FACTORY_CUPBOARD";
		static const char *FACTORY_COURTYARD = "FACTORY_COURTYARD";
		static const char *FACTORY_ALCOVE = "FACTORY_ALCOVE";
		// Ice Palace Presets
		static const char *ICEPALACE_SMALL_ROOM = "ICEPALACE_SMALL_ROOM";
		static const char *ICEPALACE_SHORT_PASSAGE = "ICEPALACE_SHORT_PASSAGE";
		static const char *ICEPALACE_MEDIUM_ROOM = "ICEPALACE_MEDIUM_ROOM";
		static const char *ICEPALACE_LARGE_ROOM = "ICEPALACE_LARGE_ROOM";
		static const char *ICEPALACE_LONG_PASSAGE = "ICEPALACE_LONG_PASSAGE";
		static const char *ICEPALACE_HALL = "ICEPALACE_HALL";
		static const char *ICEPALACE_CUPBOARD = "ICEPALACE_CUPBOARD";
		static const char *ICEPALACE_COURTYARD = "ICEPALACE_COURTYARD";
		static const char *ICEPALACE_ALCOVE = "ICEPALACE_ALCOVE";
		// Space Station Presets
		static const char *SPACE_STATION_SMALL_ROOM = "SPACE_STATION_SMALL_ROOM";
		static const char *SPACE_STATION_SHORT_PASSAGE = "SPACE_STATION_SHORT_PASSAGE";
		static const char *SPACE_STATION_MEDIUM_ROOM = "SPACE_STATION_MEDIUM_ROOM";
		static const char *SPACE_STATION_LARGE_ROOM = "SPACE_STATION_LARGE_ROOM";
		static const char *SPACE_STATION_LONG_PASSAGE = "SPACE_STATION_LONG_PASSAGE";
		static const char *SPACE_STATION_HALL = "SPACE_STATION_HALL";
		static const char *SPACE_STATION_CUPBOARD = "SPACE_STATION_CUPBOARD";
		static const char *SPACE_STATION_ALCOVE = "SPACE_STATION_ALCOVE";
		// Wooden Galleon Presets
		static const char *WOODEN_GALLEON_SMALL_ROOM = "WOODEN_GALLEON_SMALL_ROOM";
		static const char *WOODEN_GALLEON_SHORT_PASSAGE = "WOODEN_GALLEON_SHORT_PASSAGE";
		static const char *WOODEN_GALLEON_MEDIUM_ROOM = "WOODEN_GALLEON_MEDIUM_ROOM";
		static const char *WOODEN_GALLEON_LARGE_ROOM = "WOODEN_GALLEON_LARGE_ROOM";
		static const char *WOODEN_GALLEON_LONG_PASSAGE = "WOODEN_GALLEON_LONG_PASSAGE";
		static const char *WOODEN_GALLEON_HALL = "WOODEN_GALLEON_HALL";
		static const char *WOODEN_GALLEON_CUPBOARD = "WOODEN_GALLEON_CUPBOARD";
		static const char *WOODEN_GALLEON_COURTYARD = "WOODEN_GALLEON_COURTYARD";
		static const char *WOODEN_GALLEON_ALCOVE = "WOODEN_GALLEON_ALCOVE";
		// Sports Presets
		static const char *SPORT_EMPTY_STADIUM = "SPORT_EMPTY_STADIUM";
		static const char *SPORT_SQUASH_COURT = "SPORT_SQUASH_COURT";
		static const char *SPORT_SMALL_SWIMMING_POOL = "SPORT_SMALL_SWIMMING_POOL";
		static const char *SPORT_LARGE_SWIMMING_POOL = "SPORT_LARGE_SWIMMING_POOL";
		static const char *SPORT_GYMNASIUM = "SPORT_GYMNASIUM";
		static const char *SPORT_FULL_STADIUM = "SPORT_FULL_STADIUM";
		static const char *SPORT_STADIUM_TANNOY = "SPORT_STADIUM_TANNOY";
		// Prefab Presets
		static const char *PREFAB_WORKSHOP = "PREFAB_WORKSHOP";
		static const char *PREFAB_SCHOOL_ROOM = "PREFAB_SCHOOL_ROOM";
		static const char *PREFAB_PRACTISE_ROOM = "PREFAB_PRACTISE_ROOM";
		static const char *PREFAB_OUTHOUSE = "PREFAB_OUTHOUSE";
		static const char *PREFAB_CARAVAN = "PREFAB_CARAVAN";
		// Dome and Pipe Presets
		static const char *DOME_TOMB = "DOME_TOMB";
		static const char *PIPE_SMALL = "PIPE_SMALL";
		static const char *DOME_SAINT_PAULS = "DOME_SAINT_PAULS";
		static const char *PIPE_LONGTHIN = "PIPE_LONGTHIN";
		static const char *PIPE_LARGE = "PIPE_LARGE";
		static const char *PIPE_RESONANT = "PIPE_RESONANT";
		// Outdoors Presets
		static const char *OUTDOORS_BACKYARD = "OUTDOORS_BACKYARD";
		static const char *OUTDOORS_ROLLING_PLAINS = "OUTDOORS_ROLLING_PLAINS";
		static const char *OUTDOORS_DEEP_CANYON = "OUTDOORS_DEEP_CANYON";
		static const char *OUTDOORS_CREEK = "OUTDOORS_CREEK";
		static const char *OUTDOORS_VALLEY = "OUTDOORS_VALLEY";
		// Mood Preset
		static const char *MOOD_HEAVEN = "MOOD_HEAVEN";
		static const char *MOOD_HELL = "MOOD_HELL";
		static const char *MOOD_MEMORY = "MOOD_MEMORY";
		// Driving Presets
		static const char *DRIVING_COMMENTATOR = "DRIVING_COMMENTATOR";
		static const char *DRIVING_PIT_GARAGE = "DRIVING_PIT_GARAGE";
		static const char *DRIVING_INCAR_RACER = "DRIVING_INCAR_RACER";
		static const char *DRIVING_INCAR_SPORTS = "DRIVING_INCAR_SPORTS";
		static const char *DRIVING_INCAR_LUXURY = "DRIVING_INCAR_LUXURY";
		static const char *DRIVING_FULL_GRANDSTAND = "DRIVING_FULL_GRANDSTAND";
		static const char *DRIVING_EMPTY_GRANDSTAND = "DRIVING_EMPTY_GRANDSTAND";
		static const char *DRIVING_TUNNEL = "DRIVING_TUNNEL";
		// City Presets
		static const char *CITY_STREETS = "CITY_STREETS";
		static const char *CITY_SUBWAY = "CITY_SUBWAY";
		static const char *CITY_MUSEUM = "CITY_MUSEUM";
		static const char *CITY_LIBRARY = "CITY_LIBRARY";
		static const char *CITY_UNDERPASS = "CITY_UNDERPASS";
		static const char *CITY_ABANDONED = "CITY_ABANDONED";
		// Misc. Presets
		static const char *DUSTY_ROOM = "DUSTY_ROOM";
		static const char *CHAPEL = "CHAPEL";
		static const char *SMALL_WATER_ROOM = "SMALL_WATER_ROOM";

		static const char *EfxReverbPresets = "efx_reverb_presets";
	}

	namespace EaxReverbProperties {
		static const char *density = "density";
		static const char *diffusion = "diffusion";
		static const char *gain = "gain";
		static const char *gainHF = "gain_hf";
		static const char *gainLF = "gain_lf";
		static const char *decayTime = "decay_time";
		static const char *decayHFRatio = "decay_hf_ratio";
		static const char *decayLFRatio = "decay_lf_ratio";
		static const char *reflectionsGain = "reflections_gain";
		static const char *reflectionsDelay = "reflections_delay";
		static const char *reflectionsPan = "reflections_pan";
		static const char *lateReverbGain = "late_reverb_gain";
		static const char *lateReverbDelay = "late_reverb_delay";
		static const char *lateReverbPan = "late_reverb_pan";
		static const char *echoTime = "echo_time";
		static const char *echoDepth = "echo_depth";
		static const char *modulationTime = "modulation_time";
		static const char *modulationDepth = "modulation_depth";
		static const char *airAbsorptionGainHF = "air_absorption_gain_hf";
		static const char *hfReference = "hf_reference";
		static const char *lfReference = "lf_reference";
		static const char *roomRolloffFactor = "room_rolloff_factor";
		static const char *decayHFLimit = "decay_hf_limit";
	}

	namespace ChorusProperties {
		static const char *waveform = "waveform";
		static const char *phase = "phase";
		static const char *rate = "rate";
		static const char *depth = "depth";
		static const char *feedback = "feedback";
		static const char *delay = "delay";
	}

	namespace DistortionProperties {
		static const char *edge = "edge";
		static const char *gain = "gain";
		static const char *lowpassCutoff = "lowpass_cutoff";
		static const char *eqCenter = "eq_center";
		static const char *eqBandwidth = "eq_bandwidth";
	}

	namespace EchoProperties {
		static const char *delay = "delay";
		static const char *lrDelay = "lr_delay";
		static const char *damping = "damping";
		static const char *feedback = "feedback";
		static const char *spread = "spread";
	}

	namespace FlangerProperties {
		static const char *waveform = "waveform";
		static const char *phase = "phase";
		static const char *rate = "rate";
		static const char *depth = "depth";
		static const char *feedback = "feedback";
		static const char *delay = "delay";
	}

	namespace FrequencyShifterProperties {
		static const char *frequency = "frequency";
		static const char *leftDirection = "left_direction";
		static const char *rightDirection = "right_direction";
	}

	namespace VocalMorpherProperties {
		static const char *phonemeA = "phoneme_a";
		static const char *phonemeACoarseTuning = "phoneme_a_coarse_tuning";
		static const char *phonemeB = "phoneme_b";
		static const char *phonemeBCoarseTuning = "phoneme_b_coarse_tuning";
		static const char *waveform = "waveform";
		static const char *rate = "rate";
	}

	namespace PitchShifterProperties {
		static const char *coarseTune = "coarse_tune";
		static const char *fineTune = "fine_tune";
	}

	namespace RingModulatorProperties {
		static const char *frequency = "frequency";
		static const char *highpassCutoff = "highpass_cutoff";
		static const char *waveform = "waveform";
	}

	namespace AutoWahProperties {
		static const char *attackTime = "attack_time";
		static const char *releaseTime = "release_time";
		static const char *resonance = "resonance";
		static const char *peakGain = "peak_gain";
	}

	namespace CompressorProperties {
		static const char *onOff = "on_off";
	}

	namespace EqualizerProperties {
		static const char *lowGain = "low_gain";
		static const char *lowCutoff = "low_cutoff";
		static const char *mid1Gain = "mid1_gain";
		static const char *mid1Center = "mid1_center";
		static const char *mid1Width = "mid1_width";
		static const char *mid2Gain = "mid2_gain";
		static const char *mid2Center = "mid2_center";
		static const char *mid2Width = "mid2_width";
		static const char *highGain = "high_gain";
		static const char *highCutoff = "high_cutoff";
	}

}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAudioEffectProperties::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_createtable(L, 0, 70);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<AudioEffectProperties>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::type, type);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setType, setType);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minReverbProperties, minReverbProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxReverbProperties, maxReverbProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::reverbProperties, reverbProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setReverbProperties, setReverbProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetReverbProperties, resetReverbProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minEaxReverbProperties, minEaxReverbProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxEaxReverbProperties, maxEaxReverbProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::eaxReverbProperties, eaxReverbProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setEaxReverbProperties, setEaxReverbProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetEaxReverbProperties, resetEaxReverbProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::loadEfxReverbPreset, loadEfxReverbPreset);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minChorusProperties, minChorusProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxChorusProperties, maxChorusProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::chorusProperties, chorusProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setChorusProperties, setChorusProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetChorusProperties, resetChorusProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minDistortionProperties, minDistortionProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxDistortionProperties, maxDistortionProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::distortionProperties, distortionProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setDistortionProperties, setDistortionProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetDistortionProperties, resetDistortionProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minEchoProperties, minEchoProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxEchoProperties, maxEchoProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::echoProperties, echoProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setEchoProperties, setEchoProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetEchoProperties, resetEchoProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minFlangerProperties, minFlangerProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxFlangerProperties, maxFlangerProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::flangerProperties, flangerProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setFlangerProperties, setFlangerProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetFlangerProperties, resetFlangerProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minFrequencyShifterProperties, minFrequencyShifterProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxFrequencyShifterProperties, maxFrequencyShifterProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::frequencyShifterProperties, frequencyShifterProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setFrequencyShifterProperties, setFrequencyShifterProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetFrequencyShifterProperties, resetFrequencyShifterProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minVocalMorpherProperties, minVocalMorpherProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxVocalMorpherProperties, maxVocalMorpherProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::vocalMorpherProperties, vocalMorpherProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setVocalMorpherProperties, setVocalMorpherProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetVocalMorpherProperties, resetVocalMorpherProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minPitchShifterProperties, minPitchShifterProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxPitchShifterProperties, maxPitchShifterProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::pitchShifterProperties, pitchShifterProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setPitchShifterProperties, setPitchShifterProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetPitchShifterProperties, resetPitchShifterProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minRingModulatorProperties, minRingModulatorProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxRingModulatorProperties, maxRingModulatorProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::ringModulatorProperties, ringModulatorProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setRingModulatorProperties, setRingModulatorProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetRingModulatorProperties, resetRingModulatorProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minAutoWahProperties, minAutoWahProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxAutoWahProperties, maxAutoWahProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::autoWahProperties, autoWahProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setAutoWahProperties, setAutoWahProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetAutoWahProperties, resetAutoWahProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minCompressorProperties, minCompressorProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxCompressorProperties, maxCompressorProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::compressorProperties, compressorProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setCompressorProperties, setCompressorProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetCompressorProperties, resetCompressorProperties);

	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::minEqualizerProperties, minEqualizerProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::maxEqualizerProperties, maxEqualizerProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::equalizerProperties, equalizerProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::setEqualizerProperties, setEqualizerProperties);
	LuaUtils::addFunction(L, LuaNames::AudioEffectProperties::resetEqualizerProperties, resetEqualizerProperties);

	lua_setfield(L, -2, LuaNames::AudioEffectProperties::AudioEffectProperties);
}

void LuaAudioEffectProperties::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 113);

	// Default Presets (26)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::GENERIC, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::GENERIC));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PADDED_CELL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PADDED_CELL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::BATHROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::BATHROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::LIVING_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::LIVING_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::STONE_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::STONE_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::AUDITORIUM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::AUDITORIUM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CONCERT_HALL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CONCERT_HALL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CAVE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CAVE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ARENA, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ARENA));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::HANGAR, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::HANGAR));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CARPETED_HALLWAY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CARPETED_HALLWAY));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::HALLWAY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::HALLWAY));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::STONE_CORRIDOR, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::STONE_CORRIDOR));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ALLEY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ALLEY));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::FOREST, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::FOREST));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CITY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CITY));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::MOUNTAINS, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::MOUNTAINS));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::QUARRY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::QUARRY));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PLAIN, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PLAIN));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PARKING_LOT, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PARKING_LOT));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SEWER_PIPE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SEWER_PIPE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::UNDERWATER, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::UNDERWATER));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DRUGGED, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DRUGGED));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DIZZY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DIZZY));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PSYCHOTIC, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PSYCHOTIC));
	// Castle Presets (9)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CASTLE_SMALL_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CASTLE_SMALL_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CASTLE_SHORT_PASSAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CASTLE_SHORT_PASSAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CASTLE_MEDIUM_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CASTLE_MEDIUM_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CASTLE_LARGE_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CASTLE_LARGE_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CASTLE_LONG_PASSAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CASTLE_LONG_PASSAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CASTLE_HALL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CASTLE_HALL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CASTLE_CUPBOARD, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CASTLE_CUPBOARD));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CASTLE_COURTYARD, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CASTLE_COURTYARD));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CASTLE_ALCOVE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CASTLE_ALCOVE));
	// Factory Presets (9)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::FACTORY_SMALL_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::FACTORY_SMALL_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::FACTORY_SHORT_PASSAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::FACTORY_SHORT_PASSAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::FACTORY_MEDIUM_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::FACTORY_MEDIUM_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::FACTORY_LARGE_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::FACTORY_LARGE_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::FACTORY_LONG_PASSAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::FACTORY_LONG_PASSAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::FACTORY_HALL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::FACTORY_HALL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::FACTORY_CUPBOARD, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::FACTORY_CUPBOARD));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::FACTORY_COURTYARD, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::FACTORY_COURTYARD));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::FACTORY_ALCOVE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::FACTORY_ALCOVE));
	// Ice Palace Presets (9)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ICEPALACE_SMALL_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ICEPALACE_SMALL_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ICEPALACE_SHORT_PASSAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ICEPALACE_SHORT_PASSAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ICEPALACE_MEDIUM_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ICEPALACE_MEDIUM_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ICEPALACE_LARGE_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ICEPALACE_LARGE_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ICEPALACE_LONG_PASSAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ICEPALACE_LONG_PASSAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ICEPALACE_HALL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ICEPALACE_HALL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ICEPALACE_CUPBOARD, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ICEPALACE_CUPBOARD));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ICEPALACE_COURTYARD, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ICEPALACE_COURTYARD));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::ICEPALACE_ALCOVE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::ICEPALACE_ALCOVE));
	// Space Station Presets (8)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPACE_STATION_SMALL_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPACE_STATION_SMALL_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPACE_STATION_SHORT_PASSAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPACE_STATION_SHORT_PASSAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPACE_STATION_MEDIUM_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPACE_STATION_MEDIUM_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPACE_STATION_LARGE_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPACE_STATION_LARGE_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPACE_STATION_LONG_PASSAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPACE_STATION_LONG_PASSAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPACE_STATION_HALL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPACE_STATION_HALL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPACE_STATION_CUPBOARD, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPACE_STATION_CUPBOARD));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPACE_STATION_ALCOVE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPACE_STATION_ALCOVE));
	// Wooden Galleon Presets (9)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_SMALL_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_SMALL_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_SHORT_PASSAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_SHORT_PASSAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_MEDIUM_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_MEDIUM_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_LARGE_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_LARGE_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_LONG_PASSAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_LONG_PASSAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_HALL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_HALL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_CUPBOARD, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_CUPBOARD));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_COURTYARD, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_COURTYARD));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_ALCOVE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::WOODEN_GALLEON_ALCOVE));
	// Sports Presets (7)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPORT_EMPTY_STADIUM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPORT_EMPTY_STADIUM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPORT_SQUASH_COURT, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPORT_SQUASH_COURT));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPORT_SMALL_SWIMMING_POOL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPORT_SMALL_SWIMMING_POOL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPORT_LARGE_SWIMMING_POOL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPORT_LARGE_SWIMMING_POOL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPORT_GYMNASIUM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPORT_GYMNASIUM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPORT_FULL_STADIUM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPORT_FULL_STADIUM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SPORT_STADIUM_TANNOY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SPORT_STADIUM_TANNOY));
	// Prefab Presets (5)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PREFAB_WORKSHOP, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PREFAB_WORKSHOP));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PREFAB_SCHOOL_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PREFAB_SCHOOL_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PREFAB_PRACTISE_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PREFAB_PRACTISE_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PREFAB_OUTHOUSE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PREFAB_OUTHOUSE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PREFAB_CARAVAN, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PREFAB_CARAVAN));
	// Dome and Pipe Presets (6)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DOME_TOMB, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DOME_TOMB));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PIPE_SMALL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PIPE_SMALL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DOME_SAINT_PAULS, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DOME_SAINT_PAULS));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PIPE_LONGTHIN, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PIPE_LONGTHIN));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PIPE_LARGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PIPE_LARGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::PIPE_RESONANT, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::PIPE_RESONANT));
	// Outdoors Presets (5)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::OUTDOORS_BACKYARD, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::OUTDOORS_BACKYARD));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::OUTDOORS_ROLLING_PLAINS, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::OUTDOORS_ROLLING_PLAINS));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::OUTDOORS_DEEP_CANYON, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::OUTDOORS_DEEP_CANYON));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::OUTDOORS_CREEK, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::OUTDOORS_CREEK));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::OUTDOORS_VALLEY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::OUTDOORS_VALLEY));
	// Mood Presets (3)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::MOOD_HEAVEN, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::MOOD_HEAVEN));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::MOOD_HELL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::MOOD_HELL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::MOOD_MEMORY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::MOOD_MEMORY));
	// Driving Presets (8)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DRIVING_COMMENTATOR, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DRIVING_COMMENTATOR));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DRIVING_PIT_GARAGE, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DRIVING_PIT_GARAGE));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DRIVING_INCAR_RACER, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DRIVING_INCAR_RACER));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DRIVING_INCAR_SPORTS, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DRIVING_INCAR_SPORTS));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DRIVING_INCAR_LUXURY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DRIVING_INCAR_LUXURY));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DRIVING_FULL_GRANDSTAND, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DRIVING_FULL_GRANDSTAND));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DRIVING_EMPTY_GRANDSTAND, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DRIVING_EMPTY_GRANDSTAND));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DRIVING_TUNNEL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DRIVING_TUNNEL));
	// City Presets (6)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CITY_STREETS, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CITY_STREETS));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CITY_SUBWAY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CITY_SUBWAY));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CITY_MUSEUM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CITY_MUSEUM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CITY_LIBRARY, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CITY_LIBRARY));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CITY_UNDERPASS, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CITY_UNDERPASS));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CITY_ABANDONED, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CITY_ABANDONED));
	// Misc. Presets (3)
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::DUSTY_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::DUSTY_ROOM));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::CHAPEL, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::CHAPEL));
	LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EfxReverbPresets::SMALL_WATER_ROOM, static_cast<int64_t>(AudioEffectProperties::EfxReverbPresets::SMALL_WATER_ROOM));

	lua_setfield(L, -2, LuaNames::AudioEffectProperties::EfxReverbPresets::EfxReverbPresets);
}

void LuaAudioEffectProperties::release(void *object)
{
	AudioEffectProperties *audioEffectProps = reinterpret_cast<AudioEffectProperties *>(object);
	delete audioEffectProps;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaAudioEffectProperties::newObject(lua_State *L)
{
	LuaClassTracker<AudioEffectProperties>::newObject(L);

	return 1;
}

int LuaAudioEffectProperties::type(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		LuaUtils::push(L, static_cast<int64_t>(audioEffectProps->type()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setType(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);
	const AudioEffect::Type type = static_cast<AudioEffect::Type>(LuaUtils::retrieve<int64_t>(L, -1));

	if (audioEffectProps)
		audioEffectProps->setType(type);

	return 0;
}

//////////////////////////////
// Reverb
//////////////////////////////

namespace {

	AudioEffectProperties::ReverbProperties retrieveReverbProperties(lua_State *L, int index)
	{
		AudioEffectProperties::ReverbProperties props;

		props.density = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::density);
		props.diffusion = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::diffusion);
		props.gain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::gain);
		props.gainHF = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::gainHF);
		props.decayTime = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::decayTime);
		props.decayHFRatio = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::decayHFRatio);
		props.reflectionsGain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::reflectionsGain);
		props.reflectionsDelay = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::reflectionsDelay);
		props.lateReverbGain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::lateReverbGain);
		props.lateReverbDelay = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::lateReverbDelay);
		props.airAbsorptionGainHF = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::airAbsorptionGainHF);
		props.roomRolloffFactor = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::roomRolloffFactor);
		props.decayHFLimit = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::ReverbProperties::decayHFLimit);

		return props;
	}

	void pushReverbProperties(lua_State *L, const AudioEffectProperties::ReverbProperties &props)
	{
		lua_createtable(L, 0, 13);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::density, props.density);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::diffusion, props.diffusion);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::gain, props.gain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::gainHF, props.gainHF);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::decayTime, props.decayTime);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::decayHFRatio, props.decayHFRatio);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::reflectionsGain, props.reflectionsGain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::reflectionsDelay, props.reflectionsDelay);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::lateReverbGain, props.lateReverbGain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::lateReverbDelay, props.lateReverbDelay);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::airAbsorptionGainHF, props.airAbsorptionGainHF);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::roomRolloffFactor, props.roomRolloffFactor);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ReverbProperties::decayHFLimit, props.decayHFLimit);
	}
}

int LuaAudioEffectProperties::minReverbProperties(lua_State *L)
{
	pushReverbProperties(L, AudioEffectProperties::minReverbProperties());

	return 1;
}

int LuaAudioEffectProperties::maxReverbProperties(lua_State *L)
{
	pushReverbProperties(L, AudioEffectProperties::maxReverbProperties());

	return 1;
}

int LuaAudioEffectProperties::reverbProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushReverbProperties(L, audioEffectProps->reverbProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setReverbProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::ReverbProperties props = retrieveReverbProperties(L, -1);
		audioEffectProps->setReverbProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetReverbProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetReverbProperties();

	return 0;
}

//////////////////////////////
// Reverb
//////////////////////////////

namespace {

	AudioEffectProperties::EaxReverbProperties retrieveEaxReverbProperties(lua_State *L, int index)
	{
		AudioEffectProperties::EaxReverbProperties props;

		props.density = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::density);
		props.diffusion = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::diffusion);
		props.gain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::gain);
		props.gainHF = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::gainHF);
		props.gainLF = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::gainLF);
		props.decayTime = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::decayTime);
		props.decayHFRatio = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::decayHFRatio);
		props.decayLFRatio = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::decayLFRatio);
		props.reflectionsGain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::reflectionsGain);
		props.reflectionsDelay = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::reflectionsDelay);
		const Vector3f reflectionsPan = LuaVector3fUtils::retrieveTableField(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::reflectionsPan);
		props.reflectionsPan[0] = reflectionsPan.x;
		props.reflectionsPan[1] = reflectionsPan.y;
		props.reflectionsPan[2] = reflectionsPan.z;
		props.lateReverbGain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::lateReverbGain);
		props.lateReverbDelay = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::lateReverbDelay);
		const Vector3f lateReverbPan = LuaVector3fUtils::retrieveTableField(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::lateReverbPan);
		props.lateReverbPan[0] = lateReverbPan.x;
		props.lateReverbPan[1] = lateReverbPan.y;
		props.lateReverbPan[2] = lateReverbPan.z;
		props.echoTime = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::echoTime);
		props.echoDepth = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::echoDepth);
		props.modulationTime = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::modulationTime);
		props.modulationDepth = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::modulationDepth);
		props.airAbsorptionGainHF = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::airAbsorptionGainHF);
		props.hfReference = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::hfReference);
		props.lfReference = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::lfReference);
		props.roomRolloffFactor = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::roomRolloffFactor);
		props.decayHFLimit = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::EaxReverbProperties::decayHFLimit);

		return props;
	}

	void pushEaxReverbProperties(lua_State *L, const AudioEffectProperties::EaxReverbProperties &props)
	{
		lua_createtable(L, 0, 23);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::density, props.density);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::diffusion, props.diffusion);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::gain, props.gain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::gainHF, props.gainHF);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::gainLF, props.gainLF);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::decayTime, props.decayTime);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::decayHFRatio, props.decayHFRatio);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::decayLFRatio, props.decayLFRatio);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::reflectionsGain, props.reflectionsGain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::reflectionsDelay, props.reflectionsDelay);
		const Vector3f reflectionsPan(props.reflectionsPan[0], props.reflectionsPan[1], props.reflectionsPan[2]);
		LuaVector3fUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::reflectionsPan, reflectionsPan);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::lateReverbGain, props.lateReverbGain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::lateReverbDelay, props.lateReverbDelay);
		const Vector3f lateReverbPan(props.lateReverbPan[0], props.lateReverbPan[1], props.lateReverbPan[2]);
		LuaVector3fUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::lateReverbPan, lateReverbPan);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::echoTime, props.echoTime);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::echoDepth, props.echoDepth);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::modulationTime, props.modulationTime);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::modulationDepth, props.modulationDepth);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::airAbsorptionGainHF, props.airAbsorptionGainHF);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::hfReference, props.hfReference);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::lfReference, props.lfReference);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::roomRolloffFactor, props.roomRolloffFactor);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EaxReverbProperties::decayHFLimit, props.decayHFLimit);
	}
}

int LuaAudioEffectProperties::minEaxReverbProperties(lua_State *L)
{
	pushEaxReverbProperties(L, AudioEffectProperties::minEaxReverbProperties());

	return 1;
}

int LuaAudioEffectProperties::maxEaxReverbProperties(lua_State *L)
{
	pushEaxReverbProperties(L, AudioEffectProperties::maxEaxReverbProperties());

	return 1;
}

int LuaAudioEffectProperties::eaxReverbProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushEaxReverbProperties(L, audioEffectProps->eaxReverbProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setEaxReverbProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::EaxReverbProperties props = retrieveEaxReverbProperties(L, -1);
		audioEffectProps->setEaxReverbProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetEaxReverbProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetEaxReverbProperties();

	return 0;
}

int LuaAudioEffectProperties::loadEfxReverbPreset(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);
	const AudioEffectProperties::EfxReverbPresets preset = static_cast<AudioEffectProperties::EfxReverbPresets>(LuaUtils::retrieve<int64_t>(L, -1));

	if (audioEffectProps)
	{
		AudioEffectProperties::EaxReverbProperties props = audioEffectProps->eaxReverbProperties();
		props.loadPreset(preset);
		audioEffectProps->setEaxReverbProperties(props);
	}

	return 0;
}

//////////////////////////////
// Chorus
//////////////////////////////

namespace {

	AudioEffectProperties::ChorusProperties retrieveChorusProperties(lua_State *L, int index)
	{
		AudioEffectProperties::ChorusProperties props;

		props.waveform = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::ChorusProperties::waveform);
		props.phase = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::ChorusProperties::phase);
		props.rate = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ChorusProperties::rate);
		props.depth = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ChorusProperties::depth);
		props.feedback = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ChorusProperties::feedback);
		props.delay = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::ChorusProperties::delay);

		return props;
	}

	void pushChorusProperties(lua_State *L, const AudioEffectProperties::ChorusProperties &props)
	{
		lua_createtable(L, 0, 6);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ChorusProperties::waveform, props.waveform);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ChorusProperties::phase, props.phase);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ChorusProperties::rate, props.rate);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ChorusProperties::depth, props.depth);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ChorusProperties::feedback, props.feedback);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::ChorusProperties::delay, props.delay);
	}
}

int LuaAudioEffectProperties::minChorusProperties(lua_State *L)
{
	pushChorusProperties(L, AudioEffectProperties::minChorusProperties());

	return 1;
}

int LuaAudioEffectProperties::maxChorusProperties(lua_State *L)
{
	pushChorusProperties(L, AudioEffectProperties::maxChorusProperties());

	return 1;
}

int LuaAudioEffectProperties::chorusProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushChorusProperties(L, audioEffectProps->chorusProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setChorusProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::ChorusProperties props = retrieveChorusProperties(L, -1);
		audioEffectProps->setChorusProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetChorusProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetChorusProperties();

	return 0;
}

//////////////////////////////
// Distortion
//////////////////////////////

namespace {

	AudioEffectProperties::DistortionProperties retrieveDistortionProperties(lua_State *L, int index)
	{
		AudioEffectProperties::DistortionProperties props;

		props.edge = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::DistortionProperties::edge);
		props.gain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::DistortionProperties::gain);
		props.lowpassCutoff = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::DistortionProperties::lowpassCutoff);
		props.eqCenter = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::DistortionProperties::eqCenter);
		props.eqBandwidth = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::DistortionProperties::eqBandwidth);

		return props;
	}

	void pushDistortionProperties(lua_State *L, const AudioEffectProperties::DistortionProperties &props)
	{
		lua_createtable(L, 0, 5);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::DistortionProperties::edge, props.edge);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::DistortionProperties::gain, props.gain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::DistortionProperties::lowpassCutoff, props.lowpassCutoff);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::DistortionProperties::eqCenter, props.eqCenter);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::DistortionProperties::eqBandwidth, props.eqBandwidth);
	}
}

int LuaAudioEffectProperties::minDistortionProperties(lua_State *L)
{
	pushDistortionProperties(L, AudioEffectProperties::minDistortionProperties());

	return 1;
}

int LuaAudioEffectProperties::maxDistortionProperties(lua_State *L)
{
	pushDistortionProperties(L, AudioEffectProperties::maxDistortionProperties());

	return 1;
}

int LuaAudioEffectProperties::distortionProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushDistortionProperties(L, audioEffectProps->distortionProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setDistortionProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::DistortionProperties props = retrieveDistortionProperties(L, -1);
		audioEffectProps->setDistortionProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetDistortionProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetDistortionProperties();

	return 0;
}

//////////////////////////////
// Echo
//////////////////////////////

namespace {

	AudioEffectProperties::EchoProperties retrieveEchoProperties(lua_State *L, int index)
	{
		AudioEffectProperties::EchoProperties props;

		props.delay = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EchoProperties::delay);
		props.lrDelay = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EchoProperties::lrDelay);
		props.damping = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EchoProperties::damping);
		props.feedback = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EchoProperties::feedback);
		props.spread = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EchoProperties::spread);

		return props;
	}

	void pushEchoProperties(lua_State *L, const AudioEffectProperties::EchoProperties &props)
	{
		lua_createtable(L, 0, 5);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EchoProperties::delay, props.delay);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EchoProperties::lrDelay, props.lrDelay);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EchoProperties::damping, props.damping);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EchoProperties::feedback, props.feedback);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EchoProperties::spread, props.spread);
	}
}

int LuaAudioEffectProperties::minEchoProperties(lua_State *L)
{
	pushEchoProperties(L, AudioEffectProperties::minEchoProperties());

	return 1;
}

int LuaAudioEffectProperties::maxEchoProperties(lua_State *L)
{
	pushEchoProperties(L, AudioEffectProperties::maxEchoProperties());

	return 1;
}

int LuaAudioEffectProperties::echoProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushEchoProperties(L, audioEffectProps->echoProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setEchoProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::EchoProperties props = retrieveEchoProperties(L, -1);
		audioEffectProps->setEchoProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetEchoProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetEchoProperties();

	return 0;
}

//////////////////////////////
// Flanger
//////////////////////////////

namespace {

	AudioEffectProperties::FlangerProperties retrieveFlangerProperties(lua_State *L, int index)
	{
		AudioEffectProperties::FlangerProperties props;

		props.waveform = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::FlangerProperties::waveform);
		props.phase = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::FlangerProperties::phase);
		props.rate = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::FlangerProperties::rate);
		props.depth = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::FlangerProperties::depth);
		props.feedback = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::FlangerProperties::feedback);
		props.delay = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::FlangerProperties::delay);

		return props;
	}

	void pushFlangerProperties(lua_State *L, const AudioEffectProperties::FlangerProperties &props)
	{
		lua_createtable(L, 0, 6);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::FlangerProperties::waveform, props.waveform);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::FlangerProperties::phase, props.phase);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::FlangerProperties::rate, props.rate);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::FlangerProperties::depth, props.depth);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::FlangerProperties::feedback, props.feedback);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::FlangerProperties::delay, props.delay);
	}
}

int LuaAudioEffectProperties::minFlangerProperties(lua_State *L)
{
	pushFlangerProperties(L, AudioEffectProperties::minFlangerProperties());

	return 1;
}

int LuaAudioEffectProperties::maxFlangerProperties(lua_State *L)
{
	pushFlangerProperties(L, AudioEffectProperties::maxFlangerProperties());

	return 1;
}

int LuaAudioEffectProperties::flangerProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushFlangerProperties(L, audioEffectProps->flangerProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setFlangerProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::FlangerProperties props = retrieveFlangerProperties(L, -1);
		audioEffectProps->setFlangerProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetFlangerProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetFlangerProperties();

	return 0;
}

//////////////////////////////
// Frequency Shifter
//////////////////////////////

namespace {

	AudioEffectProperties::FrequencyShifterProperties retrieveFrequencyShifterProperties(lua_State *L, int index)
	{
		AudioEffectProperties::FrequencyShifterProperties props;

		props.frequency = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::FrequencyShifterProperties::frequency);
		props.leftDirection = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::FrequencyShifterProperties::leftDirection);
		props.rightDirection = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::FrequencyShifterProperties::rightDirection);

		return props;
	}

	void pushFrequencyShifterProperties(lua_State *L, const AudioEffectProperties::FrequencyShifterProperties &props)
	{
		lua_createtable(L, 0, 3);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::FrequencyShifterProperties::frequency, props.frequency);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::FrequencyShifterProperties::leftDirection, props.leftDirection);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::FrequencyShifterProperties::rightDirection, props.rightDirection);
	}
}

int LuaAudioEffectProperties::minFrequencyShifterProperties(lua_State *L)
{
	pushFrequencyShifterProperties(L, AudioEffectProperties::minFrequencyShifterProperties());

	return 1;
}

int LuaAudioEffectProperties::maxFrequencyShifterProperties(lua_State *L)
{
	pushFrequencyShifterProperties(L, AudioEffectProperties::maxFrequencyShifterProperties());

	return 1;
}

int LuaAudioEffectProperties::frequencyShifterProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushFrequencyShifterProperties(L, audioEffectProps->frequencyShifterProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setFrequencyShifterProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::FrequencyShifterProperties props = retrieveFrequencyShifterProperties(L, -1);
		audioEffectProps->setFrequencyShifterProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetFrequencyShifterProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetFrequencyShifterProperties();

	return 0;
}

//////////////////////////////
// Vocal Morpher
//////////////////////////////

namespace {

	AudioEffectProperties::VocalMorpherProperties retrieveVocalMorpherProperties(lua_State *L, int index)
	{
		AudioEffectProperties::VocalMorpherProperties props;

		props.phonemeA = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::VocalMorpherProperties::phonemeA);
		props.phonemeACoarseTuning = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::VocalMorpherProperties::phonemeACoarseTuning);
		props.phonemeB = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::VocalMorpherProperties::phonemeB);
		props.phonemeBCoarseTuning = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::VocalMorpherProperties::phonemeBCoarseTuning);
		props.waveform = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::VocalMorpherProperties::waveform);
		props.rate = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::VocalMorpherProperties::rate);

		return props;
	}

	void pushVocalMorpherProperties(lua_State *L, const AudioEffectProperties::VocalMorpherProperties &props)
	{
		lua_createtable(L, 0, 6);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::VocalMorpherProperties::phonemeA, props.phonemeA);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::VocalMorpherProperties::phonemeACoarseTuning, props.phonemeACoarseTuning);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::VocalMorpherProperties::phonemeB, props.phonemeB);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::VocalMorpherProperties::phonemeBCoarseTuning, props.phonemeBCoarseTuning);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::VocalMorpherProperties::waveform, props.waveform);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::VocalMorpherProperties::rate, props.rate);
	}
}

int LuaAudioEffectProperties::minVocalMorpherProperties(lua_State *L)
{
	pushVocalMorpherProperties(L, AudioEffectProperties::minVocalMorpherProperties());

	return 1;
}

int LuaAudioEffectProperties::maxVocalMorpherProperties(lua_State *L)
{
	pushVocalMorpherProperties(L, AudioEffectProperties::maxVocalMorpherProperties());

	return 1;
}

int LuaAudioEffectProperties::vocalMorpherProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushVocalMorpherProperties(L, audioEffectProps->vocalMorpherProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setVocalMorpherProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::VocalMorpherProperties props = retrieveVocalMorpherProperties(L, -1);
		audioEffectProps->setVocalMorpherProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetVocalMorpherProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetVocalMorpherProperties();

	return 0;
}

//////////////////////////////
// Pitch Shifter
//////////////////////////////

namespace {

	AudioEffectProperties::PitchShifterProperties retrievePitchShifterProperties(lua_State *L, int index)
	{
		AudioEffectProperties::PitchShifterProperties props;

		props.coarseTune = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::PitchShifterProperties::coarseTune);
		props.fineTune = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::PitchShifterProperties::fineTune);

		return props;
	}

	void pushPitchShifterProperties(lua_State *L, const AudioEffectProperties::PitchShifterProperties &props)
	{
		lua_createtable(L, 0, 2);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::PitchShifterProperties::coarseTune, props.coarseTune);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::PitchShifterProperties::fineTune, props.fineTune);
	}
}

int LuaAudioEffectProperties::minPitchShifterProperties(lua_State *L)
{
	pushPitchShifterProperties(L, AudioEffectProperties::minPitchShifterProperties());

	return 1;
}

int LuaAudioEffectProperties::maxPitchShifterProperties(lua_State *L)
{
	pushPitchShifterProperties(L, AudioEffectProperties::maxPitchShifterProperties());

	return 1;
}

int LuaAudioEffectProperties::pitchShifterProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushPitchShifterProperties(L, audioEffectProps->pitchShifterProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setPitchShifterProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::PitchShifterProperties props = retrievePitchShifterProperties(L, -1);
		audioEffectProps->setPitchShifterProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetPitchShifterProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetPitchShifterProperties();

	return 0;
}

//////////////////////////////
// Ring Modulator
//////////////////////////////

namespace {

	AudioEffectProperties::RingModulatorProperties retrieveRingModulatorProperties(lua_State *L, int index)
	{
		AudioEffectProperties::RingModulatorProperties props;

		props.frequency = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::RingModulatorProperties::frequency);
		props.highpassCutoff = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::RingModulatorProperties::highpassCutoff);
		props.waveform = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::RingModulatorProperties::waveform);

		return props;
	}

	void pushRingModulatorProperties(lua_State *L, const AudioEffectProperties::RingModulatorProperties &props)
	{
		lua_createtable(L, 0, 3);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::RingModulatorProperties::frequency, props.frequency);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::RingModulatorProperties::highpassCutoff, props.highpassCutoff);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::RingModulatorProperties::waveform, props.waveform);
	}
}

int LuaAudioEffectProperties::minRingModulatorProperties(lua_State *L)
{
	pushRingModulatorProperties(L, AudioEffectProperties::minRingModulatorProperties());

	return 1;
}

int LuaAudioEffectProperties::maxRingModulatorProperties(lua_State *L)
{
	pushRingModulatorProperties(L, AudioEffectProperties::maxRingModulatorProperties());

	return 1;
}

int LuaAudioEffectProperties::ringModulatorProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushRingModulatorProperties(L, audioEffectProps->ringModulatorProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setRingModulatorProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::RingModulatorProperties props = retrieveRingModulatorProperties(L, -1);
		audioEffectProps->setRingModulatorProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetRingModulatorProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetRingModulatorProperties();

	return 0;
}

//////////////////////////////
// Auto Wah
//////////////////////////////

namespace {

	AudioEffectProperties::AutoWahProperties retrieveAutoWahProperties(lua_State *L, int index)
	{
		AudioEffectProperties::AutoWahProperties props;

		props.attackTime = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::AutoWahProperties::attackTime);
		props.releaseTime = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::AutoWahProperties::releaseTime);
		props.resonance = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::AutoWahProperties::resonance);
		props.peakGain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::AutoWahProperties::peakGain);

		return props;
	}

	void pushAutoWahProperties(lua_State *L, const AudioEffectProperties::AutoWahProperties &props)
	{
		lua_createtable(L, 0, 4);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::AutoWahProperties::attackTime, props.attackTime);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::AutoWahProperties::releaseTime, props.releaseTime);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::AutoWahProperties::resonance, props.resonance);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::AutoWahProperties::peakGain, props.peakGain);
	}
}

int LuaAudioEffectProperties::minAutoWahProperties(lua_State *L)
{
	pushAutoWahProperties(L, AudioEffectProperties::minAutoWahProperties());

	return 1;
}

int LuaAudioEffectProperties::maxAutoWahProperties(lua_State *L)
{
	pushAutoWahProperties(L, AudioEffectProperties::maxAutoWahProperties());

	return 1;
}

int LuaAudioEffectProperties::autoWahProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushAutoWahProperties(L, audioEffectProps->autoWahProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setAutoWahProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::AutoWahProperties props = retrieveAutoWahProperties(L, -1);
		audioEffectProps->setAutoWahProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetAutoWahProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetAutoWahProperties();

	return 0;
}

//////////////////////////////
// Compressor
//////////////////////////////

namespace {

	AudioEffectProperties::CompressorProperties retrieveCompressorProperties(lua_State *L, int index)
	{
		AudioEffectProperties::CompressorProperties props;

		props.onOff = LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioEffectProperties::CompressorProperties::onOff);

		return props;
	}

	void pushCompressorProperties(lua_State *L, const AudioEffectProperties::CompressorProperties &props)
	{
		lua_createtable(L, 0, 1);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::CompressorProperties::onOff, props.onOff);
	}
}

int LuaAudioEffectProperties::minCompressorProperties(lua_State *L)
{
	pushCompressorProperties(L, AudioEffectProperties::minCompressorProperties());

	return 1;
}

int LuaAudioEffectProperties::maxCompressorProperties(lua_State *L)
{
	pushCompressorProperties(L, AudioEffectProperties::maxCompressorProperties());

	return 1;
}

int LuaAudioEffectProperties::compressorProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushCompressorProperties(L, audioEffectProps->compressorProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setCompressorProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::CompressorProperties props = retrieveCompressorProperties(L, -1);
		audioEffectProps->setCompressorProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetCompressorProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetCompressorProperties();

	return 0;
}

//////////////////////////////
// Equalizer
//////////////////////////////

namespace {

	AudioEffectProperties::EqualizerProperties retrieveEqualizerProperties(lua_State *L, int index)
	{
		AudioEffectProperties::EqualizerProperties props;

		props.lowGain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EqualizerProperties::lowGain);
		props.lowCutoff = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EqualizerProperties::lowCutoff);
		props.mid1Gain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EqualizerProperties::mid1Gain);
		props.mid1Center = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EqualizerProperties::mid1Center);
		props.mid1Width = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EqualizerProperties::mid1Width);
		props.mid2Gain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EqualizerProperties::mid2Gain);
		props.mid2Center = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EqualizerProperties::mid2Center);
		props.mid2Width = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EqualizerProperties::mid2Width);
		props.highGain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EqualizerProperties::highGain);
		props.highCutoff = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioEffectProperties::EqualizerProperties::highCutoff);

		return props;
	}

	void pushEqualizerProperties(lua_State *L, const AudioEffectProperties::EqualizerProperties &props)
	{
		lua_createtable(L, 0, 10);

		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EqualizerProperties::lowGain, props.lowGain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EqualizerProperties::lowCutoff, props.lowCutoff);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EqualizerProperties::mid1Gain, props.mid1Gain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EqualizerProperties::mid1Center, props.mid1Center);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EqualizerProperties::mid1Width, props.mid1Width);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EqualizerProperties::mid2Gain, props.mid2Gain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EqualizerProperties::mid2Center, props.mid2Center);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EqualizerProperties::mid2Width, props.mid2Width);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EqualizerProperties::highGain, props.highGain);
		LuaUtils::pushField(L, LuaNames::AudioEffectProperties::EqualizerProperties::highCutoff, props.highCutoff);
	}
}

int LuaAudioEffectProperties::minEqualizerProperties(lua_State *L)
{
	pushEqualizerProperties(L, AudioEffectProperties::minEqualizerProperties());

	return 1;
}

int LuaAudioEffectProperties::maxEqualizerProperties(lua_State *L)
{
	pushEqualizerProperties(L, AudioEffectProperties::maxEqualizerProperties());

	return 1;
}

int LuaAudioEffectProperties::equalizerProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		pushEqualizerProperties(L, audioEffectProps->equalizerProperties());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectProperties::setEqualizerProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -2);

	if (audioEffectProps)
	{
		const AudioEffectProperties::EqualizerProperties props = retrieveEqualizerProperties(L, -1);
		audioEffectProps->setEqualizerProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioEffectProperties::resetEqualizerProperties(lua_State *L)
{
	AudioEffectProperties *audioEffectProps = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffectProps)
		audioEffectProps->resetEqualizerProperties();

	return 0;
}

}
