#ifndef CLASS_NCINE_COLORF
#define CLASS_NCINE_COLORF

#include <nctl/StaticArray.h>

namespace ncine {

class Color;
class ColorHdr;

/// A four channels normalized float color
class DLL_PUBLIC Colorf
{
  public:
	static const int NumChannels = 4;
	static const Colorf Black;
	static const Colorf White;
	static const Colorf Red;
	static const Colorf Green;
	static const Colorf Blue;
	static const Colorf Yellow;
	static const Colorf Magenta;
	static const Colorf Cyan;

	/// Default constructor (white color)
	Colorf();
	/// Three channels constructor
	Colorf(float red, float green, float blue);
	/// Four channels constructor
	Colorf(float red, float green, float blue, float alpha);
	/// Four channels constructor from an array
	explicit Colorf(const float channels[NumChannels]);
	/// Constructor taking an unsigned char color
	explicit Colorf(const Color &color);
	/// Constructor taking an unclamped float color
	explicit Colorf(const ColorHdr &color);

	/// Gets the red channel of the color
	inline float r() const { return channels_[0]; }
	/// Gets the green channel of the color
	inline float g() const { return channels_[1]; }
	/// Gets the blue channel of the color
	inline float b() const { return channels_[2]; }
	/// Gets the alpha channel of the color
	inline float a() const { return channels_[3]; }
	/// Gets the color array
	inline const float *data() const { return channels_.data(); }
	/// Gets the color array
	inline float *data() { return channels_.data(); }

	/// Sets four color channels
	void set(float red, float green, float blue, float alpha);
	/// Sets three color channels
	void set(float red, float green, float blue);
	/// Sets four color channels from an array
	void setVec(const float channels[NumChannels]);
	/// Sets the alpha channel
	void setAlpha(float alpha);

	/// Assignment operator from an unsigned char color
	Colorf &operator=(const Color &color);

	Colorf &operator+=(const Colorf &v);
	Colorf &operator-=(const Colorf &v);

	Colorf &operator*=(const Colorf &color);
	/// Multiplication by a constant scalar
	Colorf &operator*=(float scalar);

	Colorf operator+(const Colorf &color) const;
	Colorf operator-(const Colorf &color) const;

	Colorf operator*(const Colorf &color) const;
	/// Multiplication by a constant scalar
	Colorf operator*(float scalar) const;

  private:
	/// The four float color channels
	nctl::StaticArray<float, NumChannels> channels_;
};

}

#endif
