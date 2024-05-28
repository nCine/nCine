#define NCINE_INCLUDE_OPENAL_EFX
#include "common_headers.h"

#include <nctl/algorithms.h>
#include "common_macros.h"
#include "IAudioDevice.h"
#include "AudioFilter.h"

namespace ncine {

namespace {

	ALint typeToEfxType(AudioFilter::Type type)
	{
		switch (type)
		{
			case AudioFilter::Type::NULL_FILTER: return AL_FILTER_NULL;
			case AudioFilter::Type::LOWPASS: return AL_FILTER_LOWPASS;
			case AudioFilter::Type::HIGHPASS: return AL_FILTER_HIGHPASS;
			case AudioFilter::Type::BANDPASS: return AL_FILTER_BANDPASS;
		}
		return AL_EFFECT_NULL;
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioFilter::AudioFilter()
    : type_(Type::NULL_FILTER), filterId_(0)
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
	{
		alGetError();
		alGenFilters(1, &filterId_);
		const ALenum error = alGetError();
		FATAL_ASSERT_MSG_X(error == AL_NO_ERROR, "alGenFilters failed: 0x%x", error);

		ASSERT(alIsFilter(filterId_) == AL_TRUE);
	}
}

AudioFilter::AudioFilter(Type type)
    : AudioFilter()
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
	{
		alEffecti(filterId_, AL_FILTER_TYPE, typeToEfxType(type));
		type_ = type;
	}
}

AudioFilter::~AudioFilter()
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
		alDeleteFilters(1, &filterId_);
}

AudioFilter::AudioFilter(AudioFilter &&other)
    : filterId_(other.filterId_)
{
	other.filterId_ = AL_FILTER_NULL;
}

AudioFilter &AudioFilter::operator=(AudioFilter &&other)
{
	filterId_ = other.filterId_;

	other.filterId_ = AL_FILTER_NULL;
	return *this;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AudioFilter::applyProperties(const Properties &properties)
{
	IAudioDevice &device = theServiceLocator().audioDevice();
	if (device.hasExtension(IAudioDevice::ALExtensions::EXT_EFX) == false)
		return;

	Properties props = properties;
	props.gain = nctl::clamp(props.gain, 0.0f, 1.0f);
	props.gainLF = nctl::clamp(props.gainLF, 0.0f, 1.0f);
	props.gainHF = nctl::clamp(props.gainHF, 0.0f, 1.0f);

	setType(properties.type);

	switch (type_)
	{
		case AudioFilter::Type::NULL_FILTER:
			break;
		case AudioFilter::Type::LOWPASS:
		{
			alFilterf(filterId_, AL_LOWPASS_GAIN, props.gain);
			alFilterf(filterId_, AL_LOWPASS_GAINHF, props.gainHF);
			break;
		}
		case AudioFilter::Type::HIGHPASS:
		{
			alFilterf(filterId_, AL_HIGHPASS_GAIN, props.gain);
			alFilterf(filterId_, AL_HIGHPASS_GAINLF, props.gainLF);
			break;
		}
		case AudioFilter::Type::BANDPASS:
		{
			alFilterf(filterId_, AL_BANDPASS_GAIN, props.gain);
			alFilterf(filterId_, AL_BANDPASS_GAINLF, props.gainLF);
			alFilterf(filterId_, AL_BANDPASS_GAINHF, props.gainHF);
			break;
		}
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AudioFilter::setType(Type type)
{
	if (type_ != type)
	{
		alFilteri(filterId_, AL_FILTER_TYPE, typeToEfxType(type));
		type_ = type;
	}
}

}
