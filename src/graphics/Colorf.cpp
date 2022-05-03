#include "Colorf.h"
#include "Color.h"
#include "ColorHdr.h"
#include <nctl/algorithms.h>

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const Colorf Colorf::Black(0.0f, 0.0f, 0.0f, 1.0f);
const Colorf Colorf::White(1.0f, 1.0f, 1.0f, 1.0f);
const Colorf Colorf::Red(1.0f, 0.0f, 0.0f, 1.0f);
const Colorf Colorf::Green(0.0f, 1.0f, 0.0f, 1.0f);
const Colorf Colorf::Blue(0.0f, 0.0f, 1.0f, 1.0f);
const Colorf Colorf::Yellow(1.0f, 1.0f, 0.0f, 1.0f);
const Colorf Colorf::Magenta(1.0f, 0.0f, 1.0f, 1.0f);
const Colorf Colorf::Cyan(0.0f, 1.0f, 1.0f, 1.0f);

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Colorf::Colorf()
    : Colorf(1.0f, 1.0f, 1.0f, 1.0f)
{
}

Colorf::Colorf(float red, float green, float blue)
    : Colorf(red, green, blue, 1.0f)
{
}

Colorf::Colorf(float red, float green, float blue, float alpha)
{
	set(red, green, blue, alpha);
}

Colorf::Colorf(const float channels[NumChannels])
{
	setVec(channels);
}

Colorf::Colorf(const Color &color)
{
	const float inv = 1.0f / 255.0f;
	red_ = static_cast<float>(color.r() * inv);
	green_ = static_cast<float>(color.g() * inv);
	blue_ = static_cast<float>(color.b() * inv);
	alpha_ = static_cast<float>(color.a() * inv);
}

Colorf::Colorf(const ColorHdr &color)
    : Colorf(color.r(), color.g(), color.b(), 1.0f)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Colorf::set(float red, float green, float blue, float alpha)
{
	red_ = nctl::clamp(red, 0.0f, 1.0f);
	green_ = nctl::clamp(green, 0.0f, 1.0f);
	blue_ = nctl::clamp(blue, 0.0f, 1.0f);
	alpha_ = nctl::clamp(alpha, 0.0f, 1.0f);
}

void Colorf::set(float red, float green, float blue)
{
	red_ = nctl::clamp(red, 0.0f, 1.0f);
	green_ = nctl::clamp(green, 0.0f, 1.0f);
	blue_ = nctl::clamp(blue, 0.0f, 1.0f);
}

void Colorf::setVec(const float channels[NumChannels])
{
	set(channels[0], channels[1], channels[2], channels[3]);
}

void Colorf::setAlpha(float alpha)
{
	alpha_ = nctl::clamp(alpha, 0.0f, 1.0f);
}

Colorf &Colorf::operator=(const Color &color)
{
	const float inv = 1.0f / 255.0f;
	red_ = static_cast<float>(color.r() * inv);
	green_ = static_cast<float>(color.g() * inv);
	blue_ = static_cast<float>(color.b() * inv);
	alpha_ = static_cast<float>(color.a() * inv);

	return *this;
}

Colorf &Colorf::operator+=(const Colorf &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
	{
		const float channelValue = data()[i] + color.data()[i];
		data()[i] = nctl::clamp(channelValue, 0.0f, 1.0f);
	}

	return *this;
}

Colorf &Colorf::operator-=(const Colorf &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
	{
		const float channelValue = data()[i] - color.data()[i];
		data()[i] = nctl::clamp(channelValue, 0.0f, 1.0f);
	}

	return *this;
}

Colorf &Colorf::operator*=(const Colorf &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
	{
		const float channelValue = data()[i] * color.data()[i];
		data()[i] = nctl::clamp(channelValue, 0.0f, 1.0f);
	}

	return *this;
}

Colorf &Colorf::operator*=(float scalar)
{
	for (unsigned int i = 0; i < NumChannels; i++)
	{
		const float channelValue = data()[i] * scalar;
		data()[i] = nctl::clamp(channelValue, 0.0f, 1.0f);
	}

	return *this;
}

Colorf Colorf::operator+(const Colorf &color) const
{
	Colorf result;

	for (unsigned int i = 0; i < NumChannels; i++)
	{
		const float channelValue = data()[i] + color.data()[i];
		result.data()[i] = nctl::clamp(channelValue, 0.0f, 1.0f);
	}

	return result;
}

Colorf Colorf::operator-(const Colorf &color) const
{
	Colorf result;

	for (unsigned int i = 0; i < NumChannels; i++)
	{
		const float channelValue = data()[i] - color.data()[i];
		result.data()[i] = nctl::clamp(channelValue, 0.0f, 1.0f);
	}

	return result;
}

Colorf Colorf::operator*(const Colorf &color) const
{
	Colorf result;

	for (unsigned int i = 0; i < NumChannels; i++)
	{
		const float channelValue = data()[i] * color.data()[i];
		result.data()[i] = nctl::clamp(channelValue, 0.0f, 1.0f);
	}

	return result;
}

Colorf Colorf::operator*(float scalar) const
{
	Colorf result;

	for (unsigned int i = 0; i < NumChannels; i++)
	{
		const float channelValue = data()[i] * scalar;
		result.data()[i] = nctl::clamp(channelValue, 0.0f, 1.0f);
	}

	return result;
}

}
