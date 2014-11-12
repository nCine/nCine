#ifndef CLASS_NCINE_FONT
#define CLASS_NCINE_FONT

#include "IFile.h"
#include "Point.h"

namespace ncine {

class FontGlyph;
class Texture;

/// A class holding every information needed to correctly render text
class Font
{
  public:
	// Constructs a font class from a texture and a FNT file (from AngelCode's Bitmap Font Generator)
	Font(const char* texFilename, const char *fntFilename);
	virtual ~Font();

	/// Gets the texture object
	inline const Texture* texture() const { return texture_; }
	/// Sets the texture object
	inline void setTexture(Texture *texture) { texture_ = texture; }

	/// Returns font line height
	inline unsigned short lineHeight() const { return lineHeight_; }
	/// Returns font base
	inline unsigned short base() const { return base_; }
	/// Returns texture atlas size
	inline Point textureSize() const { return Point(width_, height_); }
	/// Returns number of glyphs
	inline unsigned short numGlyphs() const { return numGlyphs_; }
	/// Reurns a constant pointer to a glyph
	const FontGlyph* glyph(unsigned int glyphId) const;

  private:
	/// The font texture
	Texture *texture_;
	/// Font line height
	unsigned int lineHeight_;
	/// Font base
	unsigned int base_;
	/// Texture atlas width
	unsigned int width_;
	/// Texture atlas height
	unsigned int height_;
	/// Number of glyphs for this font
	unsigned int numGlyphs_;
	/// Number of kernings for this font
	unsigned int numKernings_;

	/// Maximum number of glyphs in a font
	static const unsigned int MaxGlyphs = 256;
	/// Array of font glyphs
	FontGlyph *glyphs_;

	/// Preventing construction by copy
	Font(const Font& other);

	// Loads an AngelCode's FNT file in a RAM buffer then parses it
	void parseFntFile(IFile *fileHandle);
};

}

#endif
