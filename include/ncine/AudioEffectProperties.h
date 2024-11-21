#ifndef CLASS_NCINE_AUDIOEFFECTPROPERTIES
#define CLASS_NCINE_AUDIOEFFECTPROPERTIES

#include "AudioEffect.h"

namespace ncine {

/// A class representing a set of OpenAL effect properties
class DLL_PUBLIC AudioEffectProperties
{
  public:
	struct ReverbProperties
	{
		float density;
		float diffusion;
		float gain;
		float gainHF;
		float decayTime;
		float decayHFRatio;
		float reflectionsGain;
		float reflectionsDelay;
		float lateReverbGain;
		float lateReverbDelay;
		float airAbsorptionGainHF;
		float roomRolloffFactor;
		int decayHFLimit;

		DLL_PUBLIC ReverbProperties();
	};

	enum EfxReverbPresets
	{
		// Default Presets
		GENERIC,
		PADDED_CELL,
		ROOM,
		BATHROOM,
		LIVING_ROOM,
		STONE_ROOM,
		AUDITORIUM,
		CONCERT_HALL,
		CAVE,
		ARENA,
		HANGAR,
		CARPETED_HALLWAY,
		HALLWAY,
		STONE_CORRIDOR,
		ALLEY,
		FOREST,
		CITY,
		MOUNTAINS,
		QUARRY,
		PLAIN,
		PARKING_LOT,
		SEWER_PIPE,
		UNDERWATER,
		DRUGGED,
		DIZZY,
		PSYCHOTIC,
		// Castle Presets
		CASTLE_SMALL_ROOM,
		CASTLE_SHORT_PASSAGE,
		CASTLE_MEDIUM_ROOM,
		CASTLE_LARGE_ROOM,
		CASTLE_LONG_PASSAGE,
		CASTLE_HALL,
		CASTLE_CUPBOARD,
		CASTLE_COURTYARD,
		CASTLE_ALCOVE,
		// Factory Presets
		FACTORY_SMALL_ROOM,
		FACTORY_SHORT_PASSAGE,
		FACTORY_MEDIUM_ROOM,
		FACTORY_LARGE_ROOM,
		FACTORY_LONG_PASSAGE,
		FACTORY_HALL,
		FACTORY_CUPBOARD,
		FACTORY_COURTYARD,
		FACTORY_ALCOVE,
		// Ice Palace Presets
		ICEPALACE_SMALL_ROOM,
		ICEPALACE_SHORT_PASSAGE,
		ICEPALACE_MEDIUM_ROOM,
		ICEPALACE_LARGE_ROOM,
		ICEPALACE_LONG_PASSAGE,
		ICEPALACE_HALL,
		ICEPALACE_CUPBOARD,
		ICEPALACE_COURTYARD,
		ICEPALACE_ALCOVE,
		// Space Station Presets
		SPACE_STATION_SMALL_ROOM,
		SPACE_STATION_SHORT_PASSAGE,
		SPACE_STATION_MEDIUM_ROOM,
		SPACE_STATION_LARGE_ROOM,
		SPACE_STATION_LONG_PASSAGE,
		SPACE_STATION_HALL,
		SPACE_STATION_CUPBOARD,
		SPACE_STATION_ALCOVE,
		// Wooden Galleon Presets
		WOODEN_GALLEON_SMALL_ROOM,
		WOODEN_GALLEON_SHORT_PASSAGE,
		WOODEN_GALLEON_MEDIUM_ROOM,
		WOODEN_GALLEON_LARGE_ROOM,
		WOODEN_GALLEON_LONG_PASSAGE,
		WOODEN_GALLEON_HALL,
		WOODEN_GALLEON_CUPBOARD,
		WOODEN_GALLEON_COURTYARD,
		WOODEN_GALLEON_ALCOVE,
		// Sports Presets
		SPORT_EMPTY_STADIUM,
		SPORT_SQUASH_COURT,
		SPORT_SMALL_SWIMMING_POOL,
		SPORT_LARGE_SWIMMING_POOL,
		SPORT_GYMNASIUM,
		SPORT_FULL_STADIUM,
		SPORT_STADIUM_TANNOY,
		// Prefab Presets
		PREFAB_WORKSHOP,
		PREFAB_SCHOOL_ROOM,
		PREFAB_PRACTISE_ROOM,
		PREFAB_OUTHOUSE,
		PREFAB_CARAVAN,
		// Dome and Pipe Presets
		DOME_TOMB,
		PIPE_SMALL,
		DOME_SAINT_PAULS,
		PIPE_LONGTHIN,
		PIPE_LARGE,
		PIPE_RESONANT,
		// Outdoors Presets
		OUTDOORS_BACKYARD,
		OUTDOORS_ROLLING_PLAINS,
		OUTDOORS_DEEP_CANYON,
		OUTDOORS_CREEK,
		OUTDOORS_VALLEY,
		// Mood Presets
		MOOD_HEAVEN,
		MOOD_HELL,
		MOOD_MEMORY,
		// Driving Presets
		DRIVING_COMMENTATOR,
		DRIVING_PIT_GARAGE,
		DRIVING_INCAR_RACER,
		DRIVING_INCAR_SPORTS,
		DRIVING_INCAR_LUXURY,
		DRIVING_FULL_GRANDSTAND,
		DRIVING_EMPTY_GRANDSTAND,
		DRIVING_TUNNEL,
		// City Presets
		CITY_STREETS,
		CITY_SUBWAY,
		CITY_MUSEUM,
		CITY_LIBRARY,
		CITY_UNDERPASS,
		CITY_ABANDONED,
		// Misc. Presets
		DUSTY_ROOM,
		CHAPEL,
		SMALL_WATER_ROOM,

