#ifndef CLASS_NCINE_FONTGLYPH
#define CLASS_NCINE_FONTGLYPH

#include "Array.h"
#include "Point.h"
#include "Rect.h"

namespace ncine {

/// A class holding information about a single glyph (character)
class FontGlyph
{
  public:
	FontGlyph();
	FontGlyph(unsigned short x, unsigned short y, unsigned short width, unsigned short height,
				short xOffset, short yOffset, unsigned short xAdvance);

	/// Sets glyph properties
	void set(unsigned short x, unsigned short y, unsigned short width, unsigned short height,
			 short xOffset, short yOffset, unsigned short xAdvance);

	/// Return the size in pixels of a glyph
	inline Point size() const { return Point(width_, height_); }
	/// Return the texture rectangle of a glyph
	inline Rect texRect() const { return Rect(x_, y_, width_, height_); }
	/// Return the X and Y offset of a glyph
	inline Point offset() const { return Point(xOffset_, yOffset_); }
	/// Return the X offset to advance in order to start rendering the next glyph
	inline unsigned short xAdvance() const { return xAdvance_; }

	/// Add kerning amount for a subsequent glyph
	inline void addKerning(int secondGlyph, int amount) { kernings_.insertBack(Kerning(secondGlyph, amount)); }
	// Return the kerning amount for a subsequent glyph
	int kerning(int secondGlyph) const;

  private:
	/// A structure holding glyph pairs kerning offstes
	struct Kerning
	{
		int secondGlyph_;
		int amount_;

		Kerning() : secondGlyph_(0), amount_(0) { }
		Kerning(int secondGlyph, int amount)
		{
			secondGlyph_ = secondGlyph;
			amount_ = amount;
		}
	};

	unsigned short x_;
	unsigned short y_;
	unsigned short width_;
	unsigned short height_;
	short xOffset_;
	short yOffset_;
	unsigned short xAdvance_;
	Array<Kerning> kernings_;
};

/// Sets glyph properties
inline void FontGlyph::set(unsigned short x, unsigned short y, unsigned short width, unsigned short height,
	short xOffset, short yOffset, unsigned short xAdvance)
{
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;
	xOffset_ = xOffset;
	yOffset_ = yOffset;
	xAdvance_ = xAdvance;
}

}

#endif
