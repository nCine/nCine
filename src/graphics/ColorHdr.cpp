#include "ColorHdr.h"
#include "Colorf.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const ColorHdr ColorHdr::Black(0.0f, 0.0f, 0.0f);
const ColorHdr ColorHdr::White(1.0f, 1.0f, 1.0f);
const ColorHdr ColorHdr::Red(1.0f, 0.0f, 0.0f);
const ColorHdr ColorHdr::Green(0.0f, 1.0f, 0.0f);
const ColorHdr ColorHdr::Blue(0.0f, 0.0f, 1.0f);

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ColorHdr::ColorHdr()
    : ColorHdr(1.0f, 1.0f, 1.0f)
{
}

ColorHdr::ColorHdr(float red, float green, float blue)
    : channels_(nctl::StaticArrayMode::EXTEND_SIZE)
{
	set(red, green, blue);
}

ColorHdr::ColorHdr(const float channels[NumChannels])
    : channels_(nctl::StaticArrayMode::EXTEND_SIZE)
{
	setVec(channels);
}

ColorHdr::ColorHdr(const Colorf &color)
    : ColorHdr(color.r(), color.g(), color.b())
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ColorHdr::set(float red, float green, float blue)
{
	channels_[0] = red;
	channels_[1] = green;
	channels_[2] = blue;
}

void ColorHdr::setVec(const float channels[NumChannels])
{
	set(channels[0], channels[1], channels[2]);
}

void ColorHdr::clamp()
{
	for (unsigned int i = 0; i < NumChannels; i++)
	{
		if (channels_[i] < 0.0f)
			channels_[i] = 0.0f;
	}
}

ColorHdr ColorHdr::clamped() const
{
	ColorHdr result;

	for (unsigned int i = 0; i < NumChannels; i++)
	{
		if (channels_[i] < 0.0f)
			result.channels_[i] = 0.0f;
		else
			result.channels_[i] = channels_[i];
	}

	return result;
}

ColorHdr &ColorHdr::operator=(const Colorf &color)
{
	channels_[0] = color.r();
	channels_[1] = color.g();
	channels_[2] = color.b();

	return *this;
}

ColorHdr &ColorHdr::operator+=(const ColorHdr &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
		channels_[i] += color.channels_[i];

	return *this;
}

ColorHdr &ColorHdr::operator-=(const ColorHdr &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
		channels_[i] -= color.channels_[i];

	return *this;
}

ColorHdr &ColorHdr::operator*=(const ColorHdr &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
		channels_[i] *= color.channels_[i];

	return *this;
}

ColorHdr &ColorHdr::operator*=(float scalar)
{
	for (unsigned int i = 0; i < NumChannels; i++)
		channels_[i] *= scalar;

	return *this;
}

ColorHdr ColorHdr::operator+(const ColorHdr &color) const
{
	ColorHdr result;

	for (unsigned int i = 0; i < NumChannels; i++)
		result.channels_[i] = channels_[i] + color.channels_[i];

	return result;
}

ColorHdr ColorHdr::operator-(const ColorHdr &color) const
{
	ColorHdr result;

	for (unsigned int i = 0; i < NumChannels; i++)
		result.channels_[i] = channels_[i] - color.channels_[i];

	return result;
}

ColorHdr ColorHdr::operator*(const ColorHdr &color) const
{
	ColorHdr result;

	for (unsigned int i = 0; i < NumChannels; i++)
		result.channels_[i] = channels_[i] * color.channels_[i];

	return result;
}

ColorHdr ColorHdr::operator*(float scalar) const
{
	ColorHdr result;

	for (unsigned int i = 0; i < NumChannels; i++)
		result.channels_[i] = channels_[i] * scalar;

	return result;
}

}
