#include <cstring>
#include "common_macros.h"
#include "FntParser.h"
#include "IFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

FntParser::FntParser(const char *bufferPtr, unsigned long int bufferSize)
    : numPageTags_(0), numCharTags_(0), numKerningTags_(0)
{
	parseFntBuffer(bufferPtr, bufferSize);
}

FntParser::FntParser(const char *fntFilename)
    : numPageTags_(0), numCharTags_(0), numKerningTags_(0)
{
	nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(fntFilename);
	fileHandle->setExitOnFailToOpen(false);

	fileHandle->open(IFile::OpenMode::READ);
	if (fileHandle->isOpened() == false)
		return;

	const long int size = fileHandle->size();
	nctl::UniquePtr<char[]> fileBuffer = nctl::makeUnique<char[]>(size);
	fileHandle->read(fileBuffer.get(), size);

	parseFntBuffer(fileBuffer.get(), size);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void FntParser::parseFntBuffer(const char *buffer, unsigned long int size)
{
	char const * const bufferStart = buffer;

	do
	{
		if (strncmp(buffer, "info", 4) == 0)
			parseInfoTag(buffer);
		else if (strncmp(buffer, "common", 6) == 0)
			parseCommonTag(buffer);
		else if (strncmp(buffer, "page", 4) == 0 && numPageTags_ < MaxPageTags)
			parsePageTag(buffer, numPageTags_++);
		else if (strncmp(buffer, "chars", 5) == 0)
			parseCharsTag(buffer);
		else if (strncmp(buffer, "char", 4) == 0 && numCharTags_ < MaxCharTags)
			parseCharTag(buffer, numCharTags_++);
		else if (strncmp(buffer, "kernings", 8) == 0)
			parseKerningsTag(buffer);
		else if (strncmp(buffer, "kerning", 7) == 0 && numKerningTags_ < MaxKerningTags)
			parseKerningTag(buffer, numKerningTags_++);
	} while (strchr(buffer, '\n') && (buffer = strchr(buffer, '\n') + 1) < bufferStart + size);

	LOGI_X("FNT file parsed for \"%s\", size %d, texture %dx%d, : %u pages, %u characters, %u kernings", infoTag_.face.data(), infoTag_.size, commonTag_.scaleW, commonTag_.scaleH, numPageTags_, numCharTags_, numKerningTags_);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void FntParser::parseInfoTag(const char *buffer)
{
	int auxInt = 0;
	buffer = nextField(buffer);

	if (strncmp(buffer, "face", 4) == 0)
	{
		extractValueWithSpaces(buffer, infoTag_.face);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "size", 4) == 0)
	{
		sscanf(buffer, "size=%d", &infoTag_.size);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "bold", 4) == 0)
	{
		sscanf(buffer, "bold=%d", &auxInt);
		infoTag_.bold = auxInt ? true : false;
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "italic", 6) == 0)
	{
		sscanf(buffer, "italic=%d", &auxInt);
		infoTag_.italic = auxInt ? true : false;
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "charset", 7) == 0)
	{
		extractValueWithSpaces(buffer, infoTag_.charset);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "unicode", 7) == 0)
	{
		sscanf(buffer, "unicode=%d", &auxInt);
		infoTag_.unicode = auxInt ? true : false;
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "stretchH", 8) == 0)
	{
		sscanf(buffer, "stretchH=%d", &infoTag_.stretchH);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "smooth", 6) == 0)
	{
		sscanf(buffer, "smooth=%d", &auxInt);
		infoTag_.smooth = auxInt ? true : false;
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "aa", 2) == 0)
	{
		sscanf(buffer, "aa=%d", &infoTag_.aa);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "padding", 7) == 0)
	{
		sscanf(buffer, "padding=%d,%d,%d,%d", &infoTag_.paddingUp, &infoTag_.paddingRight, &infoTag_.paddingDown, &infoTag_.paddingLeft);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "spacing", 7) == 0)
	{
		sscanf(buffer, "spacing=%d,%d", &infoTag_.hSpacing, &infoTag_.vSpacing);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "outline", 7) == 0)
	{
		sscanf(buffer, "outline=%d", &infoTag_.outline);
		buffer = nextField(buffer);
	}
}

void FntParser::parseCommonTag(const char *buffer)
{
	int auxInt = 0;
	buffer = nextField(buffer);

	if (strncmp(buffer, "lineHeight", 10) == 0)
	{
		sscanf(buffer, "lineHeight=%d", &commonTag_.lineHeight);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "base", 4) == 0)
	{
		sscanf(buffer, "base=%d", &commonTag_.base);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "scaleW", 6) == 0)
	{
		sscanf(buffer, "scaleW=%d", &commonTag_.scaleW);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "scaleH", 6) == 0)
	{
		sscanf(buffer, "scaleH=%d", &commonTag_.scaleH);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "pages", 5) == 0)
	{
		sscanf(buffer, "pages=%d", &commonTag_.pages);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "packed", 6) == 0)
	{
		sscanf(buffer, "packed=%d", &auxInt);
		commonTag_.packed = auxInt ? true : false;
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "alphaChnl", 9) == 0)
	{
		sscanf(buffer, "alphaChnl=%d", &auxInt);
		commonTag_.alphaChnl = ChannelData(auxInt);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "redChnl", 7) == 0)
	{
		sscanf(buffer, "redChnl=%d", &auxInt);
		commonTag_.redChnl = ChannelData(auxInt);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "greenChnl", 9) == 0)
	{
		sscanf(buffer, "greenChnl=%d", &auxInt);
		commonTag_.greenChnl = ChannelData(auxInt);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "blueChnl", 8) == 0)
	{
		sscanf(buffer, "blueChnl=%d", &auxInt);
		commonTag_.blueChnl = ChannelData(auxInt);
		buffer = nextField(buffer);
	}
}

