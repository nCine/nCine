#define NCINE_INCLUDE_OPENAL
#define NCINE_INCLUDE_OPENAL_EFX_PRESETS
#include "common_headers.h"

#include "common_macros.h"
#include "IAudioDevice.h"
#include "AudioEffectProperties.h"

namespace ncine {

namespace {

	ALint typeToEfxType(AudioEffect::Type type)
	{
		switch (type)
		{
			case AudioEffect::Type::NULL_EFFECT: return AL_EFFECT_NULL;
			case AudioEffect::Type::REVERB: return AL_EFFECT_REVERB;
			case AudioEffect::Type::EAXREVERB: return AL_EFFECT_EAXREVERB;
			case AudioEffect::Type::CHORUS: return AL_EFFECT_CHORUS;
			case AudioEffect::Type::DISTORTION: return AL_EFFECT_DISTORTION;
			case AudioEffect::Type::ECHO: return AL_EFFECT_ECHO;
			case AudioEffect::Type::FLANGER: return AL_EFFECT_FLANGER;
			case AudioEffect::Type::FREQUENCY_SHIFTER: return AL_EFFECT_FREQUENCY_SHIFTER;
			case AudioEffect::Type::VOCAL_MORPHER: return AL_EFFECT_VOCAL_MORPHER;
			case AudioEffect::Type::PITCH_SHIFTER: return AL_EFFECT_PITCH_SHIFTER;
			case AudioEffect::Type::RING_MODULATOR: return AL_EFFECT_RING_MODULATOR;
			case AudioEffect::Type::AUTOWAH: return AL_EFFECT_AUTOWAH;
			case AudioEffect::Type::COMPRESSOR: return AL_EFFECT_COMPRESSOR;
			case AudioEffect::Type::EQUALIZER: return AL_EFFECT_EQUALIZER;
		}
		return AL_EFFECT_NULL;
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioEffect::AudioEffect()
    : type_(Type::NULL_EFFECT), effectId_(0)
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
	{
		alGetError();
		alGenEffects(1, &effectId_);
		const ALenum error = alGetError();
		FATAL_ASSERT_MSG_X(error == AL_NO_ERROR, "alGenEffects failed: 0x%x", error);

		ASSERT(alIsEffect(effectId_) == AL_TRUE);
	}
}

AudioEffect::AudioEffect(Type type)
    : AudioEffect()
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
	{
		alEffecti(effectId_, AL_EFFECT_TYPE, typeToEfxType(type));
		type_ = type;
	}
}

AudioEffect::~AudioEffect()
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
		alDeleteEffects(1, &effectId_);
}

AudioEffect::AudioEffect(AudioEffect &&other)
    : effectId_(other.effectId_)
{
	other.effectId_ = AL_EFFECT_NULL;
}

