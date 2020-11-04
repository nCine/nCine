#ifndef CLASS_NCINE_FONT
#define CLASS_NCINE_FONT

#include "Object.h"
#include "Vector2.h"
#include <nctl/HashMap.h>

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

	/// Constructs an empty font object with no glyphs nor kerning pairs
	Font();

	/// Constructs the object from an AngelCode's `FNT` memory buffer and a texture
	Font(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize, const char *texFilename);
	/// Constructs the object from an AngelCode's `FNT` memory buffer and a texture memory buffer
	Font(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize,
	     const char *texBufferName, const unsigned char *texBufferPtr, unsigned long int texBufferSize);

	/// Constructs the object from an AngelCode's `FNT` file
	explicit Font(const char *fntFilename);
	/// Constructs the object from an AngelCode's `FNT` file and a texture file
	Font(const char *fntFilename, const char *texFilename);

	~Font() override;

	bool loadFromMemory(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize, const char *texFilename);
	bool loadFromMemory(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize,
	                    const char *texBufferName, const unsigned char *texBufferPtr, unsigned long int texBufferSize);
	bool loadFromFile(const char *fntFilename);
	bool loadFromFile(const char *fntFilename, const char *texFilename);

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

	/// Maximum number of font glyphs stored in the array (single byte UTF-8 characters)
	static const unsigned int GlyphArraySize = 256;
	/// Array of font glyphs encoded in a single UTF-8 code unit
	nctl::UniquePtr<FontGlyph[]> glyphArray_;
	/// Maximum number of font glyphs stored in the hashmap (multi-byte UTF-8 characters)
	static const unsigned int GlyphHashmapSize = 1024;
	/// Hashmap of font glyphs encoded in more than one UTF-8 code unit
	nctl::HashMap<unsigned short int, FontGlyph> glyphHashMap_;

	RenderMode renderMode_;

	/// Deleted copy constructor
	Font(const Font &) = delete;
	/// Deleted assignment operator
	Font &operator=(const Font &) = delete;

	/// Checks whether the FNT information are compatible with rendering or not
	bool checkFntInformation(const FntParser &fntParser);

	/// Determines the render mode based on the FNT information
	void determineRenderMode(const FntParser &fntParser);
	/// Retrieves font information from the FNT parser
	void retrieveInfoFromFnt(const FntParser &fntParser);
};

}

#endif
