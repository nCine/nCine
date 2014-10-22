#ifndef CLASS_NCCOLOR
#define CLASS_NCCOLOR

/// A 32 bit color
class ncColor
{
  public:
	/// Default constructor (white color)
	ncColor()
	{ set(255, 255, 255, 255); }
	/// Four channels constructor (unsigned char)
	ncColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
	{ set(red, green, blue, alpha); }
	/// Three channels constructor (unsigned char)
	ncColor(unsigned char red, unsigned char green, unsigned char blue)
	{ set(red, green, blue); }
	/// Four channels constructor (unsigned int)
	ncColor(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
	{ set(red, green, blue, alpha); }
	/// Three channels constructor (unsigned int)
	ncColor(unsigned int red, unsigned int green, unsigned int blue)
	{ set(red, green, blue); }
	/// Four channels constructor (normalized float)
	ncColor(float red, float green, float blue, float alpha)
	{ setF(red, green, blue, alpha); }
	/// Three channels constructor (normalized float)
	ncColor(float red, float green, float blue)
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
	/*! Note: It is useful with glColor4ub */
	inline const unsigned char* vector() const { return channels_; }
	/// Flaot getter for the red channel of the color
	inline float fR() const { return channels_[0] / 255.0f; }
	/// Float getter for the green channel of the color
	inline float fG() const { return channels_[1] / 255.0f; }
	/// Float getter for the blue channel of the color
	inline float fB() const { return channels_[2] / 255.0f; }
	/// Float getter for the alpha channel of the color
	inline float fA() const { return channels_[3] / 255.0f; }

	/// Ses four color channels (unsigned char)
	void set(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
	// Sets three color channels (unsigned char)
	void set(unsigned char red, unsigned char green, unsigned char blue);
	// Sets four color channels (unsigned char vector)
	void setVec(unsigned char channels[4]);

	// Sets four color channels (normalized float)
	void setF(float red, float green, float blue, float alpha);
	// Sets three color channels (normalized float)
	void setF(float red, float green, float blue);
	// Sets four color channels (float vector)
	void setFVec(float channels[4]);

	// Sets the alpha channel (unsigned char)
	void setAlpha(unsigned char alpha);
	// Sets the alpha channel (normalized float)
	void setAlphaF(float alpha);

	// Equality operator
	bool operator==(const ncColor& color) const;
	ncColor operator*(const ncColor& color) const;
	// Multiplication by a constant scalar
	ncColor operator*(float scalar) const;

  private:
	/// The four unsigned byte color channels
	unsigned char channels_[4];
};


/// Sets four color channels (unsigned char)
inline void ncColor::set(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	channels_[0] = red;
	channels_[1] = green;
	channels_[2] = blue;
	channels_[3] = alpha;
}

/// Sets three color channels (unsigned char)
inline void ncColor::set(unsigned char red, unsigned char green, unsigned char blue)
{
	set(red, green, blue, 255);
}

/// Sets four color channels (unsigned char vector)
inline void ncColor::setVec(unsigned char channels[4])
{
	set(channels[0], channels[1], channels[2], channels[3]);
}

/// Sets four color channels (normalized float)
inline void ncColor::setF(float red, float green, float blue, float alpha)
{
	// TODO: Clamp negative values too
	channels_[0] = red > 1.0f ? 255 : static_cast<unsigned char>(red * 255);
	channels_[1] = green > 1.0f ? 255 : static_cast<unsigned char>(green * 255);
	channels_[2] = blue > 1.0f ? 255 : static_cast<unsigned char>(blue * 255);
	channels_[3] = alpha > 1.0f ? 255 : static_cast<unsigned char>(alpha * 255);
}

/// Sets three color channels (normalized float)
inline void ncColor::setF(float red, float green, float blue)
{
	setF(red, green, blue, 1.0f);
}

/// Sets four color channels (float vector)
inline void ncColor::setFVec(float channels[4])
{
	setF(channels[0], channels[1], channels[2], channels[3]);
}

/// Sets the alpha channel (unsigned char)
inline void ncColor::setAlpha(unsigned char alpha)
{
	channels_[3] = alpha;
}

/// Sets the alpha channel (normalized float)
inline void ncColor::setAlphaF(float alpha)
{
	// TODO: Clamp negative values too
	channels_[3] = alpha > 1.0f ? 255 : static_cast<unsigned char>(alpha * 255);
}

/// Equality operator
inline bool ncColor::operator==(const ncColor& color) const
{
	return (r() == color.r() && g() == color.g() &&
			b() == color.b() && a() == color.a());
}

inline ncColor ncColor::operator*(const ncColor& color) const
{
	ncColor result;

	for (int i = 0; i < 4; i++)
	{
		float channelValue = (color.channels_[i] / 255.0f) * channels_[i];
		// clamping
		if (channelValue > 255.0f)
		{
			channelValue = 255.0f;
		}
		else if (channelValue < 0.0f)
		{
			channelValue = 0.0f;
		}

		result.channels_[i] = static_cast<unsigned char>(channelValue);
	}

	return result;
}

/// Multiplication by a constant scalar
inline ncColor ncColor::operator*(float scalar) const
{
	ncColor result;

	for (int i = 0; i < 4; i++)
	{
		float channelValue = scalar * channels_[i];
		// clamping
		if (channelValue > 255.0f)
		{
			channelValue = 255.0f;
		}
		else if (channelValue < 0.0f)
		{
			channelValue = 0.0f;
		}

		result.channels_[i] = static_cast<unsigned char>(channelValue);
	}

	return result;
}

#endif
