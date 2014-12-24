#ifndef CLASS_NCINE_POINT
#define CLASS_NCINE_POINT

namespace ncine {

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
	/// Constructs from elements
	Point(int const xx, int const yy) : x(xx), y(yy) { }

	// Sets point elements
	void set(int const xx, int const yy);

	// Equality operator
	bool operator==(const Point& point) const;
};

/// Sets point elements
inline void Point::set(int const xx, int const yy)
{
	x = xx;
	y = yy;
}

/// Equality operator
inline bool Point::operator==(const Point& point) const
{
	return (x == point.x && y == point.y);
}

}

#endif
