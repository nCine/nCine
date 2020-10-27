#include "common_macros.h"
#include <nctl/CString.h>
#include "Font.h"
#include "FntParser.h"
#include "FontGlyph.h"
#include "Texture.h"
#include "FileSystem.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! \note The specified texture will override the one in the FNT file */
Font::Font(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize, const char *texFilename)
    : Object(ObjectType::FONT, fntBufferName),
      texture_(nctl::makeUnique<Texture>(texFilename)),
      lineHeight_(0), base_(0), width_(0), height_(0), numGlyphs_(0), numKernings_(0),
      glyphArray_(nctl::makeUnique<FontGlyph[]>(GlyphArraySize)),
      glyphHashMap_(GlyphHashmapSize), renderMode_(RenderMode::GLYPH_IN_RED)
{
	ZoneScoped;
	ZoneText(fntBufferName, nctl::strnlen(fntBufferName, nctl::String::MaxCStringLength));

	FntParser fntParser(reinterpret_cast<const char *>(fntBufferPtr), fntBufferSize);
	retrieveInfoFromFnt(fntParser);
	checkFntInformation(fntParser);
}

/*! \note The specified texture will override the one in the FNT file */
Font::Font(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize,
           const char *texBufferName, const unsigned char *texBufferPtr, unsigned long int texBufferSize)
    : Object(ObjectType::FONT, fntBufferName),
      texture_(nctl::makeUnique<Texture>(texBufferName, texBufferPtr, texBufferSize)),
      lineHeight_(0), base_(0), width_(0), height_(0), numGlyphs_(0), numKernings_(0),
      glyphArray_(nctl::makeUnique<FontGlyph[]>(GlyphArraySize)),
      glyphHashMap_(GlyphHashmapSize), renderMode_(RenderMode::GLYPH_IN_RED)
{
	ZoneScoped;
	ZoneText(fntBufferName, nctl::strnlen(fntBufferName, nctl::String::MaxCStringLength));

	FntParser fntParser(reinterpret_cast<const char *>(fntBufferPtr), fntBufferSize);
	retrieveInfoFromFnt(fntParser);
	checkFntInformation(fntParser);
}

/*! \note The texture specified by the FNT file will be automatically loaded */
Font::Font(const char *fntFilename)
    : Object(ObjectType::FONT, fntFilename),
      lineHeight_(0), base_(0), width_(0), height_(0), numGlyphs_(0), numKernings_(0),
      glyphArray_(nctl::makeUnique<FontGlyph[]>(GlyphArraySize)),
      glyphHashMap_(GlyphHashmapSize), renderMode_(RenderMode::GLYPH_IN_RED)
{
	ZoneScoped;
	ZoneText(fntFilename, nctl::strnlen(fntFilename, nctl::String::MaxCStringLength));

	FntParser fntParser(fntFilename);
	retrieveInfoFromFnt(fntParser);

	nctl::String dirName = fs::dirName(fntFilename);
	nctl::String texFilename = fs::absoluteJoinPath(dirName, fntParser.pageTag(0).file);
	texture_ = nctl::makeUnique<Texture>(texFilename.data());
	checkFntInformation(fntParser);
}

/*! \note The specified texture will override the one in the FNT file */
Font::Font(const char *fntFilename, const char *texFilename)
    : Object(ObjectType::FONT, fntFilename),
      texture_(nctl::makeUnique<Texture>(texFilename)),
      lineHeight_(0), base_(0), width_(0), height_(0), numGlyphs_(0), numKernings_(0),
      glyphArray_(nctl::makeUnique<FontGlyph[]>(GlyphArraySize)),
      glyphHashMap_(GlyphHashmapSize), renderMode_(RenderMode::GLYPH_IN_RED)
{
	ZoneScoped;
	ZoneText(fntFilename, nctl::strnlen(fntFilename, nctl::String::MaxCStringLength));

	FntParser fntParser(fntFilename);
	retrieveInfoFromFnt(fntParser);
	checkFntInformation(fntParser);
}

