#ifndef CLASS_NCINE_COLOR
#define CLASS_NCINE_COLOR

#include <nctl/StaticArray.h>

namespace ncine {

class Colorf;

/// A four channels unsigned char color
class DLL_PUBLIC Color
{
  public:
	/// Default constructor (white color)
	Color();
	/// Three channels constructor
	Color(unsigned int red, unsigned int green, unsigned int blue);
	/// Four channels constructor
	Color(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha);
	/// Four channels constructor from an array
	Color(const unsigned int channels[]);
	/// Construtor taking a normalized float color
	explicit Color(const Colorf &color);

	/// Gets the red channel of the color
	inline unsigned char r() const { return channels_[0]; }
	/// Gets the green channel of the color
	inline unsigned char g() const { return channels_[1]; }
	/// Gets the blue channel of the color
	inline unsigned char b() const { return channels_[2]; }
	/// Gets the alpha channel of the color
	inline unsigned char a() const { return channels_[3]; }
	/// Gets the color array
	inline const unsigned char *data() const { return channels_.data(); }
	/// Gets the color array
	inline unsigned char *data() { return channels_.data(); }

	/// Sets four color channels
	void set(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha);
	/// Sets three color channels
	void set(unsigned int red, unsigned int green, unsigned int blue);
	/// Sets four color channels from an array
	void setVec(const unsigned int channels[]);

	/// Sets the alpha channel
	void setAlpha(unsigned int alpha);

	/// Assignment operator from a normalized float color
	Color &operator=(const Colorf &color);

	/// Equality operator
	bool operator==(const Color &color) const;

	Color &operator*=(const Color &color);
	/// Multiplication by a constant scalar
	Color &operator*=(float scalar);

	Color operator*(const Color &color) const;
	/// Multiplication by a constant scalar
	Color operator*(float scalar) const;

	static const Color Black;
	static const Color White;
	static const Color Red;
	static const Color Green;
	static const Color Blue;

  private:
	/// The four unsigned byte color channels
	nctl::StaticArray<unsigned char, 4> channels_;
};

}

#endif
