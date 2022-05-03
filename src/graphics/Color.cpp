#include "Color.h"
#include "Colorf.h"
#include <nctl/algorithms.h>

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const Color Color::Black(0, 0, 0, 255);
const Color Color::White(255, 255, 255, 255);
const Color Color::Red(255, 0, 0, 255);
const Color Color::Green(0, 255, 0, 255);
const Color Color::Blue(0, 0, 255, 255);
const Color Color::Yellow(255, 255, 0, 255);
const Color Color::Magenta(255, 0, 255, 255);
const Color Color::Cyan(0, 255, 255, 255);

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Color::Color()
    : Color(255, 255, 255, 255)
{
}

Color::Color(unsigned int red, unsigned int green, unsigned int blue)
    : Color(red, green, blue, 255)
{
}

Color::Color(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
{
	set(red, green, blue, alpha);
}

Color::Color(unsigned int hex)
{
	setAlpha(255);
	// The following method might set the alpha channel
	set(hex);
}

Color::Color(const unsigned int channels[NumChannels])
{
	setVec(channels);
}

Color::Color(const Colorf &color)
{
	red_ = static_cast<unsigned char>(color.r() * 255);
	green_ = static_cast<unsigned char>(color.g() * 255);
	blue_ = static_cast<unsigned char>(color.b() * 255);
	alpha_ = static_cast<unsigned char>(color.a() * 255);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int Color::rgba() const
{
	return (red_ << 24) + (green_ << 16) + (blue_ << 8) + alpha_;
}

unsigned int Color::argb() const
{
	return (alpha_ << 24) + (red_ << 16) + (green_ << 8) + blue_;
}

unsigned int Color::abgr() const
{
	return (alpha_ << 24) + (blue_ << 16) + (green_ << 8) + red_;
}

unsigned int Color::bgra() const
{
	return (blue_ << 24) + (green_ << 16) + (red_ << 8) + alpha_;
}

void Color::set(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
{
	red_ = static_cast<unsigned char>(red);
	green_ = static_cast<unsigned char>(green);
	blue_ = static_cast<unsigned char>(blue);
	alpha_ = static_cast<unsigned char>(alpha);
}

void Color::set(unsigned int red, unsigned int green, unsigned int blue)
{
	red_ = static_cast<unsigned char>(red);
	green_ = static_cast<unsigned char>(green);
	blue_ = static_cast<unsigned char>(blue);
}

void Color::set(unsigned int hex)
{
	red_ = static_cast<unsigned char>((hex & 0xFF0000) >> 16);
	green_ = static_cast<unsigned char>((hex & 0xFF00) >> 8);
	blue_ = static_cast<unsigned char>(hex & 0xFF);

	if (hex > 0xFFFFFF)
		alpha_ = static_cast<unsigned char>((hex & 0xFF000000) >> 24);
}

void Color::setVec(const unsigned int channels[NumChannels])
{
	set(channels[0], channels[1], channels[2], channels[3]);
}

void Color::setAlpha(unsigned int alpha)
{
	alpha_ = static_cast<unsigned char>(alpha);
}

Color &Color::operator=(const Colorf &color)
{
	red_ = static_cast<unsigned char>(color.r() * 255.0f);
	green_ = static_cast<unsigned char>(color.g() * 255.0f);
	blue_ = static_cast<unsigned char>(color.b() * 255.0f);
	alpha_ = static_cast<unsigned char>(color.a() * 255.0f);

	return *this;
}

bool Color::operator==(const Color &color) const
{
	return (r() == color.r() && g() == color.g() &&
	        b() == color.b() && a() == color.a());
}

Color &Color::operator+=(const Color &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
	{
		unsigned int channelValue = data()[i] + color.data()[i];
		channelValue = nctl::clamp(channelValue, 0U, 255U);
		data()[i] = static_cast<unsigned char>(channelValue);
	}

	return *this;
}

Color &Color::operator-=(const Color &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
	{
		unsigned int channelValue = data()[i] - color.data()[i];
		channelValue = nctl::clamp(channelValue, 0U, 255U);
		data()[i] = static_cast<unsigned char>(channelValue);
	}

	return *this;
}

Color &Color::operator*=(const Color &color)
{
	for (unsigned int i = 0; i < NumChannels; i++)
	{
		float channelValue = data()[i] * (color.data()[i] / 255.0f);
		channelValue = nctl::clamp(channelValue, 0.0f, 255.0f);
		data()[i] = static_cast<unsigned char>(channelValue);
	}

	return *this;
}

Color &Color::operator*=(float scalar)
{
	for (unsigned int i = 0; i < NumChannels; i++)
	{
		float channelValue = data()[i] * scalar;
		channelValue = nctl::clamp(channelValue, 0.0f, 255.0f);
		data()[i] = static_cast<unsigned char>(channelValue);
	}

	return *this;
}

Color Color::operator+(const Color &color) const
{
	Color result;

	for (unsigned int i = 0; i < NumChannels; i++)
	{
		unsigned int channelValue = data()[i] + color.data()[i];
		channelValue = nctl::clamp(channelValue, 0U, 255U);
		result.data()[i] = static_cast<unsigned char>(channelValue);
	}

	return result;
}

Color Color::operator-(const Color &color) const
{
	Color result;

	for (unsigned int i = 0; i < NumChannels; i++)
	{
		unsigned int channelValue = data()[i] - color.data()[i];
		channelValue = nctl::clamp(channelValue, 0U, 255U);
		result.data()[i] = static_cast<unsigned char>(channelValue);
	}

	return result;
}

Color Color::operator*(const Color &color) const
{
	Color result;

	for (unsigned int i = 0; i < NumChannels; i++)
	{
		float channelValue = data()[i] * (color.data()[i] / 255.0f);
		channelValue = nctl::clamp(channelValue, 0.0f, 255.0f);
		result.data()[i] = static_cast<unsigned char>(channelValue);
	}

	return result;
}

Color Color::operator*(float scalar) const
{
	Color result;

	for (unsigned int i = 0; i < NumChannels; i++)
	{
		float channelValue = data()[i] * scalar;
		channelValue = nctl::clamp(channelValue, 0.0f, 255.0f);
		result.data()[i] = static_cast<unsigned char>(channelValue);
	}

	return result;
}

}
