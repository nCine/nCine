#ifndef CLASS_NCINE_FONTGLYPH
#define CLASS_NCINE_FONTGLYPH

#include "Array.h"
#include "Rect.h"

namespace ncine {

/// A class holding information about a single glyph (character)
class FontGlyph
{
  public:
	FontGlyph();
	FontGlyph(unsigned int x, unsigned int y, unsigned int width, unsigned int height,
				int xOffset, int yOffset, unsigned int xAdvance);

	/// Sets glyph properties
	void set(unsigned int x, unsigned int y, unsigned int width, unsigned int height,
			 int xOffset, int yOffset, unsigned int xAdvance);

	/// Returns the size in pixels of a glyph
	inline Vector2i size() const { return Vector2i(width_, height_); }
	/// Returns the texture rectangle of a glyph
	inline Recti texRect() const { return Recti(x_, y_, width_, height_); }
	/// Returns the X and Y offset of a glyph
	inline Vector2i offset() const { return Vector2i(xOffset_, yOffset_); }
	/// Returns the X offset to advance in order to start rendering the next glyph
	inline unsigned int xAdvance() const { return xAdvance_; }

	/// Adds the kerning amount for a subsequent glyph
	inline void addKerning(int secondGlyph, int amount) { kernings_.pushBack(Kerning(secondGlyph, amount)); }
	/// Returns the kerning amount for a subsequent glyph
	int kerning(int secondGlyph) const;

  private:
	/// A structure holding glyph pairs kerning offsets
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

	unsigned int x_;
	unsigned int y_;
	unsigned int width_;
	unsigned int height_;
	int xOffset_;
	int yOffset_;
	unsigned int xAdvance_;
	Array<Kerning> kernings_;
};

inline void FontGlyph::set(unsigned int x, unsigned int y, unsigned int width, unsigned int height,
	int xOffset, int yOffset, unsigned int xAdvance)
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
