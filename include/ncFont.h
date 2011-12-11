#ifndef CLASS_NCFONT
#define CLASS_NCFONT

#include "ncDrawableNode.h"
#include "ncTexture.h"
#include "ncRect.h"
#include "ncColor.h"

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
	ncFontGlyph() : m_vKernings(4) { }
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

/// A class holding every information needed to correctly render text
class ncFont
{
private:
	static const unsigned int MAX_GLYPHS = 256;

	/// The font texture
	ncTexture *m_pTexture;
	/// Font line height
	unsigned int m_uLineHeight;
	/// Font base
	unsigned int m_uBase;
	/// Texture atlas width
	unsigned int m_uWidth;
	/// Texture atlas height
	unsigned int m_uHeight;
	/// Number of glyphs for this font
	unsigned int m_uNumGlyphs;
	/// Number of kernings for this font
	unsigned int m_uNumKernings;
	/// Array of font glyphs
	ncFontGlyph m_vGlyphs[MAX_GLYPHS];

	// Parse an AngelCode's FNT file
	void ParseFNTFile(FILE *pFile);;

public:
	// Constructs a font class from a texture and a FNT file (from AngelCode's Bitmap Font Generator)
	ncFont(const char* pTexFilename, const char *pFntFilename);
	virtual ~ncFont() { delete m_pTexture; }

	/// Gets the texture object
	inline const ncTexture* Texture() { return m_pTexture; }
	/// Sets the texture object
	inline void SetTexture(ncTexture *pTexture) { m_pTexture = pTexture; }

	/// Returns font line height
	inline unsigned short LineHeight() const { return m_uLineHeight; }
	/// Returns font base
	inline unsigned short Base() const { return m_uBase; }
	/// Returns texture atlas size
	inline ncPoint TexSize() const
	{
		return ncPoint(m_uWidth, m_uHeight);
	}
	/// Returns number of glyphs
	inline unsigned short NumGlyphs() const { return m_uNumGlyphs; }
	/// Returns a constant pointer to a glyph
	inline const ncFontGlyph* Glyph(int iGlyphId) const
	{
		if (iGlyphId < MAX_GLYPHS)
			return &m_vGlyphs[iGlyphId];
		else
			return NULL;
	}
};

#endif