		COUNT
	};

	struct EaxReverbProperties
	{
		static const float MinPan;
		static const float MaxPan;

		float density;
		float diffusion;
		float gain;
		float gainHF;
		float gainLF;
		float decayTime;
		float decayHFRatio;
		float decayLFRatio;
		float reflectionsGain;
		float reflectionsDelay;
		float reflectionsPan[3];
		float lateReverbGain;
		float lateReverbDelay;
		float lateReverbPan[3];
		float echoTime;
		float echoDepth;
		float modulationTime;
		float modulationDepth;
		float airAbsorptionGainHF;
		float hfReference;
		float lfReference;
		float roomRolloffFactor;
		int decayHFLimit;

		DLL_PUBLIC EaxReverbProperties();
		/// Loads and sets the effect properties from an EFX reverb preset
		DLL_PUBLIC void loadPreset(const EfxReverbPresets preset);
	};

	struct ChorusProperties
	{
		int waveform;
		int phase;
		float rate;
		float depth;
		float feedback;
		float delay;

		DLL_PUBLIC ChorusProperties();
	};

	struct DistortionProperties
	{
		float edge;
		float gain;
		float lowpassCutoff;
		float eqCenter;
		float eqBandwidth;

		DLL_PUBLIC DistortionProperties();
	};

	struct EchoProperties
	{
		float delay;
		float lrDelay;
		float damping;
		float feedback;
		float spread;

		DLL_PUBLIC EchoProperties();
	};

	struct FlangerProperties
	{
		int waveform;
		int phase;
		float rate;
		float depth;
		float feedback;
		float delay;

		DLL_PUBLIC FlangerProperties();
	};

	struct FrequencyShifterProperties
	{
		float frequency;
		int leftDirection;
		int rightDirection;

		DLL_PUBLIC FrequencyShifterProperties();
	};

	struct VocalMorpherProperties
	{
		int phonemeA;
		int phonemeACoarseTuning;
		int phonemeB;
		int phonemeBCoarseTuning;
		int waveform;
		float rate;

		DLL_PUBLIC VocalMorpherProperties();
	};

	struct PitchShifterProperties
	{
		int coarseTune;
		int fineTune;

		DLL_PUBLIC PitchShifterProperties();
	};

	struct RingModulatorProperties
	{
		float frequency;
		float highpassCutoff;
		int waveform;

		DLL_PUBLIC RingModulatorProperties();
	};

	struct AutoWahProperties
	{
		float attackTime;
		float releaseTime;
		float resonance;
		float peakGain;

		DLL_PUBLIC AutoWahProperties();
	};

	struct CompressorProperties
	{
		int onOff;

		DLL_PUBLIC CompressorProperties();
	};

	struct EqualizerProperties
	{
		float lowGain;
		float lowCutoff;
		float mid1Gain;
		float mid1Center;
		float mid1Width;
		float mid2Gain;
		float mid2Center;
		float mid2Width;
		float highGain;
		float highCutoff;

		DLL_PUBLIC EqualizerProperties();
	};

	/// Creates an empty of properties without any effect type
	AudioEffectProperties();
	/// Creates a set of properties for an effect type
	explicit AudioEffectProperties(AudioEffect::Type type);

	/// Default move constructor
	AudioEffectProperties(AudioEffectProperties &&other) = default;
	/// Default move assignment operator
	AudioEffectProperties &operator=(AudioEffectProperties &&other) = default;

