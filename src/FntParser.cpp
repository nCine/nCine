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

#ifdef _WIN32
	// Use binary mode on Windows to properly count '\r' characters
	const unsigned char openMode = IFile::OpenMode::READ | IFile::OpenMode::BINARY;
#else
	const unsigned char openMode = IFile::OpenMode::READ;
#endif
	fileHandle->open(openMode);

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
		else if (strncmp(buffer, "page", 4) == 0 && numPageTags_ < commonTag_.pages && numPageTags_ < MaxPageTags)
			parsePageTag(buffer, numPageTags_++);
		else if (strncmp(buffer, "chars", 5) == 0)
			parseCharsTag(buffer);
		// Some exporters, like AndryBlack's FontBuilder, don't output a `chars` tag
		else if (strncmp(buffer, "char", 4) == 0 && (numCharTags_ < charsTag_.count || charsTag_.count == 0) && numCharTags_ < MaxCharTags)
			parseCharTag(buffer, numCharTags_++);
		else if (strncmp(buffer, "kernings", 8) == 0)
			parseKerningsTag(buffer);
		else if (strncmp(buffer, "kerning", 7) == 0 && numKerningTags_ < kerningsTag_.count && numKerningTags_ < MaxKerningTags)
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
		const bool extracted = extractValueWithSpaces(buffer, infoTag_.face);
		if (extracted == false)
			LOGW("Error while parsing the \"face\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "size", 4) == 0)
	{
		const int assignedValues = sscanf(buffer, "size=%d", &infoTag_.size);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"size\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "bold", 4) == 0)
	{
		const int assignedValues = sscanf(buffer, "bold=%d", &auxInt);
		if (assignedValues == 1)
			infoTag_.bold = auxInt ? true : false;
		else
			LOGW("Error while parsing the \"bold\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "italic", 6) == 0)
	{
		const int assignedValues = sscanf(buffer, "italic=%d", &auxInt);
		if (assignedValues == 1)
			infoTag_.italic = auxInt ? true : false;
		else
			LOGW("Error while parsing the \"italic\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "charset", 7) == 0)
	{
		const bool extracted = extractValueWithSpaces(buffer, infoTag_.charset);
		if (extracted == false)
			LOGW("Error while parsing the \"charset\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "unicode", 7) == 0)
	{
		const int assignedValues = sscanf(buffer, "unicode=%d", &auxInt);
		if (assignedValues == 1)
			infoTag_.unicode = auxInt ? true : false;
		else
			LOGW("Error while parsing the \"unicode\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "stretchH", 8) == 0)
	{
		const int assignedValues = sscanf(buffer, "stretchH=%d", &infoTag_.stretchH);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"stretchH\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "smooth", 6) == 0)
	{
		const int assignedValues = sscanf(buffer, "smooth=%d", &auxInt);
		if (assignedValues == 1)
			infoTag_.smooth = auxInt ? true : false;
		else
			LOGW("Error while parsing the \"smooth\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "aa", 2) == 0)
	{
		const int assignedValues = sscanf(buffer, "aa=%d", &infoTag_.aa);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"aa\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "padding", 7) == 0)
	{
		const int assignedValues = sscanf(buffer, "padding=%d,%d,%d,%d", &infoTag_.paddingUp, &infoTag_.paddingRight, &infoTag_.paddingDown, &infoTag_.paddingLeft);
		if (assignedValues != 4)
			LOGW("Error while parsing the \"padding\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "spacing", 7) == 0)
	{
		const int assignedValues = sscanf(buffer, "spacing=%d,%d", &infoTag_.hSpacing, &infoTag_.vSpacing);
		if (assignedValues != 2)
			LOGW("Error while parsing the \"spacing\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "outline", 7) == 0)
	{
		const int assignedValues = sscanf(buffer, "outline=%d", &infoTag_.outline);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"outline\" field of the \"info\" tag");
		buffer = nextField(buffer);
	}
}

void FntParser::parseCommonTag(const char *buffer)
{
	int auxInt = 0;
	buffer = nextField(buffer);

	if (strncmp(buffer, "lineHeight", 10) == 0)
	{
		const int assignedValues = sscanf(buffer, "lineHeight=%d", &commonTag_.lineHeight);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"lineHeight\" field of the \"common\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "base", 4) == 0)
	{
		const int assignedValues = sscanf(buffer, "base=%d", &commonTag_.base);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"base\" field of the \"common\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "scaleW", 6) == 0)
	{
		const int assignedValues = sscanf(buffer, "scaleW=%d", &commonTag_.scaleW);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"scaleW\" field of the \"common\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "scaleH", 6) == 0)
	{
		const int assignedValues = sscanf(buffer, "scaleH=%d", &commonTag_.scaleH);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"scaleH\" field of the \"common\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "pages", 5) == 0)
	{
		const int assignedValues = sscanf(buffer, "pages=%d", &commonTag_.pages);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"pages\" field of the \"common\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "packed", 6) == 0)
	{
		const int assignedValues = sscanf(buffer, "packed=%d", &auxInt);
		if (assignedValues == 1)
			commonTag_.packed = auxInt ? true : false;
		else
			LOGW("Error while parsing the \"packed\" field of the \"common\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "alphaChnl", 9) == 0)
	{
		const int assignedValues = sscanf(buffer, "alphaChnl=%d", &auxInt);
		if (assignedValues == 1)
			commonTag_.alphaChnl = ChannelData(auxInt);
		else
			LOGW("Error while parsing the \"alphaChnl\" field of the \"common\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "redChnl", 7) == 0)
	{
		const int assignedValues = sscanf(buffer, "redChnl=%d", &auxInt);
		if (assignedValues == 1)
			commonTag_.redChnl = ChannelData(auxInt);
		else
			LOGW("Error while parsing the \"redChnl\" field of the \"common\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "greenChnl", 9) == 0)
	{
		const int assignedValues = sscanf(buffer, "greenChnl=%d", &auxInt);
		if (assignedValues == 1)
			commonTag_.greenChnl = ChannelData(auxInt);
		else
			LOGW("Error while parsing the \"greenChnl\" field of the \"common\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "blueChnl", 8) == 0)
	{
		const int assignedValues = sscanf(buffer, "blueChnl=%d", &auxInt);
		if (assignedValues == 1)
			commonTag_.blueChnl = ChannelData(auxInt);
		else
			LOGW("Error while parsing the \"blueChnl\" field of the \"common\" tag");
		buffer = nextField(buffer);
	}
}

void FntParser::parsePageTag(const char *buffer, unsigned int index)
{
	buffer = nextField(buffer);

	if (strncmp(buffer, "id", 2) == 0)
	{
		const int assignedValues = sscanf(buffer, "id=%d", &pageTags_[index].id);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"id\" field of the \"page\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "file", 4) == 0)
	{
		const bool extracted = extractValueWithSpaces(buffer, pageTags_[index].file);
		if (extracted == false)
			LOGW("Error while parsing the \"file\" field of the \"page\" tag");
		buffer = nextField(buffer);
	}
}

void FntParser::parseCharsTag(const char *buffer)
{
	buffer = nextField(buffer);

	if (strncmp(buffer, "count", 5) == 0)
	{
		const int assignedValues = sscanf(buffer, "count=%d", &charsTag_.count);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"count\" field of the \"chars\" tag");
		buffer = nextField(buffer);
	}
}

void FntParser::parseCharTag(const char *buffer, unsigned int index)
{
	int auxInt = 0;
	buffer = nextField(buffer);

	if (strncmp(buffer, "id", 2) == 0)
	{
		const int assignedValues = sscanf(buffer, "id=%d", &charTags_[index].id);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"id\" field of the \"char\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "x", 1) == 0)
	{
		const int assignedValues = sscanf(buffer, "x=%d", &charTags_[index].x);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"x\" field of the \"char\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "y", 1) == 0)
	{
		const int assignedValues = sscanf(buffer, "y=%d", &charTags_[index].y);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"y\" field of the \"char\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "width", 5) == 0)
	{
		const int assignedValues = sscanf(buffer, "width=%d", &charTags_[index].width);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"width\" field of the \"char\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "height", 6) == 0)
	{
		const int assignedValues = sscanf(buffer, "height=%d", &charTags_[index].height);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"height\" field of the \"char\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "xoffset", 7) == 0)
	{
		const int assignedValues = sscanf(buffer, "xoffset=%d", &charTags_[index].xoffset);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"xoffset\" field of the \"char\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "yoffset", 7) == 0)
	{
		const int assignedValues = sscanf(buffer, "yoffset=%d", &charTags_[index].yoffset);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"yoffset\" field of the \"char\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "xadvance", 8) == 0)
	{
		const int assignedValues = sscanf(buffer, "xadvance=%d", &charTags_[index].xadvance);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"xadvance\" field of the \"char\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "page", 4) == 0)
	{
		const int assignedValues = sscanf(buffer, "page=%d", &charTags_[index].page);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"page\" field of the \"char\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "chnl", 4) == 0)
	{
		const int assignedValues = sscanf(buffer, "chnl=%d", &auxInt);
		if (assignedValues == 1)
			charTags_[index].chnl = CharChannel(auxInt);
		else
			LOGW("Error while parsing the \"chnl\" field of the \"char\" tag");
		buffer = nextField(buffer);
	}
}

void FntParser::parseKerningsTag(const char *buffer)
{
	buffer = nextField(buffer);

	if (strncmp(buffer, "count", 5) == 0)
	{
		const int assignedValues = sscanf(buffer, "count=%d", &kerningsTag_.count);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"count\" field of the \"kernings\" tag");
		buffer = nextField(buffer);
	}
}

void FntParser::parseKerningTag(const char *buffer, unsigned int index)
{
	buffer = nextField(buffer);

	if (strncmp(buffer, "first", 5) == 0)
	{
		const int assignedValues = sscanf(buffer, "first=%d", &kerningTags_[index].first);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"first\" field of the \"kerning\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "second", 6) == 0)
	{
		const int assignedValues = sscanf(buffer, "second=%d", &kerningTags_[index].second);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"second\" field of the \"kerning\" tag");
		buffer = nextField(buffer);
	}

	if (strncmp(buffer, "amount", 6) == 0)
	{
		const int assignedValues = sscanf(buffer, "amount=%d", &kerningTags_[index].amount);
		if (assignedValues != 1)
			LOGW("Error while parsing the \"amount\" field of the \"kerning\" tag");
		buffer = nextField(buffer);
	}
}

const char *FntParser::nextField(const char *buffer) const
{
	const bool isNotWhiteSpace = (*buffer != ' ' && *buffer != '\t' && *buffer != '\n');
	if (isNotWhiteSpace == false)
		LOGW("There have been an error while searching for the next field");
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
	const bool equalSignCondition = (!needEqualSign || hasEqualSign);
	if (equalSignCondition == false)
		LOGW("There have been an error with the equal sign condition");

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
