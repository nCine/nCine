#ifndef CLASS_NCINE_COLORHDR
#define CLASS_NCINE_COLORHDR

#include <nctl/StaticArray.h>

namespace ncine {

class Colorf;

/// A three channels unclamped float color
class DLL_PUBLIC ColorHdr
{
  public:
	static const int NumChannels = 3;
	static const ColorHdr Black;
	static const ColorHdr White;
	static const ColorHdr Red;
	static const ColorHdr Green;
	static const ColorHdr Blue;

	/// Default constructor (white color)
	ColorHdr();
	/// Three channels constructor
	ColorHdr(float red, float green, float blue);
	/// Three channels constructor from an array
	explicit ColorHdr(const float channels[NumChannels]);
	/// Constructor taking a float color
	explicit ColorHdr(const Colorf &color);

	/// Gets the red channel of the color
	inline float r() const { return channels_[0]; }
	/// Gets the green channel of the color
	inline float g() const { return channels_[1]; }
	/// Gets the blue channel of the color
	inline float b() const { return channels_[2]; }
	/// Gets the color array
	inline const float *data() const { return channels_.data(); }
	/// Gets the color array
	inline float *data() { return channels_.data(); }

	/// Sets three color channels
	void set(float red, float green, float blue);
	/// Sets four color channels from an array
	void setVec(const float channels[NumChannels]);

	/// Clamps negative channel values to zero
	void clamp();
	/// Returns a clamped version of this color
	ColorHdr clamped() const;

	/// Assignment operator from a float color
	ColorHdr &operator=(const Colorf &color);

	ColorHdr &operator+=(const ColorHdr &v);
	ColorHdr &operator-=(const ColorHdr &v);

	ColorHdr &operator*=(const ColorHdr &color);
	/// Multiplication by a constant scalar
	ColorHdr &operator*=(float scalar);

	ColorHdr operator+(const ColorHdr &color) const;
	ColorHdr operator-(const ColorHdr &color) const;

	ColorHdr operator*(const ColorHdr &color) const;
	/// Multiplication by a constant scalar
	ColorHdr operator*(float scalar) const;

  private:
	/// The three float color channels
	nctl::StaticArray<float, NumChannels> channels_;
};

}

#endif