	/// Default copy constructor
	AudioEffectProperties(const AudioEffectProperties &) = default;
	/// DEfault assignment operator
	AudioEffectProperties &operator=(const AudioEffectProperties &) = default;

	/// Returns the effect properties type
	inline AudioEffect::Type type() const { return type_; }
	/// Sets the effect properties type
	void setType(AudioEffect::Type type);

	/// Returns the minimum values for the reverb effect properties
	static ReverbProperties minReverbProperties();
	/// Returns the maximum values for the reverb effect properties
	static ReverbProperties maxReverbProperties();

	/// Returns the current constant values for the reverb effect properties
	const ReverbProperties &reverbProperties() const;
	/// Returns the current values for the reverb effect properties
	ReverbProperties &reverbProperties();
	/// Sets all values for the reverb effect properties
	void setReverbProperties(const ReverbProperties &newReverbProperties);
	/// Resets all property values for the reverb effect to thir default
	void resetReverbProperties();

	/// Returns the minimum values for the EAX reverb effect properties
	static EaxReverbProperties minEaxReverbProperties();
	/// Returns the maximum values for the EAX reverb effect properties
	static EaxReverbProperties maxEaxReverbProperties();

	/// Returns the current constant values for the EAX reverb effect properties
	const EaxReverbProperties &eaxReverbProperties() const;
	/// Returns the current values for the EAX reverb effect properties
	EaxReverbProperties &eaxReverbProperties();
	/// Sets all values for the EAX reverb effect properties
	void setEaxReverbProperties(const EaxReverbProperties &newEaxReverbProperties);
	/// Resets all property values for the EAX reverb effect to thir default
	void resetEaxReverbProperties();

	/// Returns the minimum values for the chorus effect properties
	static ChorusProperties minChorusProperties();
	/// Returns the maximum values for the chorus effect properties
	static ChorusProperties maxChorusProperties();

	/// Returns the current constant values for the chorus effect properties
	const ChorusProperties &chorusProperties() const;
	/// Returns the current values for the chorus effect properties
	ChorusProperties &chorusProperties();
	/// Sets all values for the chorus effect properties
	void setChorusProperties(const ChorusProperties &newChorusProperties);
	/// Resets all property values for the chorus effect to thir default
	void resetChorusProperties();

	/// Returns the minimum values for the distortion effect properties
	static DistortionProperties minDistortionProperties();
	/// Returns the maximum values for the distortion effect properties
	static DistortionProperties maxDistortionProperties();

	/// Returns the current constant values for the distortion effect properties
	const DistortionProperties &distortionProperties() const;
	/// Returns the current values for the distortion effect properties
	DistortionProperties &distortionProperties();
	/// Sets all values for the distortion effect properties
	void setDistortionProperties(const DistortionProperties &newDistortionProperties);
	/// Resets all property values for the distortion effect to thir default
	void resetDistortionProperties();

	/// Returns the minimum values for the echo effect properties
	static EchoProperties minEchoProperties();
	/// Returns the maximum values for the echo effect properties
	static EchoProperties maxEchoProperties();

	/// Returns the current constant values for the echo effect properties
	const EchoProperties &echoProperties() const;
	/// Returns the current values for the echo effect properties
	EchoProperties &echoProperties();
	/// Sets all values for the echo effect properties
	void setEchoProperties(const EchoProperties &newEchoProperties);
	/// Resets all property values for the echo effect to thir default
	void resetEchoProperties();

	/// Returns the minimum values for the flanger effect properties
	static FlangerProperties minFlangerProperties();
	/// Returns the maximum values for the flanger effect properties
	static FlangerProperties maxFlangerProperties();

	/// Returns the current constant values for the flanger effect properties
	const FlangerProperties &flangerProperties() const;
	/// Returns the current values for the flanger effect properties
	FlangerProperties &flangerProperties();
	/// Sets all values for the flanger effect properties
	void setFlangerProperties(const FlangerProperties &newFlangerProperties);
	/// Resets all property values for the flanger effect to thir default
	void resetFlangerProperties();

	/// Returns the minimum values for the frequency shifter effect properties
	static FrequencyShifterProperties minFrequencyShifterProperties();
	/// Returns the maximum values for the frequency shifter effect properties
	static FrequencyShifterProperties maxFrequencyShifterProperties();

	/// Returns the current constant values for the frequency shifter effect properties
	const FrequencyShifterProperties &frequencyShifterProperties() const;
	/// Returns the current values for the frequency shifter effect properties
	FrequencyShifterProperties &frequencyShifterProperties();
	/// Sets all values for the frequency shifter effect properties
	void setFrequencyShifterProperties(const FrequencyShifterProperties &newFrequencyShifterProperties);
	/// Resets all property values for the frequency shifter effect to thir default
	void resetFrequencyShifterProperties();

