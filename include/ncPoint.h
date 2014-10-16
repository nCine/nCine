#ifndef CLASS_NCPOINT
#define CLASS_NCPOINT

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
	/// Constructs from elements
	ncPoint(int const xx, int const yy) : x(xx), y(yy) { }

	// Sets point elements
	void set(int const xx, int const yy);

	// Equality operator
	bool operator==(const ncPoint& point) const;
};

/// Sets point elements
inline void ncPoint::set(int const xx, int const yy)
{
	x = xx;
	y = yy;
}

/// Equality operator
inline bool ncPoint::operator==(const ncPoint& point) const
{
	return (x == point.x && y == point.y);
}

#endif
