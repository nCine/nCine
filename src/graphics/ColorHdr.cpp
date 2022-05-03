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
{
	set(red, green, blue);
}

ColorHdr::ColorHdr(const float channels[NumChannels])
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
	red_ = red;
	green_ = green;
	blue_ = blue;
}

void ColorHdr::setVec(const float channels[NumChannels])
{
	set(channels[0], channels[1], channels[2]);
}

void ColorHdr::clamp()
{
	for (unsigned int i = 0; i < NumChannels; i++)
	{
		if (data()[i] < 0.0f)
			data()[i] = 0.0f;
	}
}

ColorHdr ColorHdr::clamped() const
{
	ColorHdr result;

	for (unsigned int i = 0; i < NumChannels; i++)
	{
		if (data()[i] < 0.0f)
			result.data()[i] = 0.0f;
		else
			result.data()[i] = data()[i];
	}

	return result;
}

ColorHdr &ColorHdr::operator=(const Colorf &color)
{
	red_ = color.r();
	green_ = color.g();
	blue_ = color.b();

	return *this;
}

ColorHdr &ColorHdr::operator+=(const ColorHdr &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
		data()[i] += color.data()[i];

	return *this;
}

ColorHdr &ColorHdr::operator-=(const ColorHdr &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
		data()[i] -= color.data()[i];

	return *this;
}

ColorHdr &ColorHdr::operator*=(const ColorHdr &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
		data()[i] *= color.data()[i];

	return *this;
}

ColorHdr &ColorHdr::operator*=(float scalar)
{
	for (unsigned int i = 0; i < NumChannels; i++)
		data()[i] *= scalar;

	return *this;
}

ColorHdr ColorHdr::operator+(const ColorHdr &color) const
{
	ColorHdr result;

	for (unsigned int i = 0; i < NumChannels; i++)
		result.data()[i] = data()[i] + color.data()[i];

	return result;
}

ColorHdr ColorHdr::operator-(const ColorHdr &color) const
{
	ColorHdr result;

	for (unsigned int i = 0; i < NumChannels; i++)
		result.data()[i] = data()[i] - color.data()[i];

	return result;
}

ColorHdr ColorHdr::operator*(const ColorHdr &color) const
{
	ColorHdr result;

	for (unsigned int i = 0; i < NumChannels; i++)
		result.data()[i] = data()[i] * color.data()[i];

	return result;
}

ColorHdr ColorHdr::operator*(float scalar) const
{
	ColorHdr result;

	for (unsigned int i = 0; i < NumChannels; i++)
		result.data()[i] = data()[i] * scalar;

	return result;
}

}