	/// Returns the minimum values for the vocal morpher effect properties
	static VocalMorpherProperties minVocalMorpherProperties();
	/// Returns the maximum values for the vocal morpher effect properties
	static VocalMorpherProperties maxVocalMorpherProperties();

	/// Returns the current constant values for the vocal morpher effect properties
	const VocalMorpherProperties &vocalMorpherProperties() const;
	/// Returns the current values for the vocal morpher effect properties
	VocalMorpherProperties &vocalMorpherProperties();
	/// Sets all values for the vocal morpher effect properties
	void setVocalMorpherProperties(const VocalMorpherProperties &newVocalMorpherProperties);
	/// Resets all property values for the vocal morpher effect to thir default
	void resetVocalMorpherProperties();

	/// Returns the minimum values for the pitch shifter effect properties
	static PitchShifterProperties minPitchShifterProperties();
	/// Returns the maximum values for the pitch shifter effect properties
	static PitchShifterProperties maxPitchShifterProperties();

	/// Returns the current constant values for the pitch shifter effect properties
	const PitchShifterProperties &pitchShifterProperties() const;
	/// Returns the current values for the pitch shifter effect properties
	PitchShifterProperties &pitchShifterProperties();
	/// Sets all values for the pitch shifter effect properties
	void setPitchShifterProperties(const PitchShifterProperties &newPitchShifterProperties);
	/// Resets all property values for the pitch shifter effect to thir default
	void resetPitchShifterProperties();

	/// Returns the minimum values for the ring modulator effect properties
	static RingModulatorProperties minRingModulatorProperties();
	/// Returns the maximum values for the ring modulator effect properties
	static RingModulatorProperties maxRingModulatorProperties();

	/// Returns the current constant values for the ring modulator effect properties
	const RingModulatorProperties &ringModulatorProperties() const;
	/// Returns the current values for the ring modulator effect properties
	RingModulatorProperties &ringModulatorProperties();
	/// Sets all values for the ring modulator effect properties
	void setRingModulatorProperties(const RingModulatorProperties &newRingModulatorProperties);
	/// Resets all property values for the ring modulator effect to thir default
	void resetRingModulatorProperties();

	/// Returns the minimum values for the auto wah effect properties
	static AutoWahProperties minAutoWahProperties();
	/// Returns the maximum values for the auto wah effect properties
	static AutoWahProperties maxAutoWahProperties();

	/// Returns the current constant values for the auto wah effect properties
	const AutoWahProperties &autoWahProperties() const;
	/// Returns the current values for the auto wah effect properties
	AutoWahProperties &autoWahProperties();
	/// Sets all values for the auto wah effect properties
	void setAutoWahProperties(const AutoWahProperties &newAutoWahProperties);
	/// Resets all property values for the auto wah effect to thir default
	void resetAutoWahProperties();

	/// Returns the minimum values for the compressor effect properties
	static CompressorProperties minCompressorProperties();
	/// Returns the maximum values for the compressor effect properties
	static CompressorProperties maxCompressorProperties();

	/// Returns the current constant values for the compressor effect properties
	const CompressorProperties &compressorProperties() const;
	/// Returns the current values for the compressor effect properties
	CompressorProperties &compressorProperties();
	/// Sets all values for the compressor effect properties
	void setCompressorProperties(const CompressorProperties &newCompressorProperties);
	/// Resets all property values for the compressor effect to thir default
	void resetCompressorProperties();

	/// Returns the minimum values for the equalizer effect properties
	static EqualizerProperties minEqualizerProperties();
	/// Returns the maximum values for the equalizer effect properties
	static EqualizerProperties maxEqualizerProperties();

	/// Returns the current constant values for the equalizer effect properties
	const EqualizerProperties &equalizerProperties() const;
	/// Returns the current values for the equalizer effect properties
	EqualizerProperties &equalizerProperties();
	/// Sets all values for the equalizer effect properties
	void setEqualizerProperties(const EqualizerProperties &newEqualizerProperties);
	/// Resets all property values for the equalizer effect to thir default
	void resetEqualizerProperties();

  private:
	AudioEffect::Type type_;
	/*! \note Biggest properties set is `EFXEAXREVERBPROPERTIES`, with 26 floats and 1 int */
	static const unsigned int PropertiesDataSize = 26 * sizeof(float) + sizeof(int);
	/// Memory buffer containing the data of a set of effect properties
	char propertiesData_[PropertiesDataSize];

	bool checkType(AudioEffect::Type type) const;
};

}

#endif
