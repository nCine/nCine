#ifndef CLASS_NCFONTGLYPH
#define CLASS_NCFONTGLYPH

#include "ncArray.h"
#include "ncPoint.h"
#include "ncRect.h"

/// A class holding information about a single glyph (character)
class ncFontGlyph
{
private:
	/// A structure holding glyph pairs kerning offstes
	struct ncKerning {
		int m_iSecondGlyph;
		int m_iAmount;

		ncKerning() : m_iSecondGlyph(0), m_iAmount(0) { }
		ncKerning(int iSecondGlyph, int iAmount)
		{
			m_iSecondGlyph = iSecondGlyph;
			m_iAmount = iAmount;
		}
	};

	unsigned short m_uX;
	unsigned short m_uY;
	unsigned short m_uWidth;
	unsigned short m_uHeight;
	short m_iXOffset;
	short m_iYOffset;
	unsigned short m_uXAdvance;
	ncArray<ncKerning> m_vKernings;

public:
	ncFontGlyph()
		: m_uX(0), m_uY(0), m_uWidth(0), m_uHeight(0), m_iXOffset(0),
		  m_iYOffset(0), m_uXAdvance(0), m_vKernings(4) { }
	ncFontGlyph(unsigned short uX, unsigned short uY, unsigned short uWidth, unsigned short uHeight,
		short iXOffset, short iYOffset, unsigned short uXAdvance)
		: m_uX(uX), m_uY(uY), m_uWidth(uWidth), m_uHeight(uHeight),
		  m_iXOffset(iXOffset), m_iYOffset(iYOffset), m_uXAdvance(uXAdvance),
		  m_vKernings(4) { }

	void Set(unsigned short uX, unsigned short uY, unsigned short uWidth, unsigned short uHeight,
		short iXOffset, short iYOffset, unsigned short uXAdvance)
	{
		m_uX = uX;
		m_uY = uY;
		m_uWidth = uWidth;
		m_uHeight = uHeight;
		m_iXOffset = iXOffset;
		m_iYOffset = iYOffset;
		m_uXAdvance = uXAdvance;
	}

	/// Return the size in pixels of a glyph
	inline ncPoint Size() const { return ncPoint(m_uWidth, m_uHeight); }
	/// Return the texture rectangle of a glyph
	inline ncRect TexRect() const { return ncRect(m_uX, m_uY, m_uWidth, m_uHeight); }
	/// Return the X and Y offset of a glyph
	inline ncPoint Offset() const { return ncPoint(m_iXOffset, m_iYOffset); }
	/// Return the X offset to advance in order to start rendering the next glyph
	inline unsigned short XAdvance() const { return m_uXAdvance; }

	/// Add kerning amount for a subsequent glyph
	inline void AddKerning(int iSecondGlyph, int iAmount) { m_vKernings.InsertBack(ncKerning(iSecondGlyph, iAmount)); }
	// Return the kerning amount for a subsequent glyph
	int Kerning(int iSecondGlyph) const;
};

#endif
