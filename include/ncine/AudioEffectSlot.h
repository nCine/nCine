#ifndef CLASS_NCINE_AUDIOEFFECTSLOT
#define CLASS_NCINE_AUDIOEFFECTSLOT

#include "common_macros.h"

namespace ncine {

class AudioEffect;

/// A class representing an OpenAL auxilary effect slot
class DLL_PUBLIC AudioEffectSlot
{
  public:
	/// Creates an OpenAL auxilary effect slot name
	AudioEffectSlot();
	explicit AudioEffectSlot(const AudioEffect &effect);
	~AudioEffectSlot();

	/// Move constructor
	AudioEffectSlot(AudioEffectSlot &&other);
	/// Move assignment operator
	AudioEffectSlot &operator=(AudioEffectSlot &&other);

	/// Returns the OpenAL auxilary effect slot id
	inline unsigned int effectSlotId() const { return effectSlotId_; }

	/// Returns the gain output level of the auxilary effect slot
	inline float gain() const { return gain_; }
	/// Sets the gain output level of the auxilary effect slot
	void setGain(float gain);

	/// Returns the state of the auxilary slot send auto flag
	/*! \note The property is used to enable or disable automatic send
	 *  adjustments based on the physical positions of the sources and the listener. */
	inline bool auxSendAuto() const { return auxSendAuto_; }
	/// Sets the state of the auxilary slot send auto flag
	/*! \note The property is used to enable or disable automatic send
	 *  adjustments based on the physical positions of the sources and the listener. */
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