AudioEffect &AudioEffect::operator=(AudioEffect &&other)
{
	effectId_ = other.effectId_;

	other.effectId_ = AL_EFFECT_NULL;
	return *this;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AudioEffect::applyProperties(const AudioEffectProperties &properties)
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX) == false)
		return;

	setType(properties.type());

	switch (type_)
	{
		case AudioEffect::Type::NULL_EFFECT:
			break;
		case AudioEffect::Type::REVERB:
		{
			const AudioEffectProperties::ReverbProperties reverbProperties = properties.reverbProperties();
			alEffectf(effectId_, AL_REVERB_DENSITY, reverbProperties.density);
			alEffectf(effectId_, AL_REVERB_DIFFUSION, reverbProperties.diffusion);
			alEffectf(effectId_, AL_REVERB_GAIN, reverbProperties.gain);
			alEffectf(effectId_, AL_REVERB_GAINHF, reverbProperties.gainHF);
			alEffectf(effectId_, AL_REVERB_DECAY_TIME, reverbProperties.decayTime);
			alEffectf(effectId_, AL_REVERB_DECAY_HFRATIO, reverbProperties.decayHFRatio);
			alEffectf(effectId_, AL_REVERB_REFLECTIONS_GAIN, reverbProperties.reflectionsGain);
			alEffectf(effectId_, AL_REVERB_REFLECTIONS_DELAY, reverbProperties.reflectionsDelay);
			alEffectf(effectId_, AL_REVERB_LATE_REVERB_GAIN, reverbProperties.lateReverbGain);
			alEffectf(effectId_, AL_REVERB_LATE_REVERB_DELAY, reverbProperties.lateReverbDelay);
			alEffectf(effectId_, AL_REVERB_AIR_ABSORPTION_GAINHF, reverbProperties.airAbsorptionGainHF);
			alEffectf(effectId_, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverbProperties.roomRolloffFactor);
			alEffecti(effectId_, AL_REVERB_DECAY_HFLIMIT, reverbProperties.decayHFLimit);
			break;
		}
		case AudioEffect::Type::EAXREVERB:
		{
			const AudioEffectProperties::EaxReverbProperties eaxReverbProperties = properties.eaxReverbProperties();
			alEffectf(effectId_, AL_EAXREVERB_DENSITY, eaxReverbProperties.density);
			alEffectf(effectId_, AL_EAXREVERB_DIFFUSION, eaxReverbProperties.diffusion);
			alEffectf(effectId_, AL_EAXREVERB_GAIN, eaxReverbProperties.gain);
			alEffectf(effectId_, AL_EAXREVERB_GAINHF, eaxReverbProperties.gainHF);
			alEffectf(effectId_, AL_EAXREVERB_GAINLF, eaxReverbProperties.gainLF);
			alEffectf(effectId_, AL_EAXREVERB_DECAY_TIME, eaxReverbProperties.decayTime);
			alEffectf(effectId_, AL_EAXREVERB_DECAY_HFRATIO, eaxReverbProperties.decayHFRatio);
			alEffectf(effectId_, AL_EAXREVERB_DECAY_LFRATIO, eaxReverbProperties.decayLFRatio);
			alEffectf(effectId_, AL_EAXREVERB_REFLECTIONS_GAIN, eaxReverbProperties.reflectionsGain);
			alEffectf(effectId_, AL_EAXREVERB_REFLECTIONS_DELAY, eaxReverbProperties.reflectionsDelay);
			alEffectfv(effectId_, AL_EAXREVERB_REFLECTIONS_PAN, eaxReverbProperties.reflectionsPan);
			alEffectf(effectId_, AL_EAXREVERB_LATE_REVERB_GAIN, eaxReverbProperties.lateReverbGain);
			alEffectf(effectId_, AL_EAXREVERB_LATE_REVERB_DELAY, eaxReverbProperties.lateReverbDelay);
			alEffectfv(effectId_, AL_EAXREVERB_LATE_REVERB_PAN, eaxReverbProperties.lateReverbPan);
			alEffectf(effectId_, AL_EAXREVERB_ECHO_TIME, eaxReverbProperties.echoTime);
			alEffectf(effectId_, AL_EAXREVERB_ECHO_DEPTH, eaxReverbProperties.echoDepth);
			alEffectf(effectId_, AL_EAXREVERB_MODULATION_TIME, eaxReverbProperties.modulationTime);
			alEffectf(effectId_, AL_EAXREVERB_MODULATION_DEPTH, eaxReverbProperties.modulationDepth);
			alEffectf(effectId_, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, eaxReverbProperties.airAbsorptionGainHF);
			alEffectf(effectId_, AL_EAXREVERB_HFREFERENCE, eaxReverbProperties.hfReference);
			alEffectf(effectId_, AL_EAXREVERB_LFREFERENCE, eaxReverbProperties.lfReference);
			alEffectf(effectId_, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, eaxReverbProperties.roomRolloffFactor);
			alEffecti(effectId_, AL_EAXREVERB_DECAY_HFLIMIT, eaxReverbProperties.decayHFLimit);
			break;
		}
		case AudioEffect::Type::CHORUS:
		{
			const AudioEffectProperties::ChorusProperties chorusProperties = properties.chorusProperties();
			alEffecti(effectId_, AL_CHORUS_WAVEFORM, chorusProperties.waveform);
			alEffecti(effectId_, AL_CHORUS_PHASE, chorusProperties.phase);
			alEffectf(effectId_, AL_CHORUS_RATE, chorusProperties.rate);
			alEffectf(effectId_, AL_CHORUS_DEPTH, chorusProperties.depth);
			alEffectf(effectId_, AL_CHORUS_FEEDBACK, chorusProperties.feedback);
			alEffectf(effectId_, AL_CHORUS_DELAY, chorusProperties.delay);
			break;
		}
		case AudioEffect::Type::DISTORTION:
		{
			const AudioEffectProperties::DistortionProperties distorionProperties = properties.distortionProperties();
			alEffectf(effectId_, AL_DISTORTION_EDGE, distorionProperties.edge);
			alEffectf(effectId_, AL_DISTORTION_GAIN, distorionProperties.gain);
			alEffectf(effectId_, AL_DISTORTION_LOWPASS_CUTOFF, distorionProperties.lowpassCutoff);
			alEffectf(effectId_, AL_DISTORTION_EQCENTER, distorionProperties.eqCenter);
			alEffectf(effectId_, AL_DISTORTION_EQBANDWIDTH, distorionProperties.eqBandwidth);
			break;
		}
		case AudioEffect::Type::ECHO:
		{
			const AudioEffectProperties::EchoProperties echoProperties = properties.echoProperties();
			alEffectf(effectId_, AL_ECHO_DELAY, echoProperties.delay);
			alEffectf(effectId_, AL_ECHO_LRDELAY, echoProperties.lrDelay);
			alEffectf(effectId_, AL_ECHO_DAMPING, echoProperties.damping);
			alEffectf(effectId_, AL_ECHO_FEEDBACK, echoProperties.feedback);
			alEffectf(effectId_, AL_ECHO_SPREAD, echoProperties.spread);
			break;
		}
		case AudioEffect::Type::FLANGER:
		{
			const AudioEffectProperties::FlangerProperties flangerProperties = properties.flangerProperties();
			alEffecti(effectId_, AL_FLANGER_WAVEFORM, flangerProperties.waveform);
			alEffecti(effectId_, AL_FLANGER_PHASE, flangerProperties.phase);
			alEffectf(effectId_, AL_FLANGER_RATE, flangerProperties.rate);
			alEffectf(effectId_, AL_FLANGER_DEPTH, flangerProperties.depth);
			alEffectf(effectId_, AL_FLANGER_FEEDBACK, flangerProperties.feedback);
			alEffectf(effectId_, AL_FLANGER_DELAY, flangerProperties.delay);
			break;
		}
		case AudioEffect::Type::FREQUENCY_SHIFTER:
		{
			const AudioEffectProperties::FrequencyShifterProperties frequencyShifterProperties = properties.frequencyShifterProperties();
			alEffectf(effectId_, AL_FREQUENCY_SHIFTER_FREQUENCY, frequencyShifterProperties.frequency);
			alEffecti(effectId_, AL_FREQUENCY_SHIFTER_LEFT_DIRECTION, frequencyShifterProperties.leftDirection);
			alEffecti(effectId_, AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION, frequencyShifterProperties.rightDirection);
			break;
		}
		case AudioEffect::Type::VOCAL_MORPHER:
		{
			const AudioEffectProperties::VocalMorpherProperties vocalMorpherProperties = properties.vocalMorpherProperties();
			alEffecti(effectId_, AL_VOCAL_MORPHER_PHONEMEA, vocalMorpherProperties.phonemeA);
			alEffecti(effectId_, AL_VOCAL_MORPHER_PHONEMEA_COARSE_TUNING, vocalMorpherProperties.phonemeACoarseTuning);
			alEffecti(effectId_, AL_VOCAL_MORPHER_PHONEMEB, vocalMorpherProperties.phonemeB);
			alEffecti(effectId_, AL_VOCAL_MORPHER_PHONEMEB_COARSE_TUNING, vocalMorpherProperties.phonemeBCoarseTuning);
			alEffecti(effectId_, AL_VOCAL_MORPHER_WAVEFORM, vocalMorpherProperties.waveform);
			alEffectf(effectId_, AL_VOCAL_MORPHER_RATE, vocalMorpherProperties.rate);
			break;
		}
		case AudioEffect::Type::PITCH_SHIFTER:
		{
			const AudioEffectProperties::PitchShifterProperties pitchShifterProperties = properties.pitchShifterProperties();
			alEffecti(effectId_, AL_PITCH_SHIFTER_COARSE_TUNE, pitchShifterProperties.coarseTune);
			alEffecti(effectId_, AL_PITCH_SHIFTER_FINE_TUNE, pitchShifterProperties.fineTune);
			break;
		}
		case AudioEffect::Type::RING_MODULATOR:
		{
			const AudioEffectProperties::RingModulatorProperties ringModulatorProperties = properties.ringModulatorProperties();
			alEffectf(effectId_, AL_RING_MODULATOR_FREQUENCY, ringModulatorProperties.frequency);
			alEffectf(effectId_, AL_RING_MODULATOR_HIGHPASS_CUTOFF, ringModulatorProperties.highpassCutoff);
			alEffecti(effectId_, AL_RING_MODULATOR_WAVEFORM, ringModulatorProperties.waveform);
			break;
		}
		case AudioEffect::Type::AUTOWAH:
		{
			const AudioEffectProperties::AutoWahProperties autoWahProperties = properties.autoWahProperties();
			alEffectf(effectId_, AL_AUTOWAH_ATTACK_TIME, autoWahProperties.attackTime);
			alEffectf(effectId_, AL_AUTOWAH_RELEASE_TIME, autoWahProperties.releaseTime);
			alEffectf(effectId_, AL_AUTOWAH_RESONANCE, autoWahProperties.resonance);
			alEffectf(effectId_, AL_AUTOWAH_PEAK_GAIN, autoWahProperties.peakGain);
			break;
		}
		case AudioEffect::Type::COMPRESSOR:
		{
			const AudioEffectProperties::CompressorProperties compressorProperties = properties.compressorProperties();
			alEffecti(effectId_, AL_COMPRESSOR_ONOFF, compressorProperties.onOff);
			break;
		}
		case AudioEffect::Type::EQUALIZER:
		{
			const AudioEffectProperties::EqualizerProperties equalizerProperties = properties.equalizerProperties();
			alEffectf(effectId_, AL_EQUALIZER_LOW_GAIN, equalizerProperties.lowGain);
			alEffectf(effectId_, AL_EQUALIZER_LOW_CUTOFF, equalizerProperties.lowCutoff);
			alEffectf(effectId_, AL_EQUALIZER_MID1_GAIN, equalizerProperties.mid1Gain);
			alEffectf(effectId_, AL_EQUALIZER_MID1_CENTER, equalizerProperties.mid1Center);
			alEffectf(effectId_, AL_EQUALIZER_MID1_WIDTH, equalizerProperties.mid1Width);
			alEffectf(effectId_, AL_EQUALIZER_MID2_GAIN, equalizerProperties.mid2Gain);
			alEffectf(effectId_, AL_EQUALIZER_MID2_CENTER, equalizerProperties.mid2Center);
			alEffectf(effectId_, AL_EQUALIZER_MID2_WIDTH, equalizerProperties.mid2Width);
			alEffectf(effectId_, AL_EQUALIZER_HIGH_GAIN, equalizerProperties.highGain);
			alEffectf(effectId_, AL_EQUALIZER_HIGH_CUTOFF, equalizerProperties.highCutoff);
			break;
		}
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AudioEffect::setType(Type type)
{
	if (type_ != type)
	{
		alEffecti(effectId_, AL_EFFECT_TYPE, typeToEfxType(type));
		type_ = type;
	}
}

}
