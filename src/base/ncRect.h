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
		/// Construct from elements
		ncRect(int const iX, int const iY, int const iW, int const iH)
			: x(iX), y(iY), w(iW), h(iH) { }
		/// Construct from base classes
		ncRect(ncPoint const p1, ncPoint const p2)
			: x(p1.x), y(p1.y), w(p2.x), h(p2.y) { }

		/// Set rect elements
		inline void Set(int const iX, int const iY, int const iW, int const iH) {
			x = iX;
			y = iY;
			w = iW;
			h = iH;
		}

		// Calculate the center of the rect
		inline ncPoint Center() const {
			return ncPoint(x + w*0.5f, y + h*0.5f);
		}

		// Operators
		inline bool operator==(const ncRect& rect) const {
			return (x == rect.x && y == rect.y &&
					w == rect.w && h == rect.h);
		}

		friend void print(ncRect& rect) {
			printf("%i %i %i %i", rect.x, rect.y, rect.w, rect.h);
		}
};

#endif
