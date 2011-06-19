#ifndef CLASS_POINT
#define CLASS_POINT

/// A point in a two dimensional space
class Point
{
protected:
	int m_x, m_y;
public:
	/// Default constructor, all zeros
	Point() : m_x(0), m_y(0) { }
	/// Construct from elements
	Point(int const x, int const y) : m_x(x), m_y(y) { }

	// Property methods
	inline int X() const { return m_x; }
	inline void SetX(int x) { m_x = x; }
	inline int Y() const  {return m_y; }
	inline void SetY(int y) { m_y = y; }

	/// Set null every element
	inline void SetNull() {
		m_x = 0;
		m_y = 0;
	}
	/// Set point elements
	inline void Set(int const x, int const y) {
		m_x = x;
		m_y = y;
	}

	// Operators
	inline bool operator==(const Point& point) const {
		return (m_x == point.m_x &&	m_y == point.m_y);
	}

	friend void print(Point& point) {
		printf("%i %i", point.m_x, point.m_y);
	}
};

#endif
