#ifndef CLASS_NCINE_RECT
#define CLASS_NCINE_RECT

#include "Point.h"

namespace ncine {

/// A rectangle in a two dimensional space
class Rect
{
  public:
	/// Top-left X coordinate of as a public property
	int x;
	/// Top-left Y coordinate as a public property
	int y;
	/// Width as a public property
	int w;
	/// Height as a public property
	int h;

	/// Default constructor, all zeros
	Rect() : x(0), y(0), w(0), h(0) { }
	/// Constructs from elements
	Rect(int const xx, int const yy, int const ww, int const hh)
		: x(xx), y(yy), w(ww), h(hh) { }
	/// Constructs from base classes
	Rect(const Point &p1, const Point &p2)
		: x(p1.x), y(p1.y), w(p2.x), h(p2.y) { }

	// Creates a rect from center and size
	static Rect fromCenterAndSize(const Point &center, const Point &size);

	// Calculates the center of the rect
	Point center() const;
	// Sets rect elements
	void set(int const xx, int const yy, int const ww, int const hh);
	// Retains size but move to another position
	void setCenter(const Point& center);

	/// Eqality operators
	bool operator==(const Rect& rect) const;
};

/// Creates a rect from center and size
inline Rect Rect::fromCenterAndSize(const Point &center, const Point &size)
{
	return Rect(center.x - static_cast<int>(size.x * 0.5f),
				  center.y - static_cast<int>(size.y * 0.5f),
				  size.x, size.y);
}

/// Calculates the center of the rect
inline Point Rect::center() const
{
	return Point(x + static_cast<int>(w * 0.5f), y + static_cast<int>(h * 0.5f));
}

/// Sets rect elements
inline void Rect::set(int const xx, int const yy, int const ww, int const hh)
{
	x = xx;
	y = yy;
	w = ww;
	h = hh;
}

/// Retains size but move to another position
inline void Rect::setCenter(const Point &center)
{
	x = center.x - static_cast<int>(w * 0.5f);
	y = center.y - static_cast<int>(h * 0.5f);
}

/// Equality operators
inline bool Rect::operator==(const Rect &rect) const
{
	return (x == rect.x && y == rect.y &&
			w == rect.w && h == rect.h);
}

}

#endif
