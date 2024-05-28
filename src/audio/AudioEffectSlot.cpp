#define NCINE_INCLUDE_OPENAL_EFX
#include "common_headers.h"

#include "IAudioDevice.h"
#include "AudioEffectSlot.h"
#include "AudioEffect.h"
#include "AudioFilter.h"

namespace ncine {

namespace {

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioEffectSlot::AudioEffectSlot()
    : effectSlotId_(AL_EFFECTSLOT_NULL), gain_(1.0f), auxSendAuto_(false)
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
	{
		alGetError();
		alGenAuxiliaryEffectSlots(1, &effectSlotId_);
		const ALenum error = alGetError();
		FATAL_ASSERT_MSG_X(error == AL_NO_ERROR, "alGenAuxiliaryEffectSlots failed: 0x%x", error);

		ASSERT(alIsAuxiliaryEffectSlot(effectSlotId_) == AL_TRUE);

		alAuxiliaryEffectSloti(effectSlotId_, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, auxSendAuto_ ? AL_TRUE : AL_FALSE);
	}
}

AudioEffectSlot::AudioEffectSlot(const AudioEffect &effect)
    : AudioEffectSlot()
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
		applyEffect(effect);
}

AudioEffectSlot::~AudioEffectSlot()
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
	{
		alAuxiliaryEffectSloti(effectSlotId_, AL_EFFECTSLOT_EFFECT, AL_EFFECTSLOT_NULL);
		alDeleteAuxiliaryEffectSlots(1, &effectSlotId_);
	}
}

AudioEffectSlot::AudioEffectSlot(AudioEffectSlot &&other)
    : effectSlotId_(other.effectSlotId_), gain_(other.gain_),
      auxSendAuto_(other.auxSendAuto_)
{
	other.effectSlotId_ = AL_EFFECTSLOT_NULL;
}

AudioEffectSlot &AudioEffectSlot::operator=(AudioEffectSlot &&other)
{
	effectSlotId_ = other.effectSlotId_;
	gain_ = other.gain_;
	auxSendAuto_ = other.auxSendAuto_;

	other.effectSlotId_ = AL_EFFECTSLOT_NULL;
	return *this;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AudioEffectSlot::setGain(float gain)
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
	{
		gain_ = gain;
		alAuxiliaryEffectSlotf(effectSlotId_, AL_EFFECTSLOT_GAIN, gain_);
	}
}

void AudioEffectSlot::setAuxSendAuto(bool auxSendAuto)
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX) && auxSendAuto_ != auxSendAuto)
	{
		auxSendAuto_ = auxSendAuto;
		alAuxiliaryEffectSloti(effectSlotId_, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, auxSendAuto_ ? AL_TRUE : AL_FALSE);
	}
}

/*! \note You can modify or delete the effect object afterward without affecting the effect slot */
void AudioEffectSlot::applyEffect(const AudioEffect &effect)
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
	{
		alGetError();
		alAuxiliaryEffectSloti(effectSlotId_, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect.effectId()));
		const ALenum error = alGetError();
		ASSERT_MSG_X(error == AL_NO_ERROR, "alAuxiliaryEffectSloti failed: 0x%x", error);
	}
}

}
