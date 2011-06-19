#ifndef CLASS_SIZE
#define CLASS_SIZE

/// A class representing the integer size of a two dimensional rectangle
class Size
{
protected:
	int m_w, m_h;
public:
	/// Default constructor, all zeros
	Size() : m_w(0), m_h(0) { }
	/// Construct from elements
	Size(int const w, int const h) : m_w(w), m_h(h) { }

	// Property methods
	inline int Width() const {return m_w;}
	inline void SetWidth(int w) {m_w = w;}
	inline int Height() const {return m_h;}
	inline void SetHeight(int h) {m_h = h;}

	/// Set null every element
	inline void SetNull() {
		m_w = 0;
		m_h = 0;
	}
	/// Set size elements
	inline void Set(int const w, int const h) {
		m_w = w;
		m_h = h;
	}

	// Operators
	inline bool operator==(const Size& size) const {
		return (m_w == size.m_w && m_h == size.m_h);
	}

	friend void print(Size& size) {
		printf("%i %i", size.m_w, size.m_h);
	}
};

#endif
