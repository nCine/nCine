#ifndef CLASS_RECT
#define CLASS_RECT

#include "Point.h"
#include "Size.h"

/// A rectangle in a two dimensional space
class Rect
{
	protected:
		int m_x, m_y, m_w, m_h;
	public:
		/// Default constructor, all zeros
		Rect() : m_x(0), m_y(0), m_w(0), m_h(0) { }
		/// Construct from elements
		Rect(int const x, int const y, int const w, int const h)
			: m_x(x), m_y(y), m_w(w), m_h(h) { }
		/// Construct from base classes
		Rect(Point const p, Size const s)
			: m_x(p.X()), m_y(p.Y()), m_w(s.Width()), m_h(s.Height()) { }

		// Property methods
		inline int X() const {return m_x;}
		inline void SetX(int x) {m_x = x;}
		inline int Y() const {return m_y;}
		inline void SetY(int y) {m_y = y;}
		inline int Width() const {return m_w;}
		inline void SetWidth(int w) {m_w = w;}
		inline int Height() const {return m_h;}
		inline void SetHeight(int h) {m_h = h;}

		/// Set null every element
		inline void SetNull() {
			m_x = 0;
			m_y = 0;
			m_w = 0;
			m_h = 0;
		};
		/// Set rect elements
		inline void Set(int const x, int const y, int const w, int const h) {
			m_x = x;
			m_y = y;
			m_w = w;
			m_h = h;
		}

		// Calculate the center of the rect
		inline Point Center() const {
			return Point(m_x + m_w*0.5f, m_y + m_h*0.5f);
		}

		// Operators
		inline bool operator==(const Rect& rect) const {
			return (m_x == rect.m_x &&
					m_y == rect.m_y &&
					m_w == rect.m_w &&
					m_h == rect.m_h);
		}

		friend void print(Rect& rect) {
			printf("%i %i %i %i", rect.m_x, rect.m_y, rect.m_w,rect.m_h);
		}
};

#endif
