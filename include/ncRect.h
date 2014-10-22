#ifndef CLASS_NCRECT
#define CLASS_NCRECT

#include "ncPoint.h"

/// A rectangle in a two dimensional space
class ncRect
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
	ncRect() : x(0), y(0), w(0), h(0) { }
	/// Constructs from elements
	ncRect(int const xx, int const yy, int const ww, int const hh)
		: x(xx), y(yy), w(ww), h(hh) { }
	/// Constructs from base classes
	ncRect(const ncPoint &p1, const ncPoint &p2)
		: x(p1.x), y(p1.y), w(p2.x), h(p2.y) { }

	// Creates a rect from center and size
	static ncRect fromCenterAndSize(const ncPoint &center, const ncPoint &size);

	// Calculates the center of the rect
	ncPoint center() const;
	// Sets rect elements
	void set(int const xx, int const yy, int const ww, int const hh);
	// Retains size but move to another position
	void setCenter(const ncPoint& center);

	/// Eqality operators
	bool operator==(const ncRect& rect) const;
};

/// Creates a rect from center and size
inline ncRect ncRect::fromCenterAndSize(const ncPoint &center, const ncPoint &size)
{
	return ncRect(center.x - static_cast<int>(size.x * 0.5f),
				  center.y - static_cast<int>(size.y * 0.5f),
				  size.x, size.y);
}

/// Calculates the center of the rect
inline ncPoint ncRect::center() const
{
	return ncPoint(x + static_cast<int>(w * 0.5f), y + static_cast<int>(h * 0.5f));
}

/// Sets rect elements
inline void ncRect::set(int const xx, int const yy, int const ww, int const hh)
{
	x = xx;
	y = yy;
	w = ww;
	h = hh;
}

/// Retains size but move to another position
inline void ncRect::setCenter(const ncPoint &center)
{
	x = center.x - static_cast<int>(w * 0.5f);
	y = center.y - static_cast<int>(h * 0.5f);
}

/// Equality operators
inline bool ncRect::operator==(const ncRect &rect) const
{
	return (x == rect.x && y == rect.y &&
			w == rect.w && h == rect.h);
}

#endif
