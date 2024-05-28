#ifndef CLASS_NCINE_AUDIOEFFECT
#define CLASS_NCINE_AUDIOEFFECT

#include "common_defines.h"

namespace ncine {

class AudioEffectProperties;

/// A class representing an OpenAL effect
class DLL_PUBLIC AudioEffect
{
  public:
	enum class Type
	{
		NULL_EFFECT,
		REVERB,
		EAXREVERB,
		CHORUS,
		DISTORTION,
		ECHO,
		FLANGER,
		FREQUENCY_SHIFTER,
		VOCAL_MORPHER,
		PITCH_SHIFTER,
		RING_MODULATOR,
		AUTOWAH,
		COMPRESSOR,
		EQUALIZER
	};

	/// Creates an OpenAL effect name
	AudioEffect();
	/// Creates an OpenAL effect name with the specified type
	explicit AudioEffect(Type type);
	~AudioEffect();

	/// Move constructor
	AudioEffect(AudioEffect &&other);
	/// Move assignment operator
	AudioEffect &operator=(AudioEffect &&other);

	/// Returns the of the effect
	inline Type type() const { return type_; }

	/// Returns the OpenAL effect id
	inline unsigned int effectId() const { return effectId_; }

	void applyProperties(const AudioEffectProperties &properties);

  private:
	/// The effect type
	Type type_;

	/// The OpenAL effect id
	unsigned int effectId_;

	/// Deleted copy constructor
	AudioEffect(const AudioEffect &) = delete;
	/// Deleted assignment operator
	AudioEffect &operator=(const AudioEffect &) = delete;

	/// Sets the type of the effect
	void setType(Type type);
};

}

#endif
