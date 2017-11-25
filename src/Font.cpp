#include <cstring>
#include "common_macros.h"
#include "Font.h"
#include "FontGlyph.h"
#include "IFile.h"
#include "Texture.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Font::Font(const char *texFilename, const char *fntFilename)
	: Object(ObjectType::FONT, fntFilename),
	  texture_(nullptr), lineHeight_(0), base_(0), width_(0), height_(0),
	  numGlyphs_(0), numKernings_(0), glyphs_(nullptr)
{
	texture_ = new Texture(texFilename);
	glyphs_ = new FontGlyph[MaxGlyphs];

	IFile *fileHandle = IFile::createFileHandle(fntFilename);
	fileHandle->open(IFile::OpenMode::READ);
	parseFntFile(fileHandle);
	delete fileHandle;
}

Font::~Font()
{
	delete[] glyphs_;
	delete texture_;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Font::setTexture(Texture *texture)
{
	ASSERT(texture);
	texture_ = texture;
}

const FontGlyph *Font::glyph(unsigned int glyphId) const
{
	ASSERT(glyphId < MaxGlyphs);
	return &glyphs_[glyphId];
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Font::parseFntFile(IFile *fileHandle)
{
	int fileLine = 0;

	unsigned int glyphId;
	unsigned int x, y, width, height;
	int xOffset, yOffset;
	int xAdvance;
	unsigned int secondGlyphId;
	int kerningAmount;

	char *fileBuffer = new char[fileHandle->size()];
	fileHandle->read(fileBuffer, fileHandle->size());

	char *buffer = fileBuffer;
	do
	{
		fileLine++;

		// skipping entirely the "info" line
		if (strncmp(buffer, "info", 4) == 0)
			continue;
		else if (strncmp(buffer, "common", 6) == 0)
		{
			sscanf(buffer, "common lineHeight=%u base=%u scaleW=%u scaleH=%u", &lineHeight_, &base_, &width_, &height_);

			FATAL_ASSERT_MSG_X(static_cast<int>(width_) == texture_->width(), "FNT texture has a different width: %u", width_);
			FATAL_ASSERT_MSG_X(static_cast<int>(height_) == texture_->height(), "FNT texture has a different height: %u", height_);
		}
		// skipping entirely the "page" line
		else if (strncmp(buffer, "page", 4) == 0)
			continue;
		else if (strncmp(buffer, "chars", 5) == 0)
			sscanf(buffer, "chars count=%u", &numGlyphs_);
		else if (strncmp(buffer, "char", 4) == 0)
		{
			sscanf(buffer, "char id=%u x=%u y=%u width=%u height=%u xoffset=%d yoffset=%d xadvance=%d", &glyphId, &x, &y, &width, &height, &xOffset, &yOffset, &xAdvance);
			if (glyphId < MaxGlyphs)
				glyphs_[glyphId].set(x, y, width, height, xOffset, yOffset, xAdvance);
			else
			{
				LOGW_X("Skipping character id #%u because bigger than glyph array size (%u)", glyphId, MaxGlyphs);
				numGlyphs_--;
				continue;
			}
		}
		else if (strncmp(buffer, "kernings", 8) == 0)
			sscanf(buffer, "kernings count=%u", &numKernings_);
		else if (strncmp(buffer, "kerning", 7) == 0)
		{
			sscanf(buffer, "kerning first=%u second=%u amount=%d ", &glyphId, &secondGlyphId, &kerningAmount);
			if (glyphId < MaxGlyphs && secondGlyphId < MaxGlyphs)
				glyphs_[glyphId].addKerning(secondGlyphId, kerningAmount);
			else
			{
				LOGW_X("Skipping kerning couple (#%u, #%u) because bigger than glyph array size (%u)", glyphId, secondGlyphId, MaxGlyphs);
				numKernings_--;
				continue;
			}
		}
	}
	while (strchr(buffer, '\n') && (buffer = strchr(buffer, '\n') + 1) < fileBuffer + fileHandle->size());

	LOGI_X("FNT file parsed: %u glyphs and %u kernings", numGlyphs_, numKernings_);
	delete[] fileBuffer;
}

}
