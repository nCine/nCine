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
	ncRect(int const iX, int const iY, int const iW, int const iH)
		: x(iX), y(iY), w(iW), h(iH) { }
	/// Constructs from base classes
	ncRect(const ncPoint &p1, const ncPoint &p2)
		: x(p1.x), y(p1.y), w(p2.x), h(p2.y) { }

	// Creates a rect from center and size
	static ncRect FromCenterAndSize(const ncPoint &center, const ncPoint &size);

	// Calculates the center of the rect
	ncPoint Center() const;
	// Sets rect elements
	void Set(int const iX, int const iY, int const iW, int const iH);
	// Retains size but move to another position
	void SetCenter(const ncPoint& center);

	/// Eqality operators
	bool operator==(const ncRect& rect) const;
};

/// Creates a rect from center and size
inline ncRect ncRect::FromCenterAndSize(const ncPoint &center, const ncPoint &size)
{
	return ncRect(center.x - size.x * 0.5f, center.y - size.y * 0.5f, size.x, size.y);
}

/// Calculates the center of the rect
inline ncPoint ncRect::Center() const
{
	return ncPoint(x + w * 0.5f, y + h * 0.5f);
}

/// Sets rect elements
inline void ncRect::Set(int const iX, int const iY, int const iW, int const iH)
{
	x = iX;
	y = iY;
	w = iW;
	h = iH;
}

/// Retains size but move to another position
inline void ncRect::SetCenter(const ncPoint &center)
{
	x = center.x - w * 0.5f;
	y = center.y - h * 0.5f;
}

/// Equality operators
inline bool ncRect::operator==(const ncRect &rect) const
{
	return (x == rect.x && y == rect.y && w == rect.w && h == rect.h);
}

#endif
