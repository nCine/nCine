#ifndef CLASS_POINT
#define CLASS_POINT

#include <cstdio> // for printf()

/// A point in a two dimensional space
class Point
{
public:
	/// X coordinate as a public property
	int x;
	/// Y coordinate as a public property
	int y;
	/// Default constructor, all zeros
	Point() : x(0), y(0) { }
	/// Construct from elements
	Point(int const iX, int const iY) : x(iX), y(iY) { }

	/// Set point elements
	inline void Set(int const iX, int const iY) {
		x = iX;
		y = iY;
	}

	/// Equality operator
	inline bool operator==(const Point& point) const {
		return (x == point.x && y == point.y);
	}

	friend void print(Point& point) {
		printf("%i %i", point.x, point.y);
	}
};

#endif
