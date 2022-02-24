#ifndef CLASS_NCTL_UTF8
#define CLASS_NCTL_UTF8

#include <ncine/common_defines.h>

namespace nctl {

namespace Utf8 {

	/// Unicode replacement character used as an invalid code point
	static const unsigned int InvalidUnicode = 0xfffd;
	/// UTF-8 encoded version of the Unicode replacement character
	static const unsigned int InvalidUtf8 = 0xefbfbd;

	/// Retrieves the Unicode code point and the UTF-8 code units from the UTF-8 C substring
	/*! \returns A pointer to the C substring following the decoded UTF-8 code units */
	DLL_PUBLIC const char *utf8ToCodePoint(const char *substring, unsigned int &codePoint, unsigned int *codeUnits);
	/// Retrieves the Unicode code point from the UTF-8 C substring
	/*! \returns A pointer to the C substring following the decoded UTF-8 code units */
	DLL_PUBLIC const char *utf8ToCodePoint(const char *substring, unsigned int &codePoint);

	/// Encodes a Unicode code point to a UTF-8 C substring and code units
	/*! \returns The number of characters used to encode a valid code point */
	DLL_PUBLIC int codePointToUtf8(unsigned int codePoint, char *substring, unsigned int *codeUnits);
}

}

#endif
