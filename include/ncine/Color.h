#ifndef CLASS_NCINE_COLOR
#define CLASS_NCINE_COLOR

#include "common_defines.h"

namespace ncine {

class Colorf;

/// A four channels unsigned char color
class DLL_PUBLIC Color
{
  public:
	static const int NumChannels = 4;
	static const Color Black;
	static const Color White;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Yellow;
	static const Color Magenta;
	static const Color Cyan;

	/// Default constructor (white color)
	Color();
	/// Three channels constructor
	Color(unsigned int red, unsigned int green, unsigned int blue);
	/// Four channels constructor
	Color(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha);
	/// Three channels constructor from a hexadecimal code
	explicit Color(unsigned int hex);
	/// Four channels constructor from an array
	explicit Color(const unsigned int channels[NumChannels]);
	/// Constructor taking a normalized float color
	explicit Color(const Colorf &color);

	/// Gets the red channel of the color
	inline unsigned char r() const { return red_; }
	/// Gets the green channel of the color
	inline unsigned char g() const { return green_; }
	/// Gets the blue channel of the color
	inline unsigned char b() const { return blue_; }
	/// Gets the alpha channel of the color
	inline unsigned char a() const { return alpha_; }

	/// Returns the color as a single RGBA unsigned integer
	unsigned int rgba() const;
	/// Returns the color as a single RGBA unsigned integer
	unsigned int argb() const;
	/// Returns the color as a single ABGR unsigned integer
	unsigned int abgr() const;
	/// Returns the color as a single BGRA unsigned integer
	unsigned int bgra() const;

	/// Gets the color array
	inline const unsigned char *data() const { return &red_; }
	/// Gets the color array
	inline unsigned char *data() { return &red_; }

	/// Sets four color channels
	void set(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha);
	/// Sets three color channels
	void set(unsigned int red, unsigned int green, unsigned int blue);
	/// Sets three color channels from a hexadecimal code
	void set(unsigned int hex);
	/// Sets four color channels from an array
	void setVec(const unsigned int channels[NumChannels]);

	/// Sets the alpha channel
	void setAlpha(unsigned int alpha);

	/// Assignment operator from a normalized float color
	Color &operator=(const Colorf &color);

	/// Equality operator
	bool operator==(const Color &color) const;

	Color &operator+=(const Color &v);
	Color &operator-=(const Color &v);

	Color &operator*=(const Color &color);
	/// Multiplication by a constant scalar
	Color &operator*=(float scalar);

	Color operator+(const Color &color) const;
	Color operator-(const Color &color) const;

	Color operator*(const Color &color) const;
	/// Multiplication by a constant scalar
	Color operator*(float scalar) const;

  private:
	unsigned char red_;
	unsigned char green_;
	unsigned char blue_;
	unsigned char alpha_;
};

}

#endif
