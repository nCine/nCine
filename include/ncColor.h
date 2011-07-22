#ifndef CLASS_NCCOLOR
#define CLASS_NCCOLOR

#include <cstdio> // for printf()

/// A 32 bit color
class ncColor
{
public:
	/// Red component
	unsigned char r;
	/// Green component
	unsigned char g;
	/// Blue component
	unsigned char b;
	/// Alpha component
	unsigned char a;

	/// Default constructor (black color)
	ncColor() : r(0), g(0), b(0), a(255) { }
	/// Four components constructor (unsigned char)
	ncColor(unsigned char ucR, unsigned char ucG, unsigned char ucB, unsigned char ucA)
		: r(ucR), g(ucG), b(ucB), a(ucA) { }
	/// Three components constructor (unsigned char)
	ncColor(unsigned char ucR, unsigned char ucG, unsigned char ucB)
		: r(ucR), g(ucG), b(ucB), a(255) { }
	/// Four components constructor (normalized float)
	ncColor(float fR, float fG, float fB, float fA)
		: r(0), g(0), b(0), a(0) { Set(fR, fG, fB, fA); }
	/// Three components constructor (normalized float)
	ncColor(float fR, float fG, float fB)
		: r(0), g(0), b(0), a(0) { Set(fR, fG, fB); }


	/// Set four color elements (unsigned char)
	inline void Set(unsigned char ucR, unsigned char ucG, unsigned char ucB, unsigned char ucA)
	{
		r = ucR;
		g = ucG;
		b = ucB;
		a = ucA;
	}
	/// Set three color elements (unsigned char)
	inline void Set(unsigned char ucR, unsigned char ucG, unsigned char ucB)
	{
		Set(ucR, ucG, ucB, 255);
	}

	/// Set four color elements (normalized float)
	inline void Set(float fR, float fG, float fB, float fA)
	{

		r = fR > 1.0f ? 255 : fR * 255;
		g = fG > 1.0f ? 255 : fG * 255;
		b = fB > 1.0f ? 255 : fB * 255;
		a = fA > 1.0f ? 255 : fA * 255;
	}
	/// Set three color elements (normalized float)
	inline void Set(float fR, float fG, float fB) {
		Set(fR, fG, fB, 1.0f);
	}

	/// Equality operator
	inline bool operator==(const ncColor& color) const {
		return (r == color.r && g == color.g && b == color.b && a == color.a);
	}

	inline void Print() const {
		printf("(%u, %u, %u, %u)", r, g, b, a);
	}
};

#endif
