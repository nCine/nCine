#ifndef CLASS_NCFONTGLYPH
#define CLASS_NCFONTGLYPH

#include "ncArray.h"
#include "ncPoint.h"
#include "ncRect.h"

/// A class holding information about a single glyph (character)
class ncFontGlyph
{
  public:
	ncFontGlyph();
	ncFontGlyph(unsigned short x, unsigned short y, unsigned short width, unsigned short height,
				short xOffset, short yOffset, unsigned short xAdvance);

	/// Sets glyph properties
	void set(unsigned short x, unsigned short y, unsigned short width, unsigned short height,
			 short xOffset, short yOffset, unsigned short xAdvance);

	/// Return the size in pixels of a glyph
	inline ncPoint size() const { return ncPoint(width_, height_); }
	/// Return the texture rectangle of a glyph
	inline ncRect texRect() const { return ncRect(x_, y_, width_, height_); }
	/// Return the X and Y offset of a glyph
	inline ncPoint offset() const { return ncPoint(xOffset_, yOffset_); }
	/// Return the X offset to advance in order to start rendering the next glyph
	inline unsigned short xAdvance() const { return xAdvance_; }

	/// Add kerning amount for a subsequent glyph
	inline void addKerning(int secondGlyph, int amount) { kernings_.insertBack(ncKerning(secondGlyph, amount)); }
	// Return the kerning amount for a subsequent glyph
	int kerning(int secondGlyph) const;

  private:
	/// A structure holding glyph pairs kerning offstes
	struct ncKerning
	{
		int secondGlyph_;
		int amount_;

		ncKerning() : secondGlyph_(0), amount_(0) { }
		ncKerning(int secondGlyph, int amount)
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
	ncArray<ncKerning> kernings_;
};

/// Sets glyph properties
inline void ncFontGlyph::set(unsigned short x, unsigned short y, unsigned short width, unsigned short height,
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

#endif
