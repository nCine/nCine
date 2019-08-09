#ifndef CLASS_NCINE_FONT
#define CLASS_NCINE_FONT

#include "Object.h"
#include "Vector2.h"

namespace ncine {

class FntParser;
class FontGlyph;
class Texture;

/// A class holding every information needed to correctly render text
class DLL_PUBLIC Font : public Object
{
  public:
	/// Depending on the glyph channel a different shader will be used
	enum RenderMode
	{
		GLYPH_IN_RED,
		GLYPH_IN_ALPHA
	};

	/// Constructs the object from an AngelCode's `FNT` file
	explicit Font(const char *fntFilename);
	/// Constructs the object from a texture and an AngelCode's `FNT` file
	Font(const char *texFilename, const char *fntFilename);
	~Font() override;

	/// Gets the texture object
	inline const Texture *texture() const { return texture_.get(); }

	/// Returns font line height
	inline unsigned int lineHeight() const { return lineHeight_; }
	/// Returns font base
	inline unsigned int base() const { return base_; }
	/// Returns texture atlas size
	inline Vector2i textureSize() const { return Vector2i(width_, height_); }
	/// Returns number of glyphs
	inline unsigned int numGlyphs() const { return numGlyphs_; }
	/// Returns number of kerning pairs
	inline unsigned int numKernings() const { return numKernings_; }
	/// Returns a constant pointer to a glyph
	const FontGlyph *glyph(unsigned int glyphId) const;

	inline RenderMode renderMode() const { return renderMode_; }

	inline static ObjectType sType() { return ObjectType::FONT; }

  private:
	/// The FNT file parser associated with this font
	nctl::UniquePtr<FntParser> fntParser_;

	/// The font texture
	nctl::UniquePtr<Texture> texture_;
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
	nctl::UniquePtr<FontGlyph[]> glyphs_;

	RenderMode renderMode_;

	/// Deleted copy constructor
	Font(const Font &) = delete;
	/// Deleted assignment operator
	Font &operator=(const Font &) = delete;

	/// Retrieves font information from the FNT parser
	void retrieveInfoFromFnt();

	/// Checks whether the FNT information are compatible with rendering or not
	void checkFntInformation();
};

}

#endif
