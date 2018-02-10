#ifndef CLASS_NCINE_COLOR
#define CLASS_NCINE_COLOR

#include "nctl/algorithms.h"
#include "nctl/StaticArray.h"

namespace ncine {

/// A 32 bit color
class DLL_PUBLIC Color
{
  public:
	/// Default constructor (white color)
	Color() { set(255, 255, 255, 255); }
	/// Three channels constructor (`unsigned int`)
	Color(unsigned int red, unsigned int green, unsigned int blue)
		: Color(red, green, blue, 255) { }
	/// Four channels constructor (`unsigned int`)
	Color(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
	{
		set(red, green, blue, alpha);
	}
	/// Three channels constructor (normalized `float`)
	Color(float red, float green, float blue) : Color(red, green, blue, 1.0f) { }
	/// Four channels constructor (normalized `float`)
	Color(float red, float green, float blue, float alpha)
	{
		setF(red, green, blue, alpha);
	}

	/// Getter for the red channel of the color
	inline unsigned char r() const { return channels_[0]; }
	/// Getter for the green channel of the color
	inline unsigned char g() const { return channels_[1]; }
	/// Getter for the blue channel of the color
	inline unsigned char b() const { return channels_[2]; }
	/// Getter for the alpha channel of the color
	inline unsigned char a() const { return channels_[3]; }
	/// Getter for the color vector
	inline const unsigned char *vector() const { return channels_.data(); }
	/// Flaot getter for the red channel of the color
	inline float fR() const { return channels_[0] / 255.0f; }
	/// Float getter for the green channel of the color
	inline float fG() const { return channels_[1] / 255.0f; }
	/// Float getter for the blue channel of the color
	inline float fB() const { return channels_[2] / 255.0f; }
	/// Float getter for the alpha channel of the color
	inline float fA() const { return channels_[3] / 255.0f; }

	/// Sets four color channels (`unsigned int`)
	inline void set(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha);
	/// Sets three color channels (`unsigned int`)
	inline void set(unsigned int red, unsigned int green, unsigned int blue);
	/// Sets four color channels (`unsigned int` vector)
	inline void setVec(unsigned int channels[4]);

	/// Sets four color channels (normalized `float`)
	inline void setF(float red, float green, float blue, float alpha);
	/// Sets three color channels (normalized `float`)
	inline void setF(float red, float green, float blue);
	/// Sets four color channels (`float` vector)
	inline void setFVec(float channels[4]);

	/// Sets the alpha channel (`unsigned int`)
	inline void setAlpha(unsigned int alpha);
	/// Sets the alpha channel (normalized `float`)
	inline void setAlphaF(float alpha);

	/// Equality operator
	inline bool operator==(const Color &color) const;

	inline Color &operator*=(const Color &color);
	/// Multiplication by a constant scalar
	inline Color &operator*=(float scalar);

	inline Color operator*(const Color &color) const;
	/// Multiplication by a constant scalar
	inline Color operator*(float scalar) const;

  private:
	/// The four unsigned byte color channels
	nctl::StaticArray<unsigned char, 4> channels_;
};

inline void Color::set(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
{
	channels_[0] = static_cast<unsigned char>(red);
	channels_[1] = static_cast<unsigned char>(green);
	channels_[2] = static_cast<unsigned char>(blue);
	channels_[3] = static_cast<unsigned char>(alpha);
}

inline void Color::set(unsigned int red, unsigned int green, unsigned int blue)
{
	set(red, green, blue, 255);
}

inline void Color::setVec(unsigned int channels[])
{
	set(channels[0], channels[1], channels[2], channels[3]);
}

inline void Color::setF(float red, float green, float blue, float alpha)
{
	channels_[0] = static_cast<unsigned char>(nctl::clamp(red, 0.0f, 1.0f) * 255);
	channels_[1] = static_cast<unsigned char>(nctl::clamp(green, 0.0f, 1.0f) * 255);
	channels_[2] = static_cast<unsigned char>(nctl::clamp(blue, 0.0f, 1.0f) * 255);
	channels_[3] = static_cast<unsigned char>(nctl::clamp(alpha, 0.0f, 1.0f) * 255);
}

inline void Color::setF(float red, float green, float blue)
{
	setF(red, green, blue, 1.0f);
}

inline void Color::setFVec(float channels[4])
{
	setF(channels[0], channels[1], channels[2], channels[3]);
}

inline void Color::setAlpha(unsigned int alpha)
{
	channels_[3] = static_cast<unsigned char>(alpha);
}

inline void Color::setAlphaF(float alpha)
{
	channels_[3] = static_cast<unsigned char>(nctl::clamp(alpha, 0.0f, 1.0f) * 255);
}

inline bool Color::operator==(const Color &color) const
{
	return (r() == color.r() && g() == color.g() &&
	        b() == color.b() && a() == color.a());
}

inline Color &Color::operator*=(const Color &color)
{
	for (unsigned int i = 0; i < 4; i++)
	{
		float channelValue = (color.channels_[i] / 255.0f) * channels_[i];
		channelValue = nctl::clamp(channelValue, 0.0f, 255.0f);
		channels_[i] = static_cast<unsigned char>(channelValue);
	}

	return *this;
}

inline Color &Color::operator*=(float scalar)
{
	for (unsigned int i = 0; i < 4; i++)
	{
		float channelValue = scalar * channels_[i];
		channelValue = nctl::clamp(channelValue, 0.0f, 255.0f);
		channels_[i] = static_cast<unsigned char>(channelValue);
	}

	return *this;
}

inline Color Color::operator*(const Color &color) const
{
	Color result;

	for (unsigned int i = 0; i < 4; i++)
	{
		float channelValue = (color.channels_[i] / 255.0f) * channels_[i];
		channelValue = nctl::clamp(channelValue, 0.0f, 255.0f);
		result.channels_[i] = static_cast<unsigned char>(channelValue);
	}

	return result;
}

inline Color Color::operator*(float scalar) const
{
	Color result;

	for (unsigned int i = 0; i < 4; i++)
	{
		float channelValue = scalar * channels_[i];
		channelValue = nctl::clamp(channelValue, 0.0f, 255.0f);
		result.channels_[i] = static_cast<unsigned char>(channelValue);
	}

	return result;
}

}

#endif
