#ifndef CLASS_NCINE_AUDIOEFFECTSLOT
#define CLASS_NCINE_AUDIOEFFECTSLOT

#include "common_macros.h"

namespace ncine {

class AudioEffect;

/// A class representing an OpenAL effect
class DLL_PUBLIC AudioEffectSlot
{
  public:
	/// Creates an OpenAL effect slot name
	AudioEffectSlot();
	explicit AudioEffectSlot(const AudioEffect &effect);
	~AudioEffectSlot();

	/// Move constructor
	AudioEffectSlot(AudioEffectSlot &&other);
	/// Move assignment operator
	AudioEffectSlot &operator=(AudioEffectSlot &&other);

	/// Returns the OpenAL effect slot id
	inline unsigned int effectSlotId() const { return effectSlotId_; }

	inline float gain() const { return gain_; }
	void setGain(float gain);

	inline bool auxSendAuto() const { return auxSendAuto_; }
	void setAuxSendAuto(bool auxSendAuto);

	/// Applies the effect parameters from an effect object
	void applyEffect(const AudioEffect &effect);

  private:
	/// The OpenAL effect slot id
	unsigned int effectSlotId_;
	/// Effect slot gain value
	float gain_;
	/// Effect slot auxiliary send auto flag
	bool auxSendAuto_;

	/// Deleted copy constructor
	AudioEffectSlot(const AudioEffectSlot &) = delete;
	/// Deleted assignment operator
	AudioEffectSlot &operator=(const AudioEffectSlot &) = delete;
};

}

#endif