void FntParser::parsePageTag(const char *buffer, unsigned int index)
{
	buffer = nextField(buffer);

	if (strncmp(buffer, "id", 2) == 0)
	{
		sscanf(buffer, "id=%d", &pageTags_[index].id);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "file", 4) == 0)
	{
		extractValueWithSpaces(buffer, pageTags_[index].file);
		buffer = nextField(buffer);
	}
}

void FntParser::parseCharsTag(const char *buffer)
{
	buffer = nextField(buffer);

	if (strncmp(buffer, "count", 5) == 0)
	{
		sscanf(buffer, "count=%d", &charsTag_.count);
		buffer = nextField(buffer);
	}
}

void FntParser::parseCharTag(const char *buffer, unsigned int index)
{
	int auxInt = 0;
	buffer = nextField(buffer);

	if (strncmp(buffer, "id", 2) == 0)
	{
		sscanf(buffer, "id=%d", &charTags_[index].id);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "x", 1) == 0)
	{
		sscanf(buffer, "x=%d", &charTags_[index].x);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "y", 1) == 0)
	{
		sscanf(buffer, "y=%d", &charTags_[index].y);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "width", 5) == 0)
	{
		sscanf(buffer, "width=%d", &charTags_[index].width);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "height", 6) == 0)
	{
		sscanf(buffer, "height=%d", &charTags_[index].height);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "xoffset", 7) == 0)
	{
		sscanf(buffer, "xoffset=%d", &charTags_[index].xoffset);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "yoffset", 7) == 0)
	{
		sscanf(buffer, "yoffset=%d", &charTags_[index].yoffset);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "xadvance", 8) == 0)
	{
		sscanf(buffer, "xadvance=%d", &charTags_[index].xadvance);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "page", 4) == 0)
	{
		sscanf(buffer, "page=%d", &charTags_[index].page);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "chnl", 4) == 0)
	{
		sscanf(buffer, "chnl=%d", &auxInt);
		charTags_[index].chnl = CharChannel(auxInt);
		buffer = nextField(buffer);
	}
}

void FntParser::parseKerningsTag(const char *buffer)
{
	buffer = nextField(buffer);

	if (strncmp(buffer, "count", 5) == 0)
	{
		sscanf(buffer, "count=%d", &kerningsTag_.count);
		buffer = nextField(buffer);
	}
}

void FntParser::parseKerningTag(const char *buffer, unsigned int index)
{
	buffer = nextField(buffer);

	if (strncmp(buffer, "first", 5) == 0)
	{
		sscanf(buffer, "first=%d", &kerningTags_[index].first);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "second", 6) == 0)
	{
		sscanf(buffer, "second=%d", &kerningTags_[index].second);
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "amount", 6) == 0)
	{
		sscanf(buffer, "amount=%d", &kerningTags_[index].amount);
		buffer = nextField(buffer);
	}
}

const char *FntParser::nextField(const char *buffer) const
{
	ASSERT(*buffer != ' ' && *buffer != '\t' && *buffer != '\n');
	const char *next = buffer;

	bool needEqualSign = true;
	if (strncmp(buffer, "info", 4) == 0 ||
	    strncmp(buffer, "common", 6) == 0 ||
	    strncmp(buffer, "page", 4) == 0 ||
	    strncmp(buffer, "chars", 5) == 0 ||
	    strncmp(buffer, "char", 4) == 0 ||
	    strncmp(buffer, "kernings", 8) == 0 ||
	    strncmp(buffer, "kerning", 7) == 0)
	{
		needEqualSign = false;
	}

	bool hasEqualSign = false;
	int numDoubleQuotes = 0;
	while ((*next != ' ' && *next != '\t' && *next != '\n') || numDoubleQuotes == 1)
	{
		if (*next == '=')
			hasEqualSign = true;
		if (hasEqualSign && *next == '\"')
			numDoubleQuotes++;
		next++;
	}

	while ((*next == ' ' || *next == '\t') && *next != '\n')
		next++;

	// A field name is followed by an equal sign and a value
	ASSERT(!needEqualSign || hasEqualSign);

	return next;
}

bool FntParser::extractValueWithSpaces(const char *buffer, nctl::String &string) const
{
	bool extracted = false;

	const char *start = strchr(buffer, '"') + 1;
	const char *end = nullptr;

	if (start)
		end = strchr(start, '"') - 1;

	if (end)
	{
		const unsigned int length = end - start + 1;
		if (length > 0)
			string.assign(start, length);
		extracted = true;
	}

	return extracted;
}

}
