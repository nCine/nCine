#ifndef CLASS_NCINE_COLOR
#define CLASS_NCINE_COLOR

#include "algorithms.h"
#include "StaticArray.h"

namespace ncine {

/// A 32 bit color
class DLL_PUBLIC Color
{
  public:
	/// Default constructor (white color)
	Color()
	{ set(255, 255, 255, 255); }
	/// Four channels constructor (`unsigned char`)
	Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
	{ set(red, green, blue, alpha); }
	/// Three channels constructor (`unsigned char`)
	Color(unsigned char red, unsigned char green, unsigned char blue)
	{ set(red, green, blue); }
	/// Four channels constructor (`unsigned int`)
	Color(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
	{
		set(static_cast<unsigned char>(red), static_cast<unsigned char>(green),
		    static_cast<unsigned char>(blue), static_cast<unsigned char>(alpha));
	}
	/// Three channels constructor (`unsigned int`)
	Color(unsigned int red, unsigned int green, unsigned int blue)
	{ set(static_cast<unsigned char>(red), static_cast<unsigned char>(green), static_cast<unsigned char>(blue)); }
	/// Four channels constructor (normalized `float`)
	Color(float red, float green, float blue, float alpha)
	{ setF(red, green, blue, alpha); }
	/// Three channels constructor (normalized `float`)
	Color(float red, float green, float blue)
	{ setF(red, green, blue); }

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

	/// Sets four color channels (`unsigned char`)
	void set(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
	/// Sets three color channels (`unsigned char`)
	void set(unsigned char red, unsigned char green, unsigned char blue);
	/// Sets four color channels (`unsigned char` vector)
	void setVec(unsigned char channels[4]);

	/// Sets four color channels (normalized `float`)
	void setF(float red, float green, float blue, float alpha);
	/// Sets three color channels (normalized `float`)
	void setF(float red, float green, float blue);
	/// Sets four color channels (`float` vector)
	void setFVec(float channels[4]);

	/// Sets the alpha channel (`unsigned char`)
	void setAlpha(unsigned char alpha);
	/// Sets the alpha channel (normalized `float`)
	void setAlphaF(float alpha);

	/// Equality operator
	bool operator==(const Color &color) const;

	Color &operator*=(const Color &color);
	/// Multiplication by a constant scalar
	Color &operator*=(float scalar);

	Color operator*(const Color &color) const;
	/// Multiplication by a constant scalar
	Color operator*(float scalar) const;

  private:
	/// The four unsigned byte color channels
	StaticArray<unsigned char, 4> channels_;
};


inline void Color::set(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	channels_[0] = red;
	channels_[1] = green;
	channels_[2] = blue;
	channels_[3] = alpha;
}

inline void Color::set(unsigned char red, unsigned char green, unsigned char blue)
{
	set(red, green, blue, 255);
}

inline void Color::setVec(unsigned char channels[4])
{
	set(channels[0], channels[1], channels[2], channels[3]);
}

inline void Color::setF(float red, float green, float blue, float alpha)
{
	channels_[0] = static_cast<unsigned char>(nc::clamp(red, 0.0f, 1.0f) * 255);
	channels_[1] = static_cast<unsigned char>(nc::clamp(green, 0.0f, 1.0f) * 255);
	channels_[2] = static_cast<unsigned char>(nc::clamp(blue, 0.0f, 1.0f) * 255);
	channels_[3] = static_cast<unsigned char>(nc::clamp(alpha, 0.0f, 1.0f) * 255);
}

inline void Color::setF(float red, float green, float blue)
{
	setF(red, green, blue, 1.0f);
}

inline void Color::setFVec(float channels[4])
{
	setF(channels[0], channels[1], channels[2], channels[3]);
}

inline void Color::setAlpha(unsigned char alpha)
{
	channels_[3] = alpha;
}

inline void Color::setAlphaF(float alpha)
{
	channels_[3] = static_cast<unsigned char>(nc::clamp(alpha, 0.0f, 1.0f) * 255);
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
		channelValue = nc::clamp(channelValue, 0.0f, 255.0f);
		channels_[i] = static_cast<unsigned char>(channelValue);
	}

	return *this;
}

inline Color &Color::operator*=(float scalar)
{
	for (unsigned int i = 0; i < 4; i++)
	{
		float channelValue = scalar * channels_[i];
		channelValue = nc::clamp(channelValue, 0.0f, 255.0f);
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
		channelValue = nc::clamp(channelValue, 0.0f, 255.0f);
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
		channelValue = nc::clamp(channelValue, 0.0f, 255.0f);
		result.channels_[i] = static_cast<unsigned char>(channelValue);
	}

	return result;
}

}

#endif
