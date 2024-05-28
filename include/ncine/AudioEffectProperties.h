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

	inline AudioEffect::Type type() const { return type_; }
	void setType(AudioEffect::Type type);

	static ReverbProperties minReverbProperties();
	static ReverbProperties maxReverbProperties();

	const ReverbProperties &reverbProperties() const;
	ReverbProperties &reverbProperties();
	void setReverbProperties(const ReverbProperties &newReverbProperties);
	void resetReverbProperties();

	static EaxReverbProperties minEaxReverbProperties();
	static EaxReverbProperties maxEaxReverbProperties();

	const EaxReverbProperties &eaxReverbProperties() const;
	EaxReverbProperties &eaxReverbProperties();
	void setEaxReverbProperties(const EaxReverbProperties &newEaxReverbProperties);
	void resetEaxReverbProperties();

	static ChorusProperties minChorusProperties();
	static ChorusProperties maxChorusProperties();

	const ChorusProperties &chorusProperties() const;
	ChorusProperties &chorusProperties();
	void setChorusProperties(const ChorusProperties &newChorusProperties);
	void resetChorusProperties();

	static DistortionProperties minDistortionProperties();
	static DistortionProperties maxDistortionProperties();

	const DistortionProperties &distortionProperties() const;
	DistortionProperties &distortionProperties();
	void setDistortionProperties(const DistortionProperties &newDistortionProperties);
	void resetDistortionProperties();

	static EchoProperties minEchoProperties();
	static EchoProperties maxEchoProperties();

	const EchoProperties &echoProperties() const;
	EchoProperties &echoProperties();
	void setEchoProperties(const EchoProperties &newEchoProperties);
	void resetEchoProperties();

	static FlangerProperties minFlangerProperties();
	static FlangerProperties maxFlangerProperties();

	const FlangerProperties &flangerProperties() const;
	FlangerProperties &flangerProperties();
	void setFlangerProperties(const FlangerProperties &newFlangerProperties);
	void resetFlangerProperties();

	static FrequencyShifterProperties minFrequencyShifterProperties();
	static FrequencyShifterProperties maxFrequencyShifterProperties();

	const FrequencyShifterProperties &frequencyShifterProperties() const;
	FrequencyShifterProperties &frequencyShifterProperties();
	void setFrequencyShifterProperties(const FrequencyShifterProperties &newFrequencyShifterProperties);
	void resetFrequencyShifterProperties();

	static VocalMorpherProperties minVocalMorpherProperties();
	static VocalMorpherProperties maxVocalMorpherProperties();

	const VocalMorpherProperties &vocalMorpherProperties() const;
	VocalMorpherProperties &vocalMorpherProperties();
	void setVocalMorpherProperties(const VocalMorpherProperties &newVocalMorpherProperties);
	void resetVocalMorpherProperties();

	static PitchShifterProperties minPitchShifterProperties();
	static PitchShifterProperties maxPitchShifterProperties();

	const PitchShifterProperties &pitchShifterProperties() const;
	PitchShifterProperties &pitchShifterProperties();
	void setPitchShifterProperties(const PitchShifterProperties &newPitchShifterProperties);
	void resetPitchShifterProperties();

	static RingModulatorProperties minRingModulatorProperties();
	static RingModulatorProperties maxRingModulatorProperties();

	const RingModulatorProperties &ringModulatorProperties() const;
	RingModulatorProperties &ringModulatorProperties();
	void setRingModulatorProperties(const RingModulatorProperties &newRingModulatorProperties);
	void resetRingModulatorProperties();

	static AutoWahProperties minAutoWahProperties();
	static AutoWahProperties maxAutoWahProperties();

	const AutoWahProperties &autoWahProperties() const;
	AutoWahProperties &autoWahProperties();
	void setAutoWahProperties(const AutoWahProperties &newAutoWahProperties);
	void resetAutoWahProperties();

	static CompressorProperties minCompressorProperties();
	static CompressorProperties maxCompressorProperties();

	const CompressorProperties &compressorProperties() const;
	CompressorProperties &compressorProperties();
	void setCompressorProperties(const CompressorProperties &newCompressorProperties);
	void resetCompressorProperties();

	static EqualizerProperties minEqualizerProperties();
	static EqualizerProperties maxEqualizerProperties();

	const EqualizerProperties &equalizerProperties() const;
	EqualizerProperties &equalizerProperties();
	void setEqualizerProperties(const EqualizerProperties &newEqualizerProperties);
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
