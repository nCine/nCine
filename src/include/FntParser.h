#ifndef CLASS_NCINE_FNTPARSER
#define CLASS_NCINE_FNTPARSER

#include <nctl/String.h>

namespace ncine {

class IFile;

/// FNT format parser (<em>AngelCode's Bitmap Font Generator</em>)
class DLL_PUBLIC FntParser
{
  public:
	static const unsigned int MaxFaceNameLength = 64;
	static const unsigned int MaxCharsetNameLength = 8;
	static const unsigned int MaxFileNameLength = 128;

	struct InfoTag
	{
		nctl::String face = nctl::String(MaxFaceNameLength);
		int size = 0;
		bool bold = false;
		bool italic = false;
		nctl::String charset = nctl::String(MaxCharsetNameLength);
		bool unicode = false;
		int stretchH = 0;
		bool smooth = false;
		int aa = 1;
		int paddingUp = 0;
		int paddingRight = 0;
		int paddingDown = 0;
		int paddingLeft = 0;
		int hSpacing = 0;
		int vSpacing = 0;
		int outline = 0;
	};

	enum class ChannelData : unsigned char
	{
		GLYPH = 0,
		OUTLINE = 1,
		GLYPH_AND_OUTLINE = 2,
		ZERO = 3,
		ONE = 4,
		MISSING = 255
	};

	struct CommonTag
	{
		int lineHeight = 0;
		int base = 0;
		int scaleW = 0;
		int scaleH = 0;
		int pages = 0;
		bool packed = false;
		ChannelData alphaChnl = ChannelData::MISSING;
		ChannelData redChnl = ChannelData::MISSING;
		ChannelData greenChnl = ChannelData::MISSING;
		ChannelData blueChnl = ChannelData::MISSING;
	};

	struct PageTag
	{
		int id = 0;
		nctl::String file = nctl::String(MaxFileNameLength);
	};

	struct CharsTag
	{
		int count = 0;
	};

	enum class CharChannel : unsigned char
	{
		BLUE = 1,
		GREEN = 2,
		RED = 4,
		ALPHA = 8,
		ALL = 15,
		MISSING = 255
	};

	struct CharTag
	{
		int id = 0;
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
		int xoffset = 0;
		int yoffset = 0;
		int xadvance = 0;
		int page = 0;
		CharChannel chnl = CharChannel::MISSING;
	};

	struct KerningsTag
	{
		int count = 0;
	};

	struct KerningTag
	{
		int first = 0;
		int second = 0;
		int amount = 0;
	};

	/// Loads a FNT file in a memory buffer then parses it
	explicit FntParser(const char *fntFilename);
	/// Parses a FNT file from a memory buffer of the specified size
	FntParser(const char *buffer, long int size);

	/// Returns the "info" tag structure from a parsed FNT file
	const InfoTag &infoTag() const { return infoTag_; }
	/// Returns the "common" tag structure from a parsed FNT file
	const CommonTag &commonTag() const { return commonTag_; }
	/// Returns the number of parsed "page" tag structures
	unsigned int numPageTags() const { return numPageTags_; }
	/// Returns the specified "page" tag structure from a parsed FNT file
	const PageTag &pageTag(unsigned int index) const
	{
		FATAL_ASSERT(index < numPageTags_);
		return pageTags_[index];
	}
	/// Returns the "chars" tag structure from a parsed FNT file
	const CharsTag &charsTag() const { return charsTag_; }
	/// Returns the number of parsed "char" tag structures
	unsigned int numCharTags() const { return numCharTags_; }
	/// Returns the specified "char" tag structure from a parsed FNT file
	const CharTag &charTag(unsigned int index) const
	{
		FATAL_ASSERT(index < numCharTags_);
		return charTags_[index];
	}
	/// Returns the "kernings" tag structure from a parsed FNT file
	const KerningsTag &kerningsTag() const { return kerningsTag_; }
	/// Returns the number of parsed "kerning" tag structures
	unsigned int numKerningTags() const { return numKerningTags_; }
	/// Returns the specified "kerning" tag structure from a parsed FNT file
	const KerningTag &kerningTag(unsigned int index) const
	{
		FATAL_ASSERT(index < numKerningTags_);
		return kerningTags_[index];
	}

  private:
	static const int MaxPageTags = 1;
	static const int MaxCharTags = 256;
	static const int MaxKerningTags = 512;

	/// Parsed "info" tag from the FNT file
	InfoTag infoTag_;
	/// Parsed "common" tag from the FNT file
	CommonTag commonTag_;
	/// Parsed "page" tags from the FNT file
	PageTag pageTags_[MaxPageTags];
	/// Parsed "chars" tag from the FNT file
	CharsTag charsTag_;
	/// Parsed "char" tags from the FNT file
	CharTag charTags_[MaxCharTags];
	/// Parsed "kernings" tag from the FNT file
	KerningsTag kerningsTag_;
	/// Parsed "kerning" tags from the FNT file
	KerningTag kerningTags_[MaxKerningTags];

	unsigned int numPageTags_;
	unsigned int numCharTags_;
	unsigned int numKerningTags_;

	/// Loads a FNT file in a memory buffer then parses it
	void parseFntFile(IFile *fileHandle);
	/// Parses a FNT file from a memory buffer of the specified size
	void parseFntBuffer(const char *buffer, long int size);

	void parseInfoTag(const char *buffer);
	void parseCommonTag(const char *buffer);
	void parsePageTag(const char *buffer, unsigned int index);
	void parseCharsTag(const char *buffer);
	void parseCharTag(const char *buffer, unsigned int index);
	void parseKerningsTag(const char *buffer);
	void parseKerningTag(const char *buffer, unsigned int index);

	/// Goes to the next field in a tag, skipping white spaces
	const char *nextField(const char *buffer) const;
	/// Extracts a value containing spaces into a string
	bool extractValueWithSpaces(const char *buffer, nctl::String &string) const;
};

}

#endif