Font::~Font()
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const FontGlyph *Font::glyph(unsigned int glyphId) const
{
	if (glyphId < GlyphArraySize)
		return &glyphArray_[glyphId];
	else
		return glyphHashMap_.find(glyphId);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Font::retrieveInfoFromFnt(const FntParser &fntParser)
{
	const FntParser::CommonTag &commonTag = fntParser.commonTag();

	lineHeight_ = static_cast<unsigned int>(commonTag.lineHeight);
	base_ = static_cast<unsigned int>(commonTag.base);
	width_ = static_cast<unsigned int>(commonTag.scaleW);
	height_ = static_cast<unsigned int>(commonTag.scaleH);

	const unsigned int numChars = (fntParser.numCharTags() < GlyphArraySize + GlyphHashmapSize) ? fntParser.numCharTags() : GlyphArraySize + GlyphHashmapSize;
	for (unsigned int i = 0; i < numChars; i++)
	{
		const FntParser::CharTag &charTag = fntParser.charTag(i);
		if (charTag.id < static_cast<int>(GlyphArraySize))
			glyphArray_[charTag.id].set(charTag.x, charTag.y, charTag.width, charTag.height, charTag.xoffset, charTag.yoffset, charTag.xadvance);
		else
			glyphHashMap_.emplace(charTag.id, charTag.x, charTag.y, charTag.width, charTag.height, charTag.xoffset, charTag.yoffset, charTag.xadvance);
		numGlyphs_++;
	}

	for (unsigned int i = 0; i < fntParser.numKerningTags(); i++)
	{
		const FntParser::KerningTag &kerningTag = fntParser.kerningTag(i);
		if (kerningTag.first < static_cast<int>(GlyphArraySize))
			glyphArray_[kerningTag.first].addKerning(kerningTag.second, kerningTag.amount);
		else
			glyphHashMap_[kerningTag.first].addKerning(kerningTag.second, kerningTag.amount);
		numKernings_++;
	}

	LOGI_X("FNT file information retrieved: %u glyphs and %u kernings", numGlyphs_, numKernings_);
}

void Font::checkFntInformation(const FntParser &fntParser)
{
	const FntParser::InfoTag &infoTag = fntParser.infoTag();
	FATAL_ASSERT_MSG_X(infoTag.outline == 0, "Font outline is not supported");

	const FntParser::CommonTag &commonTag = fntParser.commonTag();
	FATAL_ASSERT_MSG_X(commonTag.pages == 1, "Multiple texture pages are not supported (pages: %d)", commonTag.pages);
	FATAL_ASSERT_MSG(commonTag.packed == false, "Characters packed into each of the texture channels are not supported");

	if (texture_)
	{
		FATAL_ASSERT_MSG_X(commonTag.scaleW == texture_->width(), "Texture width is different than FNT scale width: %u instead of %u", texture_->width(), commonTag.scaleW);
		FATAL_ASSERT_MSG_X(commonTag.scaleH == texture_->height(), "Texture height is different than FNT scale height: %u instead of %u", texture_->height(), commonTag.scaleH);

		if (texture_->numChannels() == 1)
		{
			FATAL_ASSERT_MSG(commonTag.alphaChnl == FntParser::ChannelData::GLYPH ||
			                 commonTag.alphaChnl == FntParser::ChannelData::OUTLINE ||
			                 commonTag.alphaChnl == FntParser::ChannelData::MISSING,
			                 "Texture has one channel only but it does not contain glyph data");
			// One channel textures have only the red channel in OpenGL
			renderMode_ = RenderMode::GLYPH_IN_RED;
		}
		else if (texture_->numChannels() == 4)
		{
			if (commonTag.redChnl == FntParser::ChannelData::MISSING && commonTag.alphaChnl == FntParser::ChannelData::MISSING)
				renderMode_ = RenderMode::GLYPH_IN_ALPHA;
			else
			{
				FATAL_ASSERT_MSG(commonTag.redChnl == FntParser::ChannelData::GLYPH || commonTag.alphaChnl == FntParser::ChannelData::GLYPH,
				                 "Texture has four channels but neither red nor alpha channel contain glyph data");

				if (commonTag.redChnl == FntParser::ChannelData::GLYPH)
					renderMode_ = RenderMode::GLYPH_IN_RED;
				else if (commonTag.alphaChnl == FntParser::ChannelData::GLYPH)
					renderMode_ = RenderMode::GLYPH_IN_ALPHA;
			}
		}
	}
}

}
