#ifndef CLASS_RECT
#define CLASS_RECT

#include "Point.h"

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
		/// Construct from elements
		Rect(int const iX, int const iY, int const iW, int const iH)
			: x(iX), y(iY), w(iW), h(iH) { }
		/// Construct from base classes
		Rect(Point const p1, Point const p2)
			: x(p1.x), y(p1.y), w(p2.x), h(p2.y) { }

		/// Set rect elements
		inline void Set(int const iX, int const iY, int const iW, int const iH) {
			x = iX;
			y = iY;
			w = iW;
			h = iH;
		}

		// Calculate the center of the rect
		inline Point Center() const {
			return Point(x + w*0.5f, y + h*0.5f);
		}

		// Operators
		inline bool operator==(const Rect& rect) const {
			return (x == rect.x && y == rect.y &&
					w == rect.w && h == rect.h);
		}

		friend void print(Rect& rect) {
			printf("%i %i %i %i", rect.x, rect.y, rect.w, rect.h);
		}
};

#endif
