#ifndef CLASS_NCPOINT
#define CLASS_NCPOINT

#include <cstdio> // for printf()

/// A point in a two dimensional space
class ncPoint
{
public:
	/// X coordinate as a public property
	int x;
	/// Y coordinate as a public property
	int y;
	/// Default constructor, all zeros
	ncPoint() : x(0), y(0) { }
	/// Construct from elements
	ncPoint(int const iX, int const iY) : x(iX), y(iY) { }

	/// Set point elements
	inline void Set(int const iX, int const iY) {
		x = iX;
		y = iY;
	}

	/// Equality operator
	inline bool operator==(const ncPoint& point) const {
		return (x == point.x && y == point.y);
	}

	void print(ncPoint& point) {
		printf("%i %i", point.x, point.y);
	}
};

#endif
